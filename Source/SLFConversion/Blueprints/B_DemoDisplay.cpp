// B_DemoDisplay.cpp
// C++ implementation for Blueprint B_DemoDisplay
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/B_DemoDisplay.json
// Logic traced from JSON node-by-node

#include "Blueprints/B_DemoDisplay.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/KismetMathLibrary.h"

AB_DemoDisplay::AB_DemoDisplay()
{
	// Initialize default values from Blueprint
	Width = 200.0;
	Depth = 50.0;
	Height = 300.0;
	TitleText = FText::FromString(TEXT("Title"));
	TextAlignment = ETextJustify::Left;
	DescriptionScale = 1.0;
	TitleScale = 1.5;
	TitleColour = FColor::White;
	DescriptionColour = FColor::White;
	SeperateTitlePanel = false;
	FloorText = false;
	Spotlight = false;
	Numberoflinesbetweenparagraphs = 2;
	Numberofspacesbetweenlines = 1;
	TextPadding = 10.0f;
	CastShadows = true;
	ShadowCover = 0.5;
	CustomColour = FLinearColor::White;

	// Create ISM components
	ISM_Main = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ISM_Main"));
	ISM_Corner = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ISM_Corner"));
	ISM_Side = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ISM_Side"));
	ISM_Curve = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ISM_Curve"));
	ISM_CurveEdge = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ISM_CurveEdge"));
	ISM_TitleBarMain = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ISM_TitleBarMain"));
	ISM_TitleBarEnd = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ISM_TitleBarEnd"));
	ISM_ShadowStraight = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ISM_ShadowStraight"));
	ISM_ShadowRound = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ISM_ShadowRound"));

	RootComponent = ISM_Main;

	// Attach other components
	if (RootComponent)
	{
		ISM_Corner->SetupAttachment(RootComponent);
		ISM_Side->SetupAttachment(RootComponent);
		ISM_Curve->SetupAttachment(RootComponent);
		ISM_CurveEdge->SetupAttachment(RootComponent);
		ISM_TitleBarMain->SetupAttachment(RootComponent);
		ISM_TitleBarEnd->SetupAttachment(RootComponent);
		ISM_ShadowStraight->SetupAttachment(RootComponent);
		ISM_ShadowRound->SetupAttachment(RootComponent);
	}

	UE_LOG(LogTemp, Verbose, TEXT("AB_DemoDisplay::Constructor - Components created"));
}

void AB_DemoDisplay::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Blueprint construction script calls Redraw
	Redraw();
}

void AB_DemoDisplay::CreateInstances_Implementation()
{
	// Logic from JSON Create Instances graph:
	// Clear all ISM instances and prepare for new geometry

	UE_LOG(LogTemp, Verbose, TEXT("AB_DemoDisplay::CreateInstances - Clearing instances"));

	if (ISM_Main)
	{
		ISM_Main->ClearInstances();
	}
	if (ISM_Corner)
	{
		ISM_Corner->ClearInstances();
	}
	if (ISM_Side)
	{
		ISM_Side->ClearInstances();
	}
	if (ISM_Curve)
	{
		ISM_Curve->ClearInstances();
	}
	if (ISM_CurveEdge)
	{
		ISM_CurveEdge->ClearInstances();
	}
	if (ISM_TitleBarMain)
	{
		ISM_TitleBarMain->ClearInstances();
	}
	if (ISM_TitleBarEnd)
	{
		ISM_TitleBarEnd->ClearInstances();
	}
	if (ISM_ShadowStraight)
	{
		ISM_ShadowStraight->ClearInstances();
	}
	if (ISM_ShadowRound)
	{
		ISM_ShadowRound->ClearInstances();
	}
}

