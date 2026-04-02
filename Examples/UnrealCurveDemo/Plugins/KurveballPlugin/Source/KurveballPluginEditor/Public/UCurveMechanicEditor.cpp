#include "UCurveMechanicEditor.h"
#include "PropertyEditorModule.h"
#include "SCurveEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SEditableTextBox.h"

// If SCurveEditor is internal (which it often is), the easiest way is to 
// create a "Splitter" with two tabs: one for the Curve, one for Details.

void UCurveMechanicEditor::InitCurveMechanicEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UCurveMechanic* InMechanic)
{
    Mechanic = InMechanic;

    auto Layout = FTabManager::NewLayout("CurveMechanicLayout")
        ->AddArea
        (
            FTabManager::NewPrimaryArea()
            ->SetOrientation(Orient_Horizontal)
            ->Split
            (
                FTabManager::NewSplitter()
                ->SetSizeCoefficient(0.5f)
            )
            ->Split
            (
                FTabManager::NewSplitter()
                ->SetSizeCoefficient(0.5f)
            )
        );

    InitAssetEditor(Mode, InitToolkitHost, TEXT("CurveMechanicEditorApp"), Layout, true, true, (UObject*)InMechanic);
}

TSharedPtr<SWidget> UCurveMechanicEditor::GetInlineContent() const
{
    return SNew(SHorizontalBox)
        + SHorizontalBox::Slot().FillWidth(1.0f)
        [
            SNew(STextBlock).Text(FText::FromString("Curve Placeholder"))
        ]
        + SHorizontalBox::Slot().FillWidth(1.0f)
        [
            // Just return the widget we created earlier
            DetailsView.ToSharedRef()
        ];
}

void UCurveMechanicEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
    FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
    
    TSharedRef<FWorkspaceItem> CurveMechanicGroup = InTabManager->AddLocalWorkspaceMenuCategory(INVTEXT("CurveMechanicGroup"));

    InTabManager->RegisterTabSpawner("CurveEditor", FOnSpawnTab::CreateRaw(this, &UCurveMechanicEditor::SpawnCurveEditorTab))
        .SetDisplayName(FText::FromString("Curve"))
        .SetGroup(CurveMechanicGroup);

    InTabManager->RegisterTabSpawner("Details", FOnSpawnTab::CreateRaw(this, &UCurveMechanicEditor::SpawnDetailsTab))
        .SetDisplayName(FText::FromString("Details"))
        .SetGroup(CurveMechanicGroup);
}

TSharedRef<SDockTab> UCurveMechanicEditor::SpawnCurveEditorTab(const FSpawnTabArgs& Args)
{
    // This is tricky. The standard Curve Editor is usually spawned by the engine.
    // If you can't access SCurveEditor directly, you might need to use the "Curve Editor" mode of the standard editor.
    // For now, let's assume we can create a simple curve editor or use the existing one.
    
    // If SCurveEditor is not accessible, you might have to rely on the standard "Curve" tab of the UCurveFloat editor.
    // A common workaround is to open the UCurveFloat asset in a separate tab and link them.
    
    return SNew(SDockTab).Label(FText::FromString("Curve"))
        [
            SNew(SBox).HAlign(HAlign_Fill).VAlign(VAlign_Fill)
            [
                // Placeholder or actual curve editor
                SNew(STextBlock).Text(FText::FromString("Curve Editor Placeholder"))
            ]
        ];
}

TSharedRef<SDockTab> UCurveMechanicEditor::SpawnDetailsTab(const FSpawnTabArgs& Args)
{
    return SNew(SDockTab).Label(FText::FromString("Details"))
        [
            DetailsView.ToSharedRef()
        ];
}