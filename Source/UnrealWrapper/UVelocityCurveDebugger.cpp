#include "UnrealWrapper/UVelocityCurveDebugger.h"
#include "UnrealWrapper/UVelocityCurveComponent.h"
#include <Fonts/FontMeasure.h>
#include <Kismet/GameplayStatics.h>
#include <Layout/Geometry.h>
#include <Rendering/DrawElements.h>

#include "ContainerUtils.h"
#include "VelocityCurveApi.h"

int32 UVelocityCurveDebugger::NativePaint(
    const FPaintArgs& Args,
    const FGeometry& AllottedGeometry,
    const FSlateRect& MyCullingRect,
    FSlateWindowElementList& outDrawElements,
    int32 layerID,
    const FWidgetStyle& InWidgetStyle,
    bool bParentEnabled
) const
{
    // Always call the parent implementation first
    layerID = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, outDrawElements, layerID, InWidgetStyle, bParentEnabled);

    const auto* curveComponent = GetCurveComponent();
    if (!curveComponent)
    {
        return layerID + 1;
    }

    // Get the local size of the widget's allocated space (in screen pixels)
    const FVector2D localSize = AllottedGeometry.GetLocalSize();

    // Define the color and thickness of the graph lines
    constexpr float ruleThickness = 2.0f;

    static FSlateBrush backgroundBrush;
    backgroundBrush.DrawAs = ESlateBrushDrawType::Box;
    backgroundBrush.TintColor = CurveDebuggerOptions::sBackgroundColor;
    FSlateDrawElement::MakeBox(outDrawElements, layerID, AllottedGeometry.ToPaintGeometry(), &backgroundBrush, ESlateDrawEffect::None, CurveDebuggerOptions::sBackgroundColor);

    DrawLegend(outDrawElements, layerID, AllottedGeometry);

    // TODO: Draw rule lines

    DrawCurveGraph(outDrawElements, layerID, AllottedGeometry);

    // Increment layerID for subsequent drawing operations
    return layerID + 1;
}

const UVelocityCurveComponent* UVelocityCurveDebugger::GetCurveComponent() const
{
    const APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!controller)
    {
        return nullptr;
    }

    const APawn* possessedPawn = controller->GetPawn();
    if (!possessedPawn)
    {
        return nullptr;
    }

    const auto* curveComponent = possessedPawn->FindComponentByClass<UVelocityCurveComponent>();
    return curveComponent;
}

void UVelocityCurveDebugger::DrawLegend(FSlateWindowElementList& outDrawElements, int32 layerID, const FGeometry& AllottedGeometry) const
{
    static FSlateBrush legendBackgroundBrush;
    legendBackgroundBrush.DrawAs = ESlateBrushDrawType::Box;

    const auto* curveComponent = GetCurveComponent();
    if (!curveComponent)
    {
        return;
    }

    FGeometry legendGeo = AllottedGeometry.MakeChild(FVector2D::ZeroVector, CurveDebuggerOptions::sLegendSize);

    static FSlateFontInfo sLegendFont = FCoreStyle::Get().GetFontStyle(TEXT("NormalText"));
    sLegendFont.Size = 32;
    
    const TSharedRef<FSlateFontMeasure>& fontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
    const float fontHeight = fontMeasure->Measure(TEXT("T"), sLegendFont, 1.f).Y;

    uint8_t curveNum{ 0U };
    const auto& context = curveComponent->GetCurveContext();
    auto joinedRefs = Kurveball::GetJoinedContainerReferences(context.mLinearCurves, context.mRotationCurves);
    auto allCurveInstances = joinedRefs | std::views::join;

    for (const auto& [curveID, curveInstance] : allCurveInstances)
    {
        FVector2D textPosition(0.0f, curveNum * fontHeight);
        FSlateLayoutTransform textTransform(1.f, textPosition);

        std::string curveName = "Unknown";
        if (curveInstance.mMechanic.mDebugName.has_value())
        {
            curveName = *curveInstance.mMechanic.mDebugName;
        }
        
        std::stringstream curveDebugStr;
        curveDebugStr << curveName << ": " << curveInstance.mDistanceAccumulator.GetLatestSample().Y << "m/s, " << curveInstance.mDistanceAccumulator.GetTotalArea() << "cm";
        FSlateDrawElement::MakeText(outDrawElements, layerID, AllottedGeometry.ToPaintGeometry(textTransform), UTF8_TO_TCHAR(curveDebugStr.str().c_str()), sLegendFont, ESlateDrawEffect::None, CurveDebuggerOptions::sLegendTextColor);

        ++curveNum;
    }
}

void UVelocityCurveDebugger::DrawCurveGraph(FSlateWindowElementList& outDrawElements, int32 layerID, const FGeometry& AllottedGeometry) const
{
    const auto* curveComponent = GetCurveComponent();
    if (!curveComponent)
    {
        return;
    }

    const auto graphSize = AllottedGeometry.GetAbsoluteSize();

    const auto& context = curveComponent->GetCurveContext();
    auto joinedRefs = Kurveball::GetJoinedContainerReferences(context.mLinearCurves, context.mRotationCurves);
    auto allCurveInstances = joinedRefs | std::views::join;

    for (const auto& [curveID, curveInstance] : allCurveInstances)
    {
        const uint32_t graphColorRaw = Kurveball::CalculateCurveDebugColor(curveID);
        FLinearColor graphColor = FLinearColor::FromSRGBColor(FColor(graphColorRaw));
        const auto historyPointsUnreal = ToUnrealPoints(curveInstance.mHistory.Begin(),
                                                        curveInstance.mHistory.End(),
                                                        0,
                                                        AllottedGeometry.GetLocalSize().Y / 2,
                                                        CurveDebuggerOptions::sGraphScaleFactor.X,
                                                        CurveDebuggerOptions::sGraphScaleFactor.Y);

        FSlateDrawElement::MakeLines(
            outDrawElements,
            layerID,
            AllottedGeometry.ToPaintGeometry(),
            historyPointsUnreal,
            ESlateDrawEffect::None,
            graphColor,
            true, // bAntialias
            2.f
        );
    }
}