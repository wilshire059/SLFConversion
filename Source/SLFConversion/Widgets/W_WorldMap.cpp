// W_WorldMap.cpp
// Fullscreen world map overlay with fast travel markers.
// Scroll wheel to zoom, right-click drag to pan, left-click markers to travel.

#include "Widgets/W_WorldMap.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"
#include "Engine/Texture2D.h"
#include "Components/SaveLoadManagerComponent.h"
#include "Kismet/GameplayStatics.h"

UW_WorldMap::UW_WorldMap(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MapImage(nullptr)
	, MarkerCanvas(nullptr)
	, LocationNameText(nullptr)
	, ConfirmPromptText(nullptr)
	, EmptyHintText(nullptr)
	, PlayerMarker(nullptr)
	, SelectedMarkerIndex(-1)
	, bConfirmMode(false)
	, ZoomLevel(1.0f)
	, ViewCenterUV(0.5f, 0.5f)
	, bIsPanning(false)
	, LastMousePos(FVector2D::ZeroVector)
{
	SetIsFocusable(true);
	SetVisibility(ESlateVisibility::Visible); // Must be Visible to receive mouse events
}

TSharedRef<SWidget> UW_WorldMap::RebuildWidget()
{
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
		WidgetTree->RootWidget = RootCanvas;
		RootCanvas->SetClipping(EWidgetClipping::ClipToBounds);

		// Map image — will be repositioned by zoom/pan via anchors
		MapImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("MapImage"));
		RootCanvas->AddChild(MapImage);
		MapImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible); // Don't eat mouse events
		if (UCanvasPanelSlot* PanelSlot = Cast<UCanvasPanelSlot>(MapImage->Slot))
		{
			PanelSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
			PanelSlot->SetOffsets(FMargin(0.0f));
		}

		// Marker canvas — same fill, repositioned with map
		MarkerCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("MarkerCanvas"));
		RootCanvas->AddChild(MarkerCanvas);
		MarkerCanvas->SetVisibility(ESlateVisibility::SelfHitTestInvisible); // Don't eat mouse events
		if (UCanvasPanelSlot* PanelSlot = Cast<UCanvasPanelSlot>(MarkerCanvas->Slot))
		{
			PanelSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
			PanelSlot->SetOffsets(FMargin(0.0f));
		}

		// Location name text (bottom center)
		LocationNameText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("LocationNameText"));
		RootCanvas->AddChild(LocationNameText);
		if (UCanvasPanelSlot* PanelSlot = Cast<UCanvasPanelSlot>(LocationNameText->Slot))
		{
			PanelSlot->SetAnchors(FAnchors(0.5f, 1.0f, 0.5f, 1.0f));
			PanelSlot->SetAlignment(FVector2D(0.5f, 1.0f));
			PanelSlot->SetOffsets(FMargin(0.0f, 0.0f, 0.0f, 80.0f));
			PanelSlot->SetAutoSize(true);
		}
		LocationNameText->SetVisibility(ESlateVisibility::Collapsed);
		FSlateFontInfo Font = LocationNameText->GetFont();
		Font.Size = 24;
		LocationNameText->SetFont(Font);
		LocationNameText->SetColorAndOpacity(FSlateColor(FLinearColor::White));

		// Confirm prompt text (below location name)
		ConfirmPromptText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ConfirmPromptText"));
		RootCanvas->AddChild(ConfirmPromptText);
		if (UCanvasPanelSlot* PanelSlot = Cast<UCanvasPanelSlot>(ConfirmPromptText->Slot))
		{
			PanelSlot->SetAnchors(FAnchors(0.5f, 1.0f, 0.5f, 1.0f));
			PanelSlot->SetAlignment(FVector2D(0.5f, 1.0f));
			PanelSlot->SetOffsets(FMargin(0.0f, 0.0f, 0.0f, 40.0f));
			PanelSlot->SetAutoSize(true);
		}
		ConfirmPromptText->SetVisibility(ESlateVisibility::Collapsed);
		FSlateFontInfo ConfirmFont = ConfirmPromptText->GetFont();
		ConfirmFont.Size = 20;
		ConfirmPromptText->SetFont(ConfirmFont);
		ConfirmPromptText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.85f, 0.0f, 1.0f)));

		// Empty hint text (center)
		EmptyHintText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("EmptyHintText"));
		RootCanvas->AddChild(EmptyHintText);
		if (UCanvasPanelSlot* PanelSlot = Cast<UCanvasPanelSlot>(EmptyHintText->Slot))
		{
			PanelSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
			PanelSlot->SetAlignment(FVector2D(0.5f, 0.5f));
			PanelSlot->SetAutoSize(true);
		}
		EmptyHintText->SetText(FText::FromString(TEXT("Discover resting points to unlock fast travel")));
		EmptyHintText->SetVisibility(ESlateVisibility::Collapsed);
		FSlateFontInfo HintFont = EmptyHintText->GetFont();
		HintFont.Size = 16;
		EmptyHintText->SetFont(HintFont);
		EmptyHintText->SetColorAndOpacity(FSlateColor(FLinearColor(0.6f, 0.6f, 0.6f, 1.0f)));

		UE_LOG(LogTemp, Log, TEXT("[W_WorldMap] Widget tree built"));
	}
	return Super::RebuildWidget();
}