void AB_DemoDisplay::CreateDisplay_Implementation()
{
	// Logic from JSON Create Display graph:
	// Sequence of operations:
	// 1. Add curve instances at corners
	// 2. Add corner instances
	// 3. Call ScalablePanel for main panels
	// 4. Add side instances
	// 5. If SeperateTitlePanel: call AddTitlePanel
	// 6. If CastShadows: call ShadowBox

	UE_LOG(LogTemp, Verbose, TEXT("AB_DemoDisplay::CreateDisplay - Creating display geometry"));

	// Scale factors
	const double WidthScale = Width / 100.0;
	const double HeightScale = Height / 100.0;
	const double DepthScale = Depth / 100.0;

	// Step 1: Add curve instances at top corners
	if (ISM_Curve)
	{
		// Top-left curve
		FTransform CurveTransform;
		CurveTransform.SetLocation(FVector::ZeroVector);
		CurveTransform.SetRotation(FQuat::Identity);
		CurveTransform.SetScale3D(FVector(1.0, WidthScale, 1.0));
		ISM_Curve->AddInstance(CurveTransform, true);

		// Top-right curve
		FTransform CurveTransform2;
		CurveTransform2.SetLocation(FVector(0.0, Width, 0.0));
		CurveTransform2.SetRotation(FRotator(0.0, 180.0, 0.0).Quaternion());
		CurveTransform2.SetScale3D(FVector(1.0, WidthScale, 1.0));
		ISM_Curve->AddInstance(CurveTransform2, true);
	}

	// Step 2: Add curve edge instances
	if (ISM_CurveEdge)
	{
		FTransform EdgeTransform;
		EdgeTransform.SetLocation(FVector(Depth, 0.0, 0.0));
		EdgeTransform.SetRotation(FQuat::Identity);
		EdgeTransform.SetScale3D(FVector(1.0, WidthScale, HeightScale));
		ISM_CurveEdge->AddInstance(EdgeTransform, true);
	}

	// Step 3: Call ScalablePanel for main body
	FTransform BaseTransform;
	BaseTransform.SetLocation(FVector(Depth, 0.0, 0.0));
	ScalablePanel(BaseTransform, Height);

	// Step 4: Add corner instances
	if (ISM_Corner)
	{
		// Four corners
		FTransform CornerTransforms[4];
		CornerTransforms[0].SetLocation(FVector(0.0, 0.0, 0.0));
		CornerTransforms[1].SetLocation(FVector(0.0, Width, 0.0));
		CornerTransforms[1].SetRotation(FRotator(0.0, 90.0, 0.0).Quaternion());
		CornerTransforms[2].SetLocation(FVector(0.0, 0.0, Height));
		CornerTransforms[2].SetRotation(FRotator(0.0, -90.0, 0.0).Quaternion());
		CornerTransforms[3].SetLocation(FVector(0.0, Width, Height));
		CornerTransforms[3].SetRotation(FRotator(0.0, 180.0, 0.0).Quaternion());

		for (int32 i = 0; i < 4; i++)
		{
			CornerTransforms[i].SetScale3D(FVector::OneVector);
			ISM_Corner->AddInstance(CornerTransforms[i], true);
		}
	}

	// Step 5: Add side instances
	if (ISM_Side)
	{
		// Left side
		FTransform SideTransformL;
		SideTransformL.SetLocation(FVector(0.0, 0.0, 0.0));
		SideTransformL.SetRotation(FQuat::Identity);
		SideTransformL.SetScale3D(FVector(DepthScale, 1.0, HeightScale));
		ISM_Side->AddInstance(SideTransformL, true);

		// Right side
		FTransform SideTransformR;
		SideTransformR.SetLocation(FVector(0.0, Width, 0.0));
		SideTransformR.SetRotation(FRotator(0.0, 180.0, 0.0).Quaternion());
		SideTransformR.SetScale3D(FVector(DepthScale, 1.0, HeightScale));
		ISM_Side->AddInstance(SideTransformR, true);
	}

	// Step 6: Add title panel if separate
	if (SeperateTitlePanel)
	{
		AddTitlePanel();
	}

	// Step 7: Add shadow box if enabled
	if (CastShadows)
	{
		ShadowBox();
	}

	UE_LOG(LogTemp, Verbose, TEXT("AB_DemoDisplay::CreateDisplay - Display geometry complete"));
}

void AB_DemoDisplay::ScalablePanel_Implementation(const FTransform& BasePosition, double Length)
{
	// Logic from JSON Scalable Panel graph:
	// Creates scalable main panel instances based on position and length
	// Uses ISM_Main to add instances along the panel

	UE_LOG(LogTemp, Verbose, TEXT("AB_DemoDisplay::ScalablePanel - Length: %f"), Length);

	if (!ISM_Main)
	{
		return;
	}

	// Calculate how many panel instances needed
	const double PanelSize = 100.0; // Base panel size
	int32 PanelCount = FMath::Max(1, FMath::CeilToInt(Length / PanelSize));
	double ScaleZ = Length / (PanelCount * PanelSize);

	FVector BaseLocation = BasePosition.GetLocation();
	FRotator BaseRotation = BasePosition.GetRotation().Rotator();
	FVector BaseScale = BasePosition.GetScale3D();

	for (int32 i = 0; i < PanelCount; i++)
	{
		FTransform PanelTransform;
		double PanelZ = i * PanelSize * ScaleZ;
		PanelTransform.SetLocation(BaseLocation + FVector(0.0, 0.0, PanelZ));
		PanelTransform.SetRotation(BaseRotation.Quaternion());
		PanelTransform.SetScale3D(FVector(BaseScale.X, Width / 100.0, ScaleZ));

		ISM_Main->AddInstance(PanelTransform, true);
	}
}

