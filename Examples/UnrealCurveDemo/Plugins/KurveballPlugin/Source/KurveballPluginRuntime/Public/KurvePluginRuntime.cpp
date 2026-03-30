#include "KurveballPluginRuntime.h"
#include "Modules/ModuleManager.h"

// This macro registers the module with Unreal's module manager.
// FDefaultModuleImpl is a generic implementation that does nothing special.
IMPLEMENT_MODULE(FDefaultModuleImpl, KurveballPluginRuntime);

void FKurveballPluginRuntimeModule::StartupModule()
{
    // Optional: Code to run when the module loads (e.g., register commands)
}

void FKurveballPluginRuntimeModule::ShutdownModule()
{
    // Optional: Code to run when the module unloads
}