void UW_WorldMap::NativeConstruct()
{
	Super::NativeConstruct();

	if (MapImage)
	{
		UTexture2D* MapTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/Textures/T_WorldMap.T_WorldMap"));
		if (MapTexture)
		{
			MapTexture->SetForceMipLevelsToBeResident(30.0f);
			MapImage->SetBrushFromTexture(MapTexture);
			UE_LOG(LogTemp, Log, TEXT("[W_WorldMap] Loaded T_WorldMap (%dx%d)"),
				MapTexture->GetSizeX(), MapTexture->GetSizeY());
		}
		else
		{
			MapImage->SetColorAndOpacity(FLinearColor(0.05f, 0.05f, 0.1f, 1.0f));
		}
	}

	SetIsFocusable(true);
}

// ═══════════════════════════════════════════════════════════════════
// INPUT
// ═══════════════════════════════════════════════════════════════════

FReply UW_WorldMap::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FKey Key = InKeyEvent.GetKey();

	if (Key == EKeys::Escape || Key == EKeys::Gamepad_FaceButton_Right)
	{
		if (bConfirmMode)
		{
			bConfirmMode = false;
			if (ConfirmPromptText) ConfirmPromptText->SetVisibility(ESlateVisibility::Collapsed);
			return FReply::Handled();
		}
		OnMapClosed.Broadcast();
		return FReply::Handled();
	}

	if (Key == EKeys::Enter || Key == EKeys::Gamepad_FaceButton_Bottom)
	{
		if (SelectedMarkerIndex >= 0 && SelectedMarkerIndex < CachedRestPoints.Num())
		{
			if (bConfirmMode)
			{
				UE_LOG(LogTemp, Warning, TEXT("[W_WorldMap] Fast travel via Enter to '%s'"),
					*CachedRestPoints[SelectedMarkerIndex].LocationName.ToString());
				OnFastTravelRequested.Broadcast(CachedRestPoints[SelectedMarkerIndex]);
				bConfirmMode = false;
				if (ConfirmPromptText) ConfirmPromptText->SetVisibility(ESlateVisibility::Collapsed);
			}
			else
			{
				bConfirmMode = true;
				if (ConfirmPromptText)
				{
					ConfirmPromptText->SetText(FText::FromString(FString::Printf(
						TEXT("Travel to %s? [Enter / Click again]"),
						*CachedRestPoints[SelectedMarkerIndex].LocationName.ToString())));
					ConfirmPromptText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				}
			}
		}
		return FReply::Handled();
	}

	// WASD / DPad nav
	FVector2D NavDir = FVector2D::ZeroVector;
	if (Key == EKeys::W || Key == EKeys::Gamepad_DPad_Up)    NavDir = FVector2D(0.0f, -1.0f);
	if (Key == EKeys::S || Key == EKeys::Gamepad_DPad_Down)  NavDir = FVector2D(0.0f, 1.0f);
	if (Key == EKeys::A || Key == EKeys::Gamepad_DPad_Left)  NavDir = FVector2D(-1.0f, 0.0f);
	if (Key == EKeys::D || Key == EKeys::Gamepad_DPad_Right) NavDir = FVector2D(1.0f, 0.0f);

	if (!NavDir.IsNearlyZero() && CachedRestPoints.Num() > 0)
	{
		bConfirmMode = false;
		if (ConfirmPromptText) ConfirmPromptText->SetVisibility(ESlateVisibility::Collapsed);

		int32 NewIndex = FindNearestMarkerInDirection(NavDir);
		if (NewIndex >= 0 && NewIndex != SelectedMarkerIndex)
		{
			SelectedMarkerIndex = NewIndex;
			UpdateMarkerSelection();
		}
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

FReply UW_WorldMap::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FKey Button = InMouseEvent.GetEffectingButton();

	UE_LOG(LogTemp, Warning, TEXT("[W_WorldMap] NativeOnMouseButtonDown button=%s"), *Button.ToString());

	// Right mouse — start panning
	if (Button == EKeys::RightMouseButton)
	{
		bIsPanning = true;
		LastMousePos = InMouseEvent.GetScreenSpacePosition();
		return FReply::Handled().CaptureMouse(GetCachedWidget().ToSharedRef());
	}

	// Left mouse — click markers
	if (Button == EKeys::LeftMouseButton)
	{
		FVector2D LocalPos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
		int32 ClickedIdx = FindMarkerNearScreenPosition(LocalPos, InGeometry);

		UE_LOG(LogTemp, Warning, TEXT("[W_WorldMap] Click local=(%.0f,%.0f) -> marker=%d (selected=%d)"),
			LocalPos.X, LocalPos.Y, ClickedIdx, SelectedMarkerIndex);

		if (ClickedIdx >= 0)
		{
			if (ClickedIdx == SelectedMarkerIndex)
			{
				// Second click on selected marker — fast travel!
				UE_LOG(LogTemp, Warning, TEXT("[W_WorldMap] Fast travel via click to '%s'"),
					*CachedRestPoints[ClickedIdx].LocationName.ToString());
				OnFastTravelRequested.Broadcast(CachedRestPoints[ClickedIdx]);
				bConfirmMode = false;
				if (ConfirmPromptText) ConfirmPromptText->SetVisibility(ESlateVisibility::Collapsed);
			}
			else
			{
				// First click — select and show name
				SelectedMarkerIndex = ClickedIdx;
				bConfirmMode = false;
				if (ConfirmPromptText) ConfirmPromptText->SetVisibility(ESlateVisibility::Collapsed);
				UpdateMarkerSelection();

				// Show hint to click again
				if (ConfirmPromptText)
				{
					ConfirmPromptText->SetText(FText::FromString(FString::Printf(
						TEXT("Click again to travel to %s"),
						*CachedRestPoints[ClickedIdx].LocationName.ToString())));
					ConfirmPromptText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				}
			}
			return FReply::Handled();
		}

		// Click empty space — clear confirm
		bConfirmMode = false;
		if (ConfirmPromptText) ConfirmPromptText->SetVisibility(ESlateVisibility::Collapsed);
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply UW_WorldMap::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton && bIsPanning)
	{
		bIsPanning = false;
		return FReply::Handled().ReleaseMouseCapture();
	}
	return FReply::Unhandled();
}

