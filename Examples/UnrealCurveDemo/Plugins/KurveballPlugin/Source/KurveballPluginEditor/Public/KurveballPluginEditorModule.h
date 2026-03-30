#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class KURVEBALLPLUGINEDITOR_API FKurveballPluginEditorModule : public IModuleInterface
{

public:

	/** IModuleInterface implementation */
	void StartupModule() override;
	void ShutdownModule() override;

protected:

	/**
	 * Do stuff after engine is initialized.
	 */
	void OnPostEngineInit();

private:

	/** OnPostEngineInit delegate handler. */
	FDelegateHandle OnPostEngineInitDelegateHandle;
};
