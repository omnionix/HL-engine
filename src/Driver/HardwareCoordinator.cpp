#include "stdafx.h"
#include "HardwareCoordinator.h"
#include <iostream>
#include "EventDispatcher.h"
#include "DeviceContainer.h"
#include "PluginAPI.h"
#include "Device.h"
#include "DriverMessenger.h"
#include "SharedTypes.h"
#include <boost/variant.hpp>
HardwareCoordinator::HardwareCoordinator(boost::asio::io_service& io, DriverMessenger& messenger, DeviceContainer& devices )
	: m_devices(devices)
	, m_messenger(messenger)
	, m_writeBodyRepresentation(io, boost::posix_time::milliseconds(8))
{
	m_devices.OnDeviceAdded([this](Device* device) {
	
		NullSpace::SharedMemory::DeviceInfo info = {0};
		info.Id = device->id();

		std::string strName = device->name();
		std::copy(strName.begin(), strName.end(), info.DeviceName);
		info.Status = Connected;
		info.Concept = static_cast<uint32_t>(device->concept());
		m_messenger.WriteDevice(info);

		device->ForEachNode([this, device](Node* node) {
			
			NullSpace::SharedMemory::NodeInfo info = { 0 };
			info.Id = ((uint64_t)(device->id()) << 32) | node->id();
			std::string nodeName = node->name();
			std::copy(nodeName.begin(), nodeName.end(), info.NodeName);
			info.Type = node->type();
			m_messenger.WriteNode(info);
		});

	});

	m_devices.OnDeviceRemoved([this](Device* device) {
		m_messenger.UpdateDeviceStatus(device->id(), DeviceStatus::Disconnected);
		device->ForEachNode([this, device](Node* node) {
			uint64_t externalId  = ((uint64_t)(device->id()) << 32) | node->id();

			m_messenger.RemoveNode(externalId);
		});
	});

	m_writeBodyRepresentation.SetEvent([this]() { this->writeBodyRepresentation(); });
	m_writeBodyRepresentation.Start();

	
}



void HardwareCoordinator::writeTracking(nsvr_node_id node_id, nsvr_quaternion * quat)
{
	//todo: we need to actually take the quaternion arriving from the device and translate it to a region, based on the BodyGraph
	//todo: we need synchronization
	m_messenger.WriteTracking(node_id, NullSpace::SharedMemory::Quaternion{ quat->x, quat->y, quat->z, quat->w });
}

void HardwareCoordinator::writeBodyRepresentation()
{
	m_devices.EachSimulation([&messenger = m_messenger](SimulatedDevice* device) {
		device->simulate(.008);

		auto nodeView = device->render();

		for (const auto& node : nodeView) {
			for (const auto& single : node.nodes) {
				NullSpace::SharedMemory::RegionPair pair;
				pair.Type = static_cast<uint32_t>(single.Type);
				pair.Region = node.region;
				pair.Id = single.Id;

				pair.Value = NullSpace::SharedMemory::Data{
					single.DisplayData.data_0,
					single.DisplayData.data_1,
					single.DisplayData.data_2,
					single.DisplayData.intensity };
				messenger.WriteBodyView(std::move(pair));
			}
		}
	});
	
	
}


void HardwareCoordinator::SetupSubscriptions(EventDispatcher& sdkEvents)
{
	// For now, I'm simply forwarding the relevant events to all the devices
	// More complex behavior later
	
	sdkEvents.Subscribe(
	{ 
		NullSpaceIPC::HighLevelEvent::kSimpleHaptic,
		NullSpaceIPC::HighLevelEvent::kPlaybackEvent,
		NullSpaceIPC::HighLevelEvent::kRealtimeHaptic,
		NullSpaceIPC::HighLevelEvent::kCurveHaptic
	}, 
	[&](const NullSpaceIPC::HighLevelEvent& event) {
		m_devices.EachDevice([&](Device* device) {
			device->DispatchEvent(event);
		});
	});

	/*sdkEvents.Subscribe(NullSpaceIPC::HighLevelEvent::kSimpleHaptic, [&](const NullSpaceIPC::HighLevelEvent& event) {
		BOOST_LOG_TRIVIAL(info) << "Got haptic";
		m_devices.EachDevice([&](Device* device) {
			device->DispatchEvent(event);
		});
	});

	sdkEvents.Subscribe(NullSpaceIPC::HighLevelEvent::kPlaybackEvent, [&](const NullSpaceIPC::HighLevelEvent& event) {
		m_devices.EachDevice([&](Device* device) {
			device->DispatchEvent(event);
		});
	});

	sdkEvents.Subscribe(NullSpaceIPC::HighLevelEvent::kRealtimeHaptic, [&](const NullSpaceIPC::HighLevelEvent& event) {
		m_devices.EachDevice([&](Device* device) {
			device->DispatchEvent(event);
		});
	});

	sdkEvents.Subscribe(NullSpaceIPC::HighLevelEvent::kCurveHaptic, [&](const NullSpaceIPC::HighLevelEvent& event) {
		m_devices.EachDevice([&](Device* device) {
			device->DispatchEvent(event);
		});
	});
*/
	
	
}