FReply UW_WorldMap::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bIsPanning)
	{
		FVector2D CurrentPos = InMouseEvent.GetScreenSpacePosition();
		FVector2D Delta = CurrentPos - LastMousePos;
		LastMousePos = CurrentPos;

		FVector2D WidgetSize = InGeometry.GetLocalSize();
		if (WidgetSize.X > 0 && WidgetSize.Y > 0)
		{
			float UVDeltaX = -Delta.X / (WidgetSize.X * ZoomLevel);
			float UVDeltaY = -Delta.Y / (WidgetSize.Y * ZoomLevel);

			ViewCenterUV.X = FMath::Clamp(ViewCenterUV.X + UVDeltaX, 0.0f, 1.0f);
			ViewCenterUV.Y = FMath::Clamp(ViewCenterUV.Y + UVDeltaY, 0.0f, 1.0f);

			RepositionAllMarkers();
			UpdateMapImageTransform();
		}
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply UW_WorldMap::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	float WheelDelta = InMouseEvent.GetWheelDelta();

	FVector2D LocalPos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	FVector2D WidgetSize = InGeometry.GetLocalSize();
	if (WidgetSize.X > 0 && WidgetSize.Y > 0)
	{
		// UV under cursor before zoom
		FVector2D CursorViewportUV(LocalPos.X / WidgetSize.X, LocalPos.Y / WidgetSize.Y);
		FVector2D CursorMapUV = ViewportToFullMapUV(CursorViewportUV);

		float ZoomFactor = (WheelDelta > 0) ? 1.3f : (1.0f / 1.3f);
		ZoomLevel = FMath::Clamp(ZoomLevel * ZoomFactor, 1.0f, 50.0f);

		// Keep cursor position stable: ViewCenter = MapUV - (ViewportUV - 0.5) / Zoom
		ViewCenterUV.X = FMath::Clamp(CursorMapUV.X - (CursorViewportUV.X - 0.5f) / ZoomLevel, 0.0f, 1.0f);
		ViewCenterUV.Y = FMath::Clamp(CursorMapUV.Y - (CursorViewportUV.Y - 0.5f) / ZoomLevel, 0.0f, 1.0f);

		RepositionAllMarkers();
		UpdateMapImageTransform();

		UE_LOG(LogTemp, Log, TEXT("[W_WorldMap] Zoom=%.1f center=(%.3f,%.3f)"), ZoomLevel, ViewCenterUV.X, ViewCenterUV.Y);
	}

	return FReply::Handled();
}

