#include "KurveballPluginEditorModule.h"
#include "KurveballPluginEditor.h"

#include "AssetToolsModule.h"
#include "Dialogs/DlgPickAssetPath.h"
#include "Engine/DataAsset.h"
#include "Engine/PrimaryAssetLabel.h"
#include "Factories/DataAssetFactory.h"

#include "UnrealWrapper/UCurveMechanic.h"

IMPLEMENT_MODULE(FKurveballPluginEditorModule, FKurveballPluginEditor)

namespace
{
    static const FName sMenuReferenceName = TEXT("NewCurveMechanicMenuItem");
    static const FText sMenuFriendlyName = INVTEXT("New Velocity Curve Mechanic");

    static const FName sSectionReferenceName = TEXT("CurveMechanicsSection");
    static const FText sSectionFriendlyName = INVTEXT("Velocity Curve Mechanics");
}

void FKurveballPluginEditorModule::StartupModule()
{
	OnPostEngineInitDelegateHandle = FCoreDelegates::OnPostEngineInit.AddRaw(this, &FKurveballPluginEditorModule::OnPostEngineInit);

    UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AddNewContextMenu");
    FToolMenuSection& Section = Menu->AddSection(sSectionReferenceName, sSectionFriendlyName);
    
    Section.AddEntry(FToolMenuEntry::InitMenuEntry(
        sMenuReferenceName,
        FUIAction(
            FExecuteAction::CreateLambda([]()
                {
                    TSharedPtr<SDlgPickAssetPath> PickAssetPathWidget =
                        SNew(SDlgPickAssetPath)
                        .Title(INVTEXT("Choose mechanic name and location"))
                        .DefaultAssetPath(FText::FromString("/Game/Mechanics/NewMovementMechanic"));

                    if (PickAssetPathWidget->ShowModal() == EAppReturnType::Ok)
                    {
                        FString path = PickAssetPathWidget->GetAssetPath().ToString();
                        FName assetName(PickAssetPathWidget->GetAssetName().ToString());

                        UCurveMechanic* mechanic = static_cast<UCurveMechanic*>(FAssetToolsModule::GetModule().Get().CreateAsset(
                                                                assetName.ToString(),
                                                                path,
                                                                UCurveMechanic::StaticClass(),
                                                                nullptr // Factory is optional for simple types
                        ));

                        const auto curveName = assetName.ToString() + "Curve";
                        UCurveFloat* curve = static_cast<UCurveFloat*>(FAssetToolsModule::GetModule().Get().CreateAsset(
                            curveName,
                            path,
                            UCurveFloat::StaticClass(),
                            nullptr
                        ));

                        mechanic->VelocityCurveAsset = curve;
                        mechanic->CurveInstanceName = assetName;
                    }
                }),
            FCanExecuteAction()
            ),
        // Slate content: the visuals of the menu item
        SNew(STextBlock).Text(sMenuFriendlyName)
    ));

}

void FKurveballPluginEditorModule::ShutdownModule()
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

void FKurveballPluginEditorModule::OnPostEngineInit()
{
    // Do post engine stuff here
}