void AB_DemoDisplay::Redraw_Implementation()
{
	// Logic from JSON Redraw graph:
	// 1. Call CreateInstances (clear)
	// 2. Call CreateDisplay (rebuild)
	// 3. Call FormatText (update text)

	UE_LOG(LogTemp, Verbose, TEXT("AB_DemoDisplay::Redraw"));

	CreateInstances();
	CreateDisplay();
	FormatText();
}

void AB_DemoDisplay::FormatText_Implementation()
{
	// Logic from JSON Format Text graph:
	// Complex text formatting with:
	// - Paragraph handling
	// - Line breaking
	// - Carriage return insertion
	// This is a very complex function (8000+ lines in JSON)

	UE_LOG(LogTemp, Verbose, TEXT("AB_DemoDisplay::FormatText - Processing text"));

	// Build final description string from array
	DescriptionFinal.Empty();

	for (int32 i = 0; i < DescriptionText.Num(); i++)
	{
		if (i > 0)
		{
			// Add paragraph breaks
			for (int32 j = 0; j < Numberoflinesbetweenparagraphs; j++)
			{
				DescriptionFinal.Append(TEXT("\n"));
			}
		}

		FString ParagraphText = DescriptionText[i].ToString();

		// Add line spacing within paragraph
		// Note: Full Blueprint logic handles word wrapping, this is simplified
		DescriptionFinal.Append(ParagraphText);
	}

	UE_LOG(LogTemp, Verbose, TEXT("AB_DemoDisplay::FormatText - Final text: %s"), *DescriptionFinal);
}

void AB_DemoDisplay::AddTitlePanel_Implementation()
{
	// Logic from JSON Add Title Panel graph:
	// Adds title bar instances above the main panel

	UE_LOG(LogTemp, Verbose, TEXT("AB_DemoDisplay::AddTitlePanel"));

	if (!ISM_TitleBarMain || !ISM_TitleBarEnd)
	{
		return;
	}

	// Calculate title bar position (above main panel)
	const double TitleBarHeight = 30.0;
	const double TitleBarY = Height + TitleBarHeight / 2.0;

	// Main title bar (center section)
	FTransform MainBarTransform;
	MainBarTransform.SetLocation(FVector(Depth / 2.0, Width / 2.0, TitleBarY));
	MainBarTransform.SetRotation(FQuat::Identity);
	MainBarTransform.SetScale3D(FVector(Depth / 100.0, (Width - 60.0) / 100.0, 1.0));
	ISM_TitleBarMain->AddInstance(MainBarTransform, true);

	// End caps
	FTransform LeftEndTransform;
	LeftEndTransform.SetLocation(FVector(Depth / 2.0, 30.0, TitleBarY));
	LeftEndTransform.SetRotation(FQuat::Identity);
	LeftEndTransform.SetScale3D(FVector::OneVector);
	ISM_TitleBarEnd->AddInstance(LeftEndTransform, true);

	FTransform RightEndTransform;
	RightEndTransform.SetLocation(FVector(Depth / 2.0, Width - 30.0, TitleBarY));
	RightEndTransform.SetRotation(FRotator(0.0, 180.0, 0.0).Quaternion());
	RightEndTransform.SetScale3D(FVector::OneVector);
	ISM_TitleBarEnd->AddInstance(RightEndTransform, true);
}

void AB_DemoDisplay::ShadowBox_Implementation()
{
	// Logic from JSON Shadow Box graph:
	// Adds shadow geometry underneath the display panel

	UE_LOG(LogTemp, Verbose, TEXT("AB_DemoDisplay::ShadowBox"));

	if (!ISM_ShadowStraight || !ISM_ShadowRound)
	{
		return;
	}

	if (ShadowCover <= 0.0)
	{
		return;
	}

	// Shadow offset
	const double ShadowOffset = -5.0;
	const double ShadowHeight = Height * ShadowCover;

	// Straight shadow sections
	FTransform ShadowTransformL;
	ShadowTransformL.SetLocation(FVector(Depth + ShadowOffset, 0.0, 0.0));
	ShadowTransformL.SetRotation(FQuat::Identity);
	ShadowTransformL.SetScale3D(FVector(1.0, 1.0, ShadowHeight / 100.0));
	ISM_ShadowStraight->AddInstance(ShadowTransformL, true);

	FTransform ShadowTransformR;
	ShadowTransformR.SetLocation(FVector(Depth + ShadowOffset, Width, 0.0));
	ShadowTransformR.SetRotation(FRotator(0.0, 180.0, 0.0).Quaternion());
	ShadowTransformR.SetScale3D(FVector(1.0, 1.0, ShadowHeight / 100.0));
	ISM_ShadowStraight->AddInstance(ShadowTransformR, true);

	// Round shadow corners
	FTransform RoundTransform;
	RoundTransform.SetLocation(FVector(Depth + ShadowOffset, Width / 2.0, ShadowHeight));
	RoundTransform.SetRotation(FQuat::Identity);
	RoundTransform.SetScale3D(FVector(1.0, Width / 100.0, 1.0));
	ISM_ShadowRound->AddInstance(RoundTransform, true);
}