// ═══════════════════════════════════════════════════════════════════
// MARKER MANAGEMENT
// ═══════════════════════════════════════════════════════════════════

void UW_WorldMap::EventNavigateCancel()
{
	if (bConfirmMode)
	{
		bConfirmMode = false;
		if (ConfirmPromptText) ConfirmPromptText->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	OnMapClosed.Broadcast();
}

void UW_WorldMap::RefreshMarkers()
{
	for (UImage* Marker : MarkerWidgets)
	{
		if (Marker && MarkerCanvas) MarkerCanvas->RemoveChild(Marker);
	}
	MarkerWidgets.Empty();
	if (PlayerMarker && MarkerCanvas)
	{
		MarkerCanvas->RemoveChild(PlayerMarker);
		PlayerMarker = nullptr;
	}
	CachedRestPoints.Empty();
	SelectedMarkerIndex = -1;
	bConfirmMode = false;
	if (ConfirmPromptText) ConfirmPromptText->SetVisibility(ESlateVisibility::Collapsed);

	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	USaveLoadManagerComponent* SaveMgr = PC->FindComponentByClass<USaveLoadManagerComponent>();
	if (!SaveMgr) return;

	CachedRestPoints = SaveMgr->GetDiscoveredRestPoints();

	if (CachedRestPoints.Num() == 0)
	{
		if (EmptyHintText) EmptyHintText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		if (LocationNameText) LocationNameText->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	if (EmptyHintText) EmptyHintText->SetVisibility(ESlateVisibility::Collapsed);

	// Create markers
	for (int32 i = 0; i < CachedRestPoints.Num(); ++i)
	{
		UImage* Marker = CreateMarkerWidget(false);
		if (Marker) MarkerWidgets.Add(Marker);
	}
	PlayerMarker = CreateMarkerWidget(true);

	// Auto-zoom to fit
	AutoFitZoomToMarkers();

	// Select nearest to player
	FVector PlayerLoc = FVector::ZeroVector;
	if (APawn* Pawn = PC->GetPawn()) PlayerLoc = Pawn->GetActorLocation();

	float BestDist = TNumericLimits<float>::Max();
	int32 BestIdx = 0;
	for (int32 i = 0; i < CachedRestPoints.Num(); ++i)
	{
		float Dist = FVector::DistSquared(PlayerLoc, CachedRestPoints[i].WorldLocation);
		if (Dist < BestDist) { BestDist = Dist; BestIdx = i; }
	}
	SelectedMarkerIndex = BestIdx;

	RepositionAllMarkers();
	UpdateMapImageTransform();
	UpdateMarkerSelection();

	UE_LOG(LogTemp, Warning, TEXT("[W_WorldMap] Refreshed %d markers, zoom=%.1f, center=(%.3f,%.3f)"),
		CachedRestPoints.Num(), ZoomLevel, ViewCenterUV.X, ViewCenterUV.Y);
}

void UW_WorldMap::AutoFitZoomToMarkers()
{
	if (CachedRestPoints.Num() == 0) return;

	float MinU = 1.0f, MaxU = 0.0f, MinV = 1.0f, MaxV = 0.0f;
	for (const FSLFRestPointSaveInfo& RP : CachedRestPoints)
	{
		FVector2D UV = WorldToFullMapUV(RP.WorldLocation);
		MinU = FMath::Min(MinU, UV.X); MaxU = FMath::Max(MaxU, UV.X);
		MinV = FMath::Min(MinV, UV.Y); MaxV = FMath::Max(MaxV, UV.Y);
	}

	// Include player position
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (APawn* Pawn = PC->GetPawn())
		{
			FVector2D PlayerUV = WorldToFullMapUV(Pawn->GetActorLocation());
			MinU = FMath::Min(MinU, PlayerUV.X); MaxU = FMath::Max(MaxU, PlayerUV.X);
			MinV = FMath::Min(MinV, PlayerUV.Y); MaxV = FMath::Max(MaxV, PlayerUV.Y);
		}
	}

	float RangeU = MaxU - MinU;
	float RangeV = MaxV - MinV;
	float MarkerPadding = FMath::Max(RangeU, RangeV) * 0.4f;
	MarkerPadding = FMath::Max(MarkerPadding, 0.02f);

	float ViewSpan = FMath::Max(RangeU, RangeV) + MarkerPadding * 2.0f;
	ViewSpan = FMath::Max(ViewSpan, 0.03f);

	ZoomLevel = FMath::Clamp(1.0f / ViewSpan, 1.0f, 50.0f);
	ViewCenterUV.X = (MinU + MaxU) * 0.5f;
	ViewCenterUV.Y = (MinV + MaxV) * 0.5f;

	UE_LOG(LogTemp, Warning, TEXT("[W_WorldMap] AutoFit: UV(%.4f,%.4f)-(%.4f,%.4f) zoom=%.1f center=(%.3f,%.3f)"),
		MinU, MinV, MaxU, MaxV, ZoomLevel, ViewCenterUV.X, ViewCenterUV.Y);
}

void UW_WorldMap::UpdateMapImageTransform()
{
	if (!MapImage) return;

	// Scale the map image to simulate zoom. At zoom Z centered at (cx,cy):
	// Anchor min = (0.5 - cx*Z, 0.5 - cy*Z)
	// Anchor max = min + (Z, Z)
	// This makes the image Z times larger and offsets it so (cx,cy) is at screen center.
	float AnchorMinX = 0.5f - ViewCenterUV.X * ZoomLevel;
	float AnchorMinY = 0.5f - ViewCenterUV.Y * ZoomLevel;
	float AnchorMaxX = AnchorMinX + ZoomLevel;
	float AnchorMaxY = AnchorMinY + ZoomLevel;

	if (UCanvasPanelSlot* PanelSlot = Cast<UCanvasPanelSlot>(MapImage->Slot))
	{
		PanelSlot->SetAnchors(FAnchors(AnchorMinX, AnchorMinY, AnchorMaxX, AnchorMaxY));
		PanelSlot->SetOffsets(FMargin(0.0f));
	}
}

void UW_WorldMap::RepositionAllMarkers()
{
	for (int32 i = 0; i < MarkerWidgets.Num() && i < CachedRestPoints.Num(); ++i)
	{
		if (!MarkerWidgets[i]) continue;
		FVector2D MapUV = WorldToFullMapUV(CachedRestPoints[i].WorldLocation);
		FVector2D ViewUV = FullMapUVToViewport(MapUV);

		if (UCanvasPanelSlot* PanelSlot = Cast<UCanvasPanelSlot>(MarkerWidgets[i]->Slot))
		{
			PanelSlot->SetAnchors(FAnchors(ViewUV.X, ViewUV.Y, ViewUV.X, ViewUV.Y));
		}

		bool bOnScreen = (ViewUV.X > -0.05f && ViewUV.X < 1.05f && ViewUV.Y > -0.05f && ViewUV.Y < 1.05f);
		MarkerWidgets[i]->SetVisibility(bOnScreen ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	}

	if (PlayerMarker)
	{
		if (APlayerController* PC = GetOwningPlayer())
		{
			if (APawn* Pawn = PC->GetPawn())
			{
				FVector2D MapUV = WorldToFullMapUV(Pawn->GetActorLocation());
				FVector2D ViewUV = FullMapUVToViewport(MapUV);

				if (UCanvasPanelSlot* PanelSlot = Cast<UCanvasPanelSlot>(PlayerMarker->Slot))
				{
					PanelSlot->SetAnchors(FAnchors(ViewUV.X, ViewUV.Y, ViewUV.X, ViewUV.Y));
				}

				bool bOnScreen = (ViewUV.X > -0.05f && ViewUV.X < 1.05f && ViewUV.Y > -0.05f && ViewUV.Y < 1.05f);
				PlayerMarker->SetVisibility(bOnScreen ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
			}
		}
	}
}

// ═══════════════════════════════════════════════════════════════════
// COORDINATE TRANSFORMS
// ═══════════════════════════════════════════════════════════════════

FVector2D UW_WorldMap::WorldToFullMapUV(const FVector& WorldLocation) const
{
	float HalfWidth = MapOrthoWidth * 0.5f;
	float U = (WorldLocation.X - MapCenterX + HalfWidth) / MapOrthoWidth;
	float V = (WorldLocation.Y - MapCenterY + HalfWidth) / MapOrthoWidth;
	return FVector2D(U, V);
}

FVector2D UW_WorldMap::FullMapUVToViewport(const FVector2D& MapUV) const
{
	// ViewportUV = (MapUV - ViewCenter) * Zoom + 0.5
	return FVector2D(
		(MapUV.X - ViewCenterUV.X) * ZoomLevel + 0.5f,
		(MapUV.Y - ViewCenterUV.Y) * ZoomLevel + 0.5f
	);
}

FVector2D UW_WorldMap::ViewportToFullMapUV(const FVector2D& ViewportUV) const
{
	// MapUV = (ViewportUV - 0.5) / Zoom + ViewCenter
	return FVector2D(
		(ViewportUV.X - 0.5f) / ZoomLevel + ViewCenterUV.X,
		(ViewportUV.Y - 0.5f) / ZoomLevel + ViewCenterUV.Y
	);
}

// ═══════════════════════════════════════════════════════════════════
// MARKER CREATION & SELECTION
// ═══════════════════════════════════════════════════════════════════

UImage* UW_WorldMap::CreateMarkerWidget(bool bIsPlayer)
{
	if (!WidgetTree || !MarkerCanvas) return nullptr;

	UImage* Marker = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	if (!Marker) return nullptr;

	MarkerCanvas->AddChild(Marker);
	Marker->SetVisibility(ESlateVisibility::SelfHitTestInvisible); // Don't eat mouse events

	float MarkerSize = bIsPlayer ? 32.0f : 44.0f;

	if (UCanvasPanelSlot* PanelSlot = Cast<UCanvasPanelSlot>(Marker->Slot))
	{
		PanelSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
		PanelSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		PanelSlot->SetOffsets(FMargin(0.0f, 0.0f, MarkerSize, MarkerSize));
		PanelSlot->SetAutoSize(false);
	}

	// T_Target crosshair icon tinted with color
	UTexture2D* MarkerTex = LoadObject<UTexture2D>(nullptr,
		TEXT("/Game/SoulslikeFramework/Textures/_Misc/T_Target.T_Target"));
	if (MarkerTex)
	{
		Marker->SetBrushFromTexture(MarkerTex);
	}

	if (bIsPlayer)
	{
		Marker->SetColorAndOpacity(FLinearColor(0.0f, 0.8f, 1.0f, 1.0f)); // Cyan
	}
	else
	{
		Marker->SetColorAndOpacity(FLinearColor(1.0f, 0.75f, 0.0f, 1.0f)); // Gold
	}

	return Marker;
}

void UW_WorldMap::UpdateMarkerSelection()
{
	for (int32 i = 0; i < MarkerWidgets.Num(); ++i)
	{
		if (!MarkerWidgets[i]) continue;

		if (i == SelectedMarkerIndex)
		{
			MarkerWidgets[i]->SetColorAndOpacity(FLinearColor(1.0f, 0.15f, 0.15f, 1.0f)); // Bright red
			if (UCanvasPanelSlot* PanelSlot = Cast<UCanvasPanelSlot>(MarkerWidgets[i]->Slot))
			{
				PanelSlot->SetOffsets(FMargin(0.0f, 0.0f, 56.0f, 56.0f));
			}
		}
		else
		{
			MarkerWidgets[i]->SetColorAndOpacity(FLinearColor(1.0f, 0.75f, 0.0f, 1.0f)); // Gold
			if (UCanvasPanelSlot* PanelSlot = Cast<UCanvasPanelSlot>(MarkerWidgets[i]->Slot))
			{
				PanelSlot->SetOffsets(FMargin(0.0f, 0.0f, 44.0f, 44.0f));
			}
		}
	}

	if (LocationNameText && SelectedMarkerIndex >= 0 && SelectedMarkerIndex < CachedRestPoints.Num())
	{
		LocationNameText->SetText(CachedRestPoints[SelectedMarkerIndex].LocationName);
		LocationNameText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else if (LocationNameText)
	{
		LocationNameText->SetVisibility(ESlateVisibility::Collapsed);
	}
}

int32 UW_WorldMap::FindNearestMarkerInDirection(const FVector2D& Direction) const
{
	if (CachedRestPoints.Num() == 0) return -1;
	if (SelectedMarkerIndex < 0) return 0;

	FVector2D CurrentVP = FullMapUVToViewport(WorldToFullMapUV(CachedRestPoints[SelectedMarkerIndex].WorldLocation));
	float BestScore = TNumericLimits<float>::Max();
	int32 BestIdx = -1;

	for (int32 i = 0; i < CachedRestPoints.Num(); ++i)
	{
		if (i == SelectedMarkerIndex) continue;
		FVector2D CandVP = FullMapUVToViewport(WorldToFullMapUV(CachedRestPoints[i].WorldLocation));
		FVector2D Delta = CandVP - CurrentVP;
		float Dot = FVector2D::DotProduct(Delta.GetSafeNormal(), Direction);
		if (Dot < 0.3f) continue;
		float Score = Delta.Size() / FMath::Max(Dot, 0.01f);
		if (Score < BestScore) { BestScore = Score; BestIdx = i; }
	}

	return BestIdx;
}

int32 UW_WorldMap::FindMarkerNearScreenPosition(const FVector2D& LocalPosition, const FGeometry& Geometry) const
{
	if (CachedRestPoints.Num() == 0) return -1;

	FVector2D WidgetSize = Geometry.GetLocalSize();
	if (WidgetSize.X < 1.0f || WidgetSize.Y < 1.0f) return -1;

	FVector2D ClickVP(LocalPosition.X / WidgetSize.X, LocalPosition.Y / WidgetSize.Y);

	// Generous click radius: 80 pixels at 1080p
	float ClickRadius = 80.0f / FMath::Min(WidgetSize.X, WidgetSize.Y);

	float BestDist = ClickRadius;
	int32 BestIdx = -1;

	for (int32 i = 0; i < CachedRestPoints.Num(); ++i)
	{
		FVector2D MarkerVP = FullMapUVToViewport(WorldToFullMapUV(CachedRestPoints[i].WorldLocation));
		float Dist = FVector2D::Distance(ClickVP, MarkerVP);
		if (Dist < BestDist) { BestDist = Dist; BestIdx = i; }
	}

	return BestIdx;
}
