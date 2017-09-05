#include "stdafx.h"
#include "HardlightPlugin.h"
#include "PluginAPI.h"
#include <iostream>
#include <typeinfo>
#include "PluginAPIWrapper.h"
HardlightPlugin::HardlightPlugin() :
	m_io(std::make_shared<IoService>()),
	m_dispatcher(),
	m_adapter(std::make_unique<BoostSerialAdapter>(m_io->GetIOService())),
	m_firmware(m_adapter, m_io->GetIOService()),
	m_monitor(std::make_shared<KeepaliveMonitor>(m_io->GetIOService(), m_firmware)),
	m_synchronizer(std::make_unique<Synchronizer>(m_adapter->GetDataStream(), m_dispatcher, m_io->GetIOService())),
	m_device(),
	m_eventPull(m_io->GetIOService(), boost::posix_time::milliseconds(5)),
	m_imus(m_dispatcher),
	m_mockTracking(m_io->GetIOService(), boost::posix_time::millisec(16)),
	m_core{nullptr},
	m_trackingStream{nullptr}

{
	

	m_adapter->SetMonitor(m_monitor);

	m_monitor->OnReconnect([&]() {
		m_device.RaiseDeviceConnectionEvent(m_core);
	});

	m_monitor->OnDisconnect([&]() {
		m_device.RaiseDeviceDisconnectionEvent(m_core);
	});

	

	m_adapter->Connect();
	
	m_synchronizer->BeginSync();


	m_eventPull.SetEvent([&]() {
		constexpr auto ms_fraction_of_second = (1.0f / 1000.f);
		auto dt = 5 * ms_fraction_of_second;

	
		auto commands = m_device.GenerateHardwareCommands(dt);
		m_firmware.Execute(commands);

	
		
	});

	m_eventPull.Start();

	m_imus.OnTracking([&](const std::string& id, nsvr_quaternion quat) {

		if (m_trackingStream != nullptr) {

			nsvr_tracking_stream_push(m_trackingStream, &quat);

		}
	});

	
	m_imus.AssignMapping(0x12, Imu::Chest, "chest");

	
	m_mockTracking.SetEvent([&]() {
		static float begin = 0;
		
		packet fakeTrackingData;
		memset(&fakeTrackingData.raw, 0, 16);

		float ms = begin;

		fakeTrackingData.raw[2] = 0x99;
		fakeTrackingData.raw[11] = 0x12;

		memcpy(&fakeTrackingData.raw[3], &ms, sizeof(ms));
	//	fakeTrackingData.raw[7] = s.count() & 0x0F;
		m_dispatcher.Dispatch(fakeTrackingData);
		begin += 0.05f;
		if (begin >= 1.0) { begin = -1.0f; }
	
	});

	m_mockTracking.Start();

}

HardlightPlugin::~HardlightPlugin()
{
	m_eventPull.Stop();
	m_mockTracking.Stop();
	m_io->Shutdown();

}




struct bodygraph_region {
	nsvr_bodygraph_region* region;

	bodygraph_region() {
		nsvr_bodygraph_region_create(&region);
	}

	bodygraph_region& setLocation(nsvr_bodypart bodypart, double ratio, double rot) {
		nsvr_bodygraph_region_setlocation(region, bodypart, ratio, rot);
		return *this;
	}
	bodygraph_region& setDimensions(double width_cm, double height_cm) {
		nsvr_bodygraph_region_setboundingboxdimensions(region, width_cm, height_cm);
		return *this;
	}
	~bodygraph_region() {
		nsvr_bodygraph_region_destroy(&region);
		assert(region == nullptr);
	}
};

int HardlightPlugin::Configure(nsvr_core* core)
{

	


	m_core = core;
	m_device.Configure(core);

	nsvr_plugin_tracking_api tracking_api;
	tracking_api.beginstreaming_handler = [](nsvr_tracking_stream* stream, nsvr_node_id region, void* client_data) {
		AS_TYPE(HardlightPlugin, client_data)->BeginTracking(stream, region);
	};
	tracking_api.endstreaming_handler = [](nsvr_node_id region, void* client_data) {
		AS_TYPE(HardlightPlugin, client_data)->EndTracking(region);
	};
	tracking_api.client_data = this;
	nsvr_register_tracking_api(core, &tracking_api);
	

	nsvr_plugin_bodygraph_api body_api;
	body_api.setup_handler = [](nsvr_bodygraph* g, void* cd) {
		AS_TYPE(HardlightPlugin, cd)->SetupBodygraph(g);
	
	};
	body_api.client_data = this;
	nsvr_register_bodygraph_api(core, &body_api);


	return 1;
}

void HardlightPlugin::BeginTracking(nsvr_tracking_stream* stream, nsvr_node_id region)
{
	m_firmware.EnableTracking();
	m_trackingStream = stream;
}

void HardlightPlugin::EndTracking(nsvr_node_id region)
{
	m_firmware.DisableTracking();
	m_trackingStream = nullptr;
}

