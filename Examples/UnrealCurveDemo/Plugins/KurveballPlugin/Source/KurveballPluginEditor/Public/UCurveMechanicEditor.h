#pragma once
#include "Toolkits/AssetEditorToolkit.h"
#include "IDetailsView.h"

class UCurveFloat;
class UCurveMechanic;

class KURVEBALLPLUGINEDITOR_API UCurveMechanicEditor : public FAssetEditorToolkit
{
public:
    void InitCurveMechanicEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UCurveMechanic* InMechanic);

    virtual FText GetToolkitName() const override { return FText::FromString("Curve Mechanic Editor"); }
    virtual FName GetToolkitFName() const override { return FName("CurveMechanicEditor"); }

    // Get the widget
    TSharedPtr<SWidget> GetInlineContent() const override;
    void RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) override;

private:
    UCurveMechanic* Mechanic;
    TSharedPtr<IDetailsView> DetailsView;
    TSharedPtr<SWidget> CurveEditorWidget; // We will embed the curve editor here

    TSharedRef<SDockTab> SpawnCurveEditorTab(const FSpawnTabArgs& Args);
    TSharedRef<SDockTab> SpawnDetailsTab(const FSpawnTabArgs& Args);

};