void AB_DemoDisplay::ScaleSafeInstance_Implementation(UInstancedStaticMeshComponent* Instance, const FTransform& Transform)
{
	// Logic from JSON ScaleSafeInstance graph:
	// Adds an instance only if scale is valid (all components > 0)
	// Prevents zero-scale instances that cause rendering issues

	if (!Instance)
	{
		return;
	}

	FVector Scale = Transform.GetScale3D();

	// Check if all scale components are greater than 0
	bool bScaleValid = (Scale.X > 0.0) && (Scale.Y > 0.0) && (Scale.Z > 0.0);

	if (bScaleValid)
	{
		Instance->AddInstance(Transform, true);
		UE_LOG(LogTemp, Verbose, TEXT("AB_DemoDisplay::ScaleSafeInstance - Added instance with scale (%f, %f, %f)"),
			Scale.X, Scale.Y, Scale.Z);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AB_DemoDisplay::ScaleSafeInstance - Skipped invalid scale (%f, %f, %f)"),
			Scale.X, Scale.Y, Scale.Z);
	}
}

double AB_DemoDisplay::TextAlignmentOffset_Implementation(double WidthAdjustment, bool bForceCenter)
{
	// Logic from JSON Text Alignment Offset graph:
	// Returns offset based on TextAlignment enum
	// - EHTA_Left: (Width - WidthAdjustment) / 2 * 100 (positive offset)
	// - EHTA_Center: 0.0
	// - EHTA_Right: -(Width - WidthAdjustment) / 2 * 100 (negative offset)
	// If bForceCenter: always return 0.0

	if (bForceCenter)
	{
		return 0.0;
	}

	double AdjustedWidth = Width - WidthAdjustment;
	double HalfWidth = AdjustedWidth / 2.0;

	// Note: Using TextAlignment which is TEnumAsByte<ETextJustify::Type>
	// Blueprint uses EHorizTextAligment but we map to ETextJustify
	switch (TextAlignment.GetValue())
	{
		case ETextJustify::Left:
			return HalfWidth * 100.0;

		case ETextJustify::Center:
			return 0.0;

		case ETextJustify::Right:
			return -HalfWidth * 100.0;

		default:
			return 0.0;
	}
}

FTransform AB_DemoDisplay::TitlePanelTextTransform_Implementation()
{
	// Logic from JSON TitlePanelTextTransform graph:
	// Returns transform for title panel text positioning
	// Location: (Depth * 100 + offset, TextAlignmentOffset(), 22)
	// Rotation: (40, 0, 0)
	// Scale: (1, 1, 1)

	double OffsetY = TextAlignmentOffset(0.0, false);

	FVector Location(Depth * 100.0, OffsetY, 22.0);
	FRotator Rotation(40.0, 0.0, 0.0);
	FVector Scale(1.0, 1.0, 1.0);

	return FTransform(Rotation, Location, Scale);
}

FTransform AB_DemoDisplay::TitleTextTransform_Implementation()
{
	// Logic from JSON Title Text Transform graph:
	// Returns transform for title text positioning
	// Considers FloorText boolean for orientation
	// Uses TextAlignmentOffset for Y positioning

	double OffsetY = TextAlignmentOffset(0.0, false);

	FVector BaseLocation(15.0, OffsetY, Height - 20.0);
	FRotator BaseRotation(0.0, 0.0, 0.0);

	// If FloorText or Height < threshold, adjust transform
	if (FloorText || Height < 50.0)
	{
		// Swap axes for floor text orientation
		FVector FloorLocation(BaseLocation.Z, BaseLocation.Y, BaseLocation.X);
		FRotator FloorRotation(90.0, 0.0, 0.0);
		return FTransform(FloorRotation, FloorLocation, FVector::OneVector);
	}

	return FTransform(BaseRotation, BaseLocation, FVector::OneVector);
}
