#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UVelocityCurveDebugger.generated.h"

class UVelocityCurveComponent;

namespace CurveDebuggerOptions
{
    const FLinearColor sBackgroundColor = FLinearColor(FColor(0, 0, 0, 0));
    const FLinearColor sRuleColor = FLinearColor::Gray;
    const FLinearColor sGraphColor = FLinearColor::Green;
    const FLinearColor sLegendBackgroundColor = FLinearColor::Black;
    const FLinearColor sLegendTextColor = FLinearColor(FColor(220, 220, 220));
    const FVector2D sLegendSize(375, 350);

    const FVector2D sGraphScaleFactor(100, -0.4);
}

UCLASS()
class UVelocityCurveDebugger : public UUserWidget
{
    GENERATED_BODY()

protected:
    // This is the C++ equivalent of the Blueprint OnPaint function.
    // It is called every frame to draw the widget.
    virtual int32 NativePaint(
        const FPaintArgs& Args,
        const FGeometry& AllottedGeometry,
        const FSlateRect& MyCullingRect,
        FSlateWindowElementList& OutDrawElements,
        int32 layerID,
        const FWidgetStyle& InWidgetStyle,
        bool bParentEnabled) const override;
private:
    // Returns the UVelocityCurveComponent from the currently possessed actor, or nullptr if none.
    const UVelocityCurveComponent* GetCurveComponent() const;

    void DrawLegend(FSlateWindowElementList& outDrawElements, int32 layerID, const FGeometry& AllottedGeometry) const;

    void DrawCurveGraph(FSlateWindowElementList& outDrawElements, int32 layerID, const FGeometry& AllottedGeometry) const;

    // Converts a buffer of Kurveball::Float3 to Unreal FVector2f for display on screen
    template <typename InputIteratorT>
    TArray<FVector2f> ToUnrealPoints(InputIteratorT beginIter, InputIteratorT endIter, float xOffset, float yOffset, float xScale, float yScale) const
    {
        TArray<FVector2f> unrealPoints;
        for (auto iter = beginIter; iter != endIter; ++iter)
        {
            unrealPoints.Push(FVector2f(xOffset + iter->X * xScale, yOffset + iter->Y * yScale));
        }
        return unrealPoints;
    }
};
