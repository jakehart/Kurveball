#include "CurvePluginEditorModule.h"
#include "CurvePluginEditor.h"

IMPLEMENT_MODULE(FCurvePluginEditorModule, FCurvePluginEditor)

namespace
{
    static const FName sMenuReferenceName = TEXT("Curve Mechanic");
    static const FText sMenuFriendlyName = INVTEXT("Curve Mechanic");
}

void FCurvePluginEditorModule::StartupModule()
{
	OnPostEngineInitDelegateHandle = FCoreDelegates::OnPostEngineInit.AddRaw(this, &FCurvePluginEditorModule::OnPostEngineInit);

    UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AddNewContextMenu");
    FToolMenuSection& Section = Menu->AddSection("SectionInternalName", INVTEXT("Velocity Curve Mechanics"));

    auto menuSetupFunction = FNewToolMenuDelegate::CreateLambda([](UToolMenu* Menu)
        {
            Menu->AddMenuEntry(sMenuReferenceName, FToolMenuEntry::InitMenuEntry(
                NAME_None,
                FUIAction(
                    FExecuteAction::CreateLambda([]()
                        {
                            //This code will run when the submenu item is clicked
                        }),
                        //You can pass in a delegate which is used to test if the
                        //submenu action should be allowed to execute
                    FCanExecuteAction()
                    ),
                //Slate content for the new menu item
                SNew(STextBlock).Text(INVTEXT("Curve!!!"))
            ));

        });

    Section.AddEntry(FToolMenuEntry::InitMenuEntry(
        NAME_None,
        FUIAction(
            FExecuteAction::CreateLambda([]()
                {
                    //This code will run when the submenu item is clicked
                }),
                //You can pass in a delegate which is used to test if the
                //submenu action should be allowed to execute
            FCanExecuteAction()
            ),
        //Slate content for the new menu item
        SNew(STextBlock).Text(INVTEXT("Curve!!!"))
    ));

}

void FCurvePluginEditorModule::ShutdownModule()
{
	FCoreDelegates::OnPostEngineInit.Remove(OnPostEngineInitDelegateHandle);

    if (auto* ToolMenus = UToolMenus::Get())
    {
        if (UToolMenu* Menu = ToolMenus->ExtendMenu("ContentBrowser.AddNewContextMenu"))
        {
            Menu->RemoveSection(sMenuReferenceName);
        }
    }
}

void FCurvePluginEditorModule::OnPostEngineInit()
{
    // Do post engine stuff here
}
