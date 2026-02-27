// W_WorldMap.h
// Fullscreen world map overlay with fast travel markers.
// Supports zoom (scroll wheel), pan (right-click drag), and click-to-travel.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SLFGameTypes.h"
#include "W_WorldMap.generated.h"

class UImage;
class UCanvasPanel;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMapClosed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFastTravelRequested, const FSLFRestPointSaveInfo&, Destination);

UCLASS()
class SLFCONVERSION_API UW_WorldMap : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_WorldMap(const FObjectInitializer& ObjectInitializer);

	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UFUNCTION(BlueprintCallable, Category = "W_WorldMap")
	void EventNavigateCancel();

	UFUNCTION(BlueprintCallable, Category = "W_WorldMap")
	void RefreshMarkers();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnMapClosed OnMapClosed;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnFastTravelRequested OnFastTravelRequested;

protected:
	// Map configuration (matches OrthoWidth=800000, Center=(0,0) from map capture)
	static constexpr float MapCenterX = 0.0f;
	static constexpr float MapCenterY = 0.0f;
	static constexpr float MapOrthoWidth = 800000.0f;

	// Widget tree
	UPROPERTY() UImage* MapImage;
	UPROPERTY() UCanvasPanel* MarkerCanvas;
	UPROPERTY() UTextBlock* LocationNameText;
	UPROPERTY() UTextBlock* ConfirmPromptText;
	UPROPERTY() UTextBlock* EmptyHintText;

	// Marker state
	TArray<FSLFRestPointSaveInfo> CachedRestPoints;
	TArray<UImage*> MarkerWidgets;
	UPROPERTY() UImage* PlayerMarker;
	int32 SelectedMarkerIndex = -1;
	bool bConfirmMode = false;

	// Zoom and pan state
	float ZoomLevel = 1.0f;           // 1.0 = full map, higher = zoomed in
	FVector2D ViewCenterUV = FVector2D(0.5f, 0.5f); // UV center of current view
	bool bIsPanning = false;
	FVector2D LastMousePos = FVector2D::ZeroVector;

	// Helpers
	FVector2D WorldToFullMapUV(const FVector& WorldLocation) const;
	FVector2D FullMapUVToViewport(const FVector2D& MapUV) const;
	FVector2D ViewportToFullMapUV(const FVector2D& ViewportUV) const;
	void RepositionAllMarkers();
	void UpdateMapImageTransform();
	UImage* CreateMarkerWidget(bool bIsPlayer);
	void UpdateMarkerSelection();
	int32 FindNearestMarkerInDirection(const FVector2D& Direction) const;
	int32 FindMarkerNearScreenPosition(const FVector2D& LocalPosition, const FGeometry& Geometry) const;
	void AutoFitZoomToMarkers();
};
