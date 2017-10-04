#include "stdafx.h"
#include "NSDriverApi.h"
#include "Driver.h"
#include "Locator.h"
#include "EnumTranslator.h"
#include "logging_initialization.h"

#define AS_TYPE(Type, Obj) reinterpret_cast<Type *>(Obj)
#define AS_CTYPE(Type, Obj) reinterpret_cast<const Type *>(Obj)



NS_DRIVER_API NSVR_Driver_Context_t* __cdecl NSVR_Driver_Create()
{
	Locator::initialize();
	Locator::provide(new EnumTranslator());
	
	initialize_logging();

	return AS_TYPE(NSVR_Driver_Context_t, new Driver());
}

NS_DRIVER_API void __cdecl NSVR_Driver_Destroy(NSVR_Driver_Context_t* ptr)
{
	if (!ptr) {
		return;
	}

	delete AS_TYPE(Driver, ptr);
}

NS_DRIVER_API bool __cdecl NSVR_Driver_Shutdown(NSVR_Driver_Context_t * ptr)
{
	return AS_TYPE(Driver, ptr)->Shutdown();
}

NS_DRIVER_API void __cdecl NSVR_Driver_StartThread(NSVR_Driver_Context_t * ptr)
{
	AS_TYPE(Driver, ptr)->StartThread();
}

NS_DRIVER_API unsigned int __cdecl NSVR_Driver_GetVersion(void)
{
	return NS_DRIVER_API_VERSION;
}

NS_DRIVER_API int __cdecl NSVR_Driver_IsCompatibleDLL(void)
{
	unsigned int major = NSVR_Driver_GetVersion() >> 16;
	return major == NS_DRIVER_API_VERSION_MAJOR;
}

NS_DRIVER_API int __cdecl NSVR_Driver_SetupDiagnostics(NSVR_Driver_Context_t* ptr, NSVR_Diagnostics_Menu * api)
{
	AS_TYPE(Driver, ptr)->ProvideRenderingApi(api);
	return 1;
}

NS_DRIVER_API int __cdecl NSVR_Driver_DrawDiagnostics(NSVR_Driver_Context_t* ptr)
{
	AS_TYPE(Driver, ptr)->DrawDiagnostics();
	return 1;
}
