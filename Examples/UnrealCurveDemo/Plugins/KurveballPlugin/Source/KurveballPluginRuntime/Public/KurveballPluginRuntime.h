#pragma once

#include "Modules/ModuleManager.h"

class KURVEBALLPLUGINRUNTIME_API FKurveballPluginRuntimeModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};