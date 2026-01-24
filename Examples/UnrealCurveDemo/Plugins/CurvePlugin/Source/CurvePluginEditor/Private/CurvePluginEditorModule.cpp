#include "CurvePluginEditorModule.h"
#include "CurvePluginEditor.h"

IMPLEMENT_MODULE(FCurvePluginEditorModule, FCurvePluginEditor)

void FCurvePluginEditorModule::StartupModule()
{
	// Add OnPostEngineInit delegate.
	OnPostEngineInitDelegateHandle = FCoreDelegates::OnPostEngineInit.AddRaw(this, &FCurvePluginEditorModule::OnPostEngineInit);
}

void FCurvePluginEditorModule::ShutdownModule()
{
	// Remove OnPostEngineInit delegate
	FCoreDelegates::OnPostEngineInit.Remove(OnPostEngineInitDelegateHandle);
}

void FCurvePluginEditorModule::OnPostEngineInit()
{
    // Do post engine stuff here
}