void HardlightPlugin::SetupBodygraph(nsvr_bodygraph * g)
{
	//Setup the initial zones

	//bodygraph_region chestActuator;
	//chestActuator
	//	.setLocation(nsvr_bodypart_torso, nsvr_location_highest, 350)
	//	.setDimensions(6, 8);

	//bodygraph_region upperAbActuator;
	//upperAbActuator
	//	.setLocation(nsvr_bodypart_torso, nsvr_location_middle + .1, 350)
	//	.setDimensions(6, 4);

	//bodygraph_region midAbActuator;
	//midAbActuator
	//	.setLocation(nsvr_bodypart_torso, nsvr_location_middle, 350)
	//	.setDimensions(6, 4);

	//bodygraph_region lowerAbActuator;
	//lowerAbActuator
	//	.setLocation(nsvr_bodypart_torso, nsvr_location_middle - .2, 350)
	//	.setDimensions(6, 4);

	//bodygraph_region shoulderActuator;
	//shoulderActuator
	//	.setLocation(nsvr_bodypart_upperarm_left, nsvr_location_highest, 180)
	//	.setDimensions(4, 4);

	//bodygraph_region upperArmActuator;
	//upperArmActuator
	//	.setLocation(nsvr_bodypart_upperarm_left, nsvr_location_middle, 180)
	//	.setDimensions(6, 4);

	//bodygraph_region forearmActuator;
	//forearmActuator
	//	.setLocation(nsvr_bodypart_lowerarm_left, nsvr_location_middle, 180)
	//	.setDimensions(6, 4);

	//bodygraph_region backActuator;
	//backActuator
	//	.setLocation(nsvr_bodypart_torso, nsvr_location_highest - .1, 185)
	//	.setDimensions(6, 4);


	////Create the left half of the body
	//nsvr_bodygraph_createnode(g, "Chest_Left", chestActuator.region);
	//nsvr_bodygraph_createnode(g, "Upper_Ab_Left", upperAbActuator.region);
	//nsvr_bodygraph_createnode(g, "Mid_Ab_Left", midAbActuator.region);
	//nsvr_bodygraph_createnode(g, "Lower_Ab_Left", lowerAbActuator.region);

	//nsvr_bodygraph_createnode(g, "Shoulder_Left", shoulderActuator.region);
	//nsvr_bodygraph_createnode(g, "Upper_Arm_Left", upperArmActuator.region);
	//nsvr_bodygraph_createnode(g, "Lower_Arm_Left", forearmActuator.region);

	//nsvr_bodygraph_createnode(g, "Back_Left", backActuator.region);


	////Move the zones to the right side
	//chestActuator.setLocation(nsvr_bodypart_torso, nsvr_location_highest,  10);
	//upperAbActuator.setLocation(nsvr_bodypart_torso, nsvr_location_middle + .1,  10);
	//midAbActuator.setLocation(nsvr_bodypart_torso, nsvr_location_middle, 10);
	//lowerAbActuator.setLocation(nsvr_bodypart_torso, nsvr_location_middle - .2, 10);
	//shoulderActuator.setLocation(nsvr_bodypart_upperarm_right, nsvr_location_highest, 180);
	//upperArmActuator.setLocation(nsvr_bodypart_upperarm_right, nsvr_location_middle, 180);
	//forearmActuator.setLocation(nsvr_bodypart_lowerarm_right, nsvr_location_middle, 180);
	//backActuator.setLocation(nsvr_bodypart_torso, nsvr_location_highest - .1, 175);

	////Create the right half of the body
	//nsvr_bodygraph_createnode(g, "Chest_Right", chestActuator.region);
	//nsvr_bodygraph_createnode(g, "Upper_Ab_Right", upperAbActuator.region);
	//nsvr_bodygraph_createnode(g, "Mid_Ab_Right", midAbActuator.region);
	//nsvr_bodygraph_createnode(g, "Lower_Ab_Right", lowerAbActuator.region);

	//nsvr_bodygraph_createnode(g, "Shoulder_Right", shoulderActuator.region);
	//nsvr_bodygraph_createnode(g, "Upper_Arm_Right", upperArmActuator.region);
	//nsvr_bodygraph_createnode(g, "Lower_Arm_Right", forearmActuator.region);

	//nsvr_bodygraph_createnode(g, "Back_Right", backActuator.region);

	////Connect all the zones, first the left
	//nsvr_bodygraph_connect(g, "Chest_Left", "Upper_Ab_Left");
	//nsvr_bodygraph_connect(g, "Upper_Ab_Left", "Mid_Ab_Left");
	//nsvr_bodygraph_connect(g, "Mid_Ab_Left", "Lower_Ab_Left");

	//nsvr_bodygraph_connect(g, "Chest_Left", "Shoulder_Left");
	//nsvr_bodygraph_connect(g, "Shoulder_Left", "Upper_Arm_Left");
	//nsvr_bodygraph_connect(g, "Upper_Arm_Left", "Lower_Arm_Left");

	//nsvr_bodygraph_connect(g, "Shoulder_Left", "Back_Left");


	////Then the right
	//nsvr_bodygraph_connect(g, "Chest_Right", "Upper_Ab_Right");
	//nsvr_bodygraph_connect(g, "Upper_Ab_Right", "Mid_Ab_Right");
	//nsvr_bodygraph_connect(g, "Mid_Ab_Right", "Lower_Ab_Right");

	//nsvr_bodygraph_connect(g, "Chest_Right", "Shoulder_Right");
	//nsvr_bodygraph_connect(g, "Shoulder_Right", "Upper_Arm_Right");
	//nsvr_bodygraph_connect(g, "Upper_Arm_Right", "Lower_Arm_Right");

	//nsvr_bodygraph_connect(g, "Shoulder_Right", "Back_Right");

	////Finally, cross connections
	//nsvr_bodygraph_connect(g, "Chest_Right", "Chest_Left");
	//nsvr_bodygraph_connect(g, "Upper_Ab_Right", "Upper_Ab_Left");
	//nsvr_bodygraph_connect(g, "Mid_Ab_Right", "Mid_Ab_Left");
	//nsvr_bodygraph_connect(g, "Lower_Ab_Right", "Lower_Ab_Left");

	//nsvr_bodygraph_connect(g, "Back_Left", "Back_Right");

	//Lastly, we want to associate the actual devices with these logical zones
	m_device.SetupDeviceAssociations(g);
	


}

