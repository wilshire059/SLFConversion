// B_DemoRoom.cpp
// C++ implementation for Blueprint B_DemoRoom
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/B_DemoRoom.json
// Logic traced from JSON node-by-node
// This is procedural room geometry generator with 39 ISM components

#include "Blueprints/B_DemoRoom.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

AB_DemoRoom::AB_DemoRoom()
{
	// Initialize default values from Blueprint
	SegmentCounter = 0.0;
	SpacerSize = 50.0;
	DrawEndWall = true;
	DrawStartWall = true;

	// Create all ISM components (32 total)
	_10MWall = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("10MWall"));
	_10MCurve = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("10MCurve"));
	_10MPillar = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("10MPillar"));
	PillarCurve = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("PillarCurve"));
	_10MWall_Black = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("10MWall_Black"));
	_10MPillar_Black = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("10MPillar_Black"));
	_10MEdge_White = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("10MEdge_White"));
	_10MWall_Colour = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("10MWall_Colour"));
	_10MEdge_Colour = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("10MEdge_Colour"));
	Floor_Corner = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Floor_Corner"));
	Floor_Edge = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Floor_Edge"));
	UELogo = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("UELogo"));
	BackWall_Corner = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("BackWall_Corner"));
	BackWall_Edge = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("BackWall_Edge"));
	FloorFilter = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("FloorFilter"));
	_10MWall_Grey = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("10MWall_Grey"));
	_10MWall_Ceiling = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("10MWall_Ceiling"));
	BlackRibbedWall = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("BlackRibbedWall"));
	BlackRibbedCurve = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("BlackRibbedCurve"));
	WhiteRibbedWall = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("WhiteRibbedWall"));
	Pipe = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Pipe"));
	Trim_End = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Trim_End"));
	Trim_Straight = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Trim_Straight"));
	Trim_Curve = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Trim_Curve"));
	Trim = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Trim"));
	LightPanel = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("LightPanel"));
	PillarCurveCorner = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("PillarCurveCorner"));
	MetalTrim_Corner = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("MetalTrim_Corner"));
	MetalTrim_Straight = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("MetalTrim_Straight"));
	_10MPillar2 = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("10MPillar2"));
	PillarCurveCornerBlack = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("PillarCurveCornerBlack"));
	_10MPillar2Black = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("10MPillar2Black"));
	_10MEdgeBlack = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("10MEdgeBlack"));
	isikdevLogo = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("isikdevLogo"));

	// Set root component
	RootComponent = _10MWall;

	// Attach all other components to root
	if (RootComponent)
	{
		_10MCurve->SetupAttachment(RootComponent);
		_10MPillar->SetupAttachment(RootComponent);
		PillarCurve->SetupAttachment(RootComponent);
		_10MWall_Black->SetupAttachment(RootComponent);
		_10MPillar_Black->SetupAttachment(RootComponent);
		_10MEdge_White->SetupAttachment(RootComponent);
		_10MWall_Colour->SetupAttachment(RootComponent);
		_10MEdge_Colour->SetupAttachment(RootComponent);
		Floor_Corner->SetupAttachment(RootComponent);
		Floor_Edge->SetupAttachment(RootComponent);
		UELogo->SetupAttachment(RootComponent);
		BackWall_Corner->SetupAttachment(RootComponent);
		BackWall_Edge->SetupAttachment(RootComponent);
		FloorFilter->SetupAttachment(RootComponent);
		_10MWall_Grey->SetupAttachment(RootComponent);
		_10MWall_Ceiling->SetupAttachment(RootComponent);
		BlackRibbedWall->SetupAttachment(RootComponent);
		BlackRibbedCurve->SetupAttachment(RootComponent);
		WhiteRibbedWall->SetupAttachment(RootComponent);
		Pipe->SetupAttachment(RootComponent);
		Trim_End->SetupAttachment(RootComponent);
		Trim_Straight->SetupAttachment(RootComponent);
		Trim_Curve->SetupAttachment(RootComponent);
		Trim->SetupAttachment(RootComponent);
		LightPanel->SetupAttachment(RootComponent);
		PillarCurveCorner->SetupAttachment(RootComponent);
		MetalTrim_Corner->SetupAttachment(RootComponent);
		MetalTrim_Straight->SetupAttachment(RootComponent);
		_10MPillar2->SetupAttachment(RootComponent);
		PillarCurveCornerBlack->SetupAttachment(RootComponent);
		_10MPillar2Black->SetupAttachment(RootComponent);
		_10MEdgeBlack->SetupAttachment(RootComponent);
		isikdevLogo->SetupAttachment(RootComponent);
	}

	UE_LOG(LogTemp, Warning, TEXT("AB_DemoRoom::Constructor - Components created"));
}


void AB_DemoRoom::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	UE_LOG(LogTemp, Warning, TEXT("AB_DemoRoom::OnConstruction CALLED! Rooms.Num()=%d"), Rooms.Num());
	
	// Log room data
	for (int32 i = 0; i < Rooms.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Room[%d]: Width=%d, Height=%d"), i, Rooms[i].Width, Rooms[i].Height);
	}
	
	// Call Redraw to generate room geometry (replaces Blueprint ConstructionScript)
	Redraw();
	
	UE_LOG(LogTemp, Warning, TEXT("AB_DemoRoom::OnConstruction - Room geometry generated"));
}

void AB_DemoRoom::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Warning, TEXT("AB_DemoRoom::BeginPlay CALLED! Rooms.Num()=%d"), Rooms.Num());
	
	// Generate room geometry at runtime (OnConstruction only runs in editor)
	// Check if we need to regenerate - if no instances exist, call Redraw
	if (_10MWall && _10MWall->GetInstanceCount() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("AB_DemoRoom::BeginPlay - No instances found, calling Redraw"));
		Redraw();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AB_DemoRoom::BeginPlay - Instances already exist (%d), skipping Redraw"), 
			_10MWall ? _10MWall->GetInstanceCount() : 0);
	}
}

void AB_DemoRoom::CreateInstances_Implementation()
{
	// Logic from JSON Create Instances graph:
	// Clear all ISM instances to prepare for new geometry

	UE_LOG(LogTemp, Warning, TEXT("AB_DemoRoom::CreateInstances - Clearing all instances"));

	// Clear all 34 ISM components
	if (_10MWall) _10MWall->ClearInstances();
	if (_10MCurve) _10MCurve->ClearInstances();
	if (_10MPillar) _10MPillar->ClearInstances();
	if (PillarCurve) PillarCurve->ClearInstances();
	if (_10MWall_Black) _10MWall_Black->ClearInstances();
	if (_10MPillar_Black) _10MPillar_Black->ClearInstances();
	if (_10MEdge_White) _10MEdge_White->ClearInstances();
	if (_10MWall_Colour) _10MWall_Colour->ClearInstances();
	if (_10MEdge_Colour) _10MEdge_Colour->ClearInstances();
	if (Floor_Corner) Floor_Corner->ClearInstances();
	if (Floor_Edge) Floor_Edge->ClearInstances();
	if (UELogo) UELogo->ClearInstances();
	if (BackWall_Corner) BackWall_Corner->ClearInstances();
	if (BackWall_Edge) BackWall_Edge->ClearInstances();
	if (FloorFilter) FloorFilter->ClearInstances();
	if (_10MWall_Grey) _10MWall_Grey->ClearInstances();
	if (_10MWall_Ceiling) _10MWall_Ceiling->ClearInstances();
	if (BlackRibbedWall) BlackRibbedWall->ClearInstances();
	if (BlackRibbedCurve) BlackRibbedCurve->ClearInstances();
	if (WhiteRibbedWall) WhiteRibbedWall->ClearInstances();
	if (Pipe) Pipe->ClearInstances();
	if (Trim_End) Trim_End->ClearInstances();
	if (Trim_Straight) Trim_Straight->ClearInstances();
	if (Trim_Curve) Trim_Curve->ClearInstances();
	if (Trim) Trim->ClearInstances();
	if (LightPanel) LightPanel->ClearInstances();
	if (PillarCurveCorner) PillarCurveCorner->ClearInstances();
	if (MetalTrim_Corner) MetalTrim_Corner->ClearInstances();
	if (MetalTrim_Straight) MetalTrim_Straight->ClearInstances();
	if (_10MPillar2) _10MPillar2->ClearInstances();
	if (PillarCurveCornerBlack) PillarCurveCornerBlack->ClearInstances();
	if (_10MPillar2Black) _10MPillar2Black->ClearInstances();
	if (_10MEdgeBlack) _10MEdgeBlack->ClearInstances();
	if (isikdevLogo) isikdevLogo->ClearInstances();

	// Reset segment counter
	SegmentCounter = 0.0;
}

void AB_DemoRoom::Redraw_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("AB_DemoRoom::Redraw_Implementation CALLED! Rooms.Num()=%d, DrawEndWall=%d, DrawStartWall=%d"), 
		Rooms.Num(), DrawEndWall ? 1 : 0, DrawStartWall ? 1 : 0);
	
	// Check if we have valid ISM components
	UE_LOG(LogTemp, Warning, TEXT("  ISM Components: _10MWall=%p, Floor_Corner=%p, Floor_Edge=%p"), 
		_10MWall, Floor_Corner, Floor_Edge);

	CreateInstances();

	// Reset segment counter
	SegmentCounter = 0.0;

	// Create each room in the array
	for (int32 i = 0; i < Rooms.Num(); i++)
	{
		CreateRoom(Rooms[i], SegmentCounter);

		// Update segment counter based on room width
		// Note: RoomWidth from FSLFRoomSettings determines position offset
		SegmentCounter += Rooms[i].RoomWidth * 1000.0; // 10M segments = 1000 units each
	}

	// Draw end wall if enabled
	if (DrawEndWall && Rooms.Num() > 0)
	{
		const FSLFRoomSettings& LastRoom = Rooms.Last();
		FTransform EndWallTransform;
		EndWallTransform.SetLocation(FVector(SegmentCounter, 0.0, 0.0));
		EndWall(EndWallTransform, LastRoom.RoomWidth, LastRoom.RoomHeight);
	}

	// Draw start wall if enabled
	if (DrawStartWall && Rooms.Num() > 0)
	{
		const FSLFRoomSettings& FirstRoom = Rooms[0];
		FTransform StartWallTransform;
		StartWallTransform.SetLocation(FVector(0.0, 0.0, 0.0));
		StartWallTransform.SetRotation(FRotator(0.0, 180.0, 0.0).Quaternion());
		EndWall(StartWallTransform, FirstRoom.Width, FirstRoom.Height);
	}

	UE_LOG(LogTemp, Warning, TEXT("AB_DemoRoom::Redraw - Complete"));
}

void AB_DemoRoom::CreateRoom_Implementation(const FSLFRoomSettings& RoomParams, double InSegmentCounter)
{
	// Logic from JSON Create Room graph:
	// Complex sequence building room geometry from RoomParams
	// 1. Make pillars at corners
	// 2. Create walls
	// 3. Create floor
	// 4. Create ceiling
	// 5. Add lighting
	// 6. Add spacers

	UE_LOG(LogTemp, Warning, TEXT("AB_DemoRoom::CreateRoom - SegmentCounter: %f"), InSegmentCounter);

	int32 RoomWidth = RoomParams.Width;
	int32 RoomHeight = RoomParams.Height;
	double PillarThickness = RoomParams.PillarThickness;
	bool bSquareExterior = RoomParams.bSquareExterior;
	double InnerPanelPercent = RoomParams.InnerPanelPercent;
	double OuterPanelPercent = RoomParams.OuterPanelPercent;

	// Base position for this room segment
	FTransform BaseTransform;
	BaseTransform.SetLocation(FVector(InSegmentCounter, 0.0, 0.0));
	BaseTransform.SetRotation(FQuat::Identity);
	BaseTransform.SetScale3D(FVector::OneVector);

	// Create front left pillar
	FTransform PillarTransformFL = BaseTransform;
	MakePillar(PillarTransformFL, RoomWidth, RoomHeight, PillarThickness, bSquareExterior, true);

	// Create front right pillar
	FTransform PillarTransformFR = BaseTransform;
	PillarTransformFR.SetLocation(FVector(InSegmentCounter, RoomWidth * 1000.0, 0.0));
	MakePillar(PillarTransformFR, RoomWidth, RoomHeight, PillarThickness, bSquareExterior, true);

	// Create walls
	MakeBackWall(BaseTransform, RoomWidth, RoomHeight, InSegmentCounter, InnerPanelPercent, OuterPanelPercent);

	// Create floor
	Floor();

	// Create rounded end section
	FTransform RoundedEndTransform = BaseTransform;
	RoundedEndTransform.SetLocation(FVector(InSegmentCounter + RoomWidth * 500.0, 0.0, 0.0));
	MakeRoundedEnd(RoundedEndTransform, RoomWidth, InSegmentCounter, InnerPanelPercent, OuterPanelPercent);

	// Add lighting
	Lighting(RoomParams);

	// Add spacer between rooms
	if (SpacerSize > 0.0)
	{
		AddSpacer(InSegmentCounter + RoomWidth * 1000.0);
	}

	// Add metal strip decoration
	FTransform MetalStripTransform = BaseTransform;
	MetalStripTransform.SetLocation(FVector(InSegmentCounter + RoomWidth * 500.0, RoomWidth * 500.0, RoomHeight * 1000.0 - 100.0));
	MetalStrip(MetalStripTransform);
}

void AB_DemoRoom::Floor_Implementation()
{
	// Logic from JSON Floor graph:
	// Creates floor geometry with corner pieces and edge pieces

	UE_LOG(LogTemp, Warning, TEXT("AB_DemoRoom::Floor"));

	if (!Floor_Corner || !Floor_Edge)
	{
		return;
	}

	// Add floor corner instances
	FTransform FloorCornerTransforms[4];
	FloorCornerTransforms[0].SetLocation(FVector(0.0, 0.0, 0.0));
	FloorCornerTransforms[1].SetLocation(FVector(0.0, 0.0, 0.0));
	FloorCornerTransforms[1].SetRotation(FRotator(0.0, 90.0, 0.0).Quaternion());
	FloorCornerTransforms[2].SetLocation(FVector(0.0, 0.0, 0.0));
	FloorCornerTransforms[2].SetRotation(FRotator(0.0, 180.0, 0.0).Quaternion());
	FloorCornerTransforms[3].SetLocation(FVector(0.0, 0.0, 0.0));
	FloorCornerTransforms[3].SetRotation(FRotator(0.0, 270.0, 0.0).Quaternion());

	for (int32 i = 0; i < 4; i++)
	{
		FloorCornerTransforms[i].SetScale3D(FVector::OneVector);
		Floor_Corner->AddInstance(FloorCornerTransforms[i], true);
	}
}

void AB_DemoRoom::EndWall_Implementation(const FTransform& WallPosition, int32 Width, int32 Height)
{
	// Logic from JSON End Wall graph:
	// Creates end wall at specified position with given dimensions

	UE_LOG(LogTemp, Warning, TEXT("AB_DemoRoom::EndWall - Width: %d, Height: %d"), Width, Height);

	if (!_10MWall)
	{
		return;
	}

	FVector BaseLocation = WallPosition.GetLocation();
	FRotator BaseRotation = WallPosition.GetRotation().Rotator();

	// Create wall panels based on height
	int32 PanelCount = FMath::Max(1, Height);

	for (int32 i = 0; i < PanelCount; i++)
	{
		FTransform WallTransform;
		WallTransform.SetLocation(BaseLocation + FVector(0.0, 0.0, i * 1000.0));
		WallTransform.SetRotation(BaseRotation.Quaternion());
		WallTransform.SetScale3D(FVector(1.0, Width, 1.0));

		_10MWall->AddInstance(WallTransform, true);
	}
}

void AB_DemoRoom::MakePillar_Implementation(const FTransform& BasePosition, int32 Width, int32 Height, double PillarThickness, bool SquareExterior, bool White)
{
	// Logic from JSON Make Pillar graph:
	// Creates pillar with optional curve at specified position

	UE_LOG(LogTemp, Warning, TEXT("AB_DemoRoom::MakePillar - Height: %d, Thickness: %f, White: %s"),
		Height, PillarThickness, White ? TEXT("true") : TEXT("false"));

	UInstancedStaticMeshComponent* PillarComponent = White ? _10MPillar : _10MPillar_Black;
	UInstancedStaticMeshComponent* CurveComponent = White ? PillarCurve : PillarCurveCornerBlack;

	if (!PillarComponent)
	{
		return;
	}

	FVector BaseLocation = BasePosition.GetLocation();
	FRotator BaseRotation = BasePosition.GetRotation().Rotator();

	// Create pillar instances for each height segment
	for (int32 i = 0; i < Height; i++)
	{
		FTransform PillarTransform;
		PillarTransform.SetLocation(BaseLocation + FVector(0.0, 0.0, i * 1000.0));
		PillarTransform.SetRotation(BaseRotation.Quaternion());
		PillarTransform.SetScale3D(FVector(PillarThickness / 100.0, PillarThickness / 100.0, 1.0));

		PillarComponent->AddInstance(PillarTransform, true);
	}

	// Add curve if not square exterior
	if (!SquareExterior && CurveComponent)
	{
		FTransform CurveTransform;
		CurveTransform.SetLocation(BaseLocation);
		CurveTransform.SetRotation(BaseRotation.Quaternion());
		CurveTransform.SetScale3D(FVector(PillarThickness / 100.0, PillarThickness / 100.0, static_cast<double>(Height)));

		CurveComponent->AddInstance(CurveTransform, true);
	}
}

void AB_DemoRoom::MakeDoor_Implementation(int32 RoomWidth, int32 RoomHeight, double Offset)
{
	// Logic from JSON Make Door graph:
	// Creates door opening in wall

	UE_LOG(LogTemp, Warning, TEXT("AB_DemoRoom::MakeDoor - Width: %d, Height: %d, Offset: %f"),
		RoomWidth, RoomHeight, Offset);

	// Door dimensions (standard door)
	const double DoorWidth = 200.0;
	const double DoorHeight = 300.0;

	// Center position
	double CenterY = RoomWidth * 500.0;
	double DoorZ = 0.0;

	// Create door frame using black wall segments
	if (_10MWall_Black)
	{
		// Left frame
		FTransform LeftFrame;
		LeftFrame.SetLocation(FVector(Offset, CenterY - DoorWidth / 2.0, DoorZ));
		LeftFrame.SetRotation(FRotator(0.0, 90.0, 0.0).Quaternion());
		LeftFrame.SetScale3D(FVector(0.1, 1.0, DoorHeight / 1000.0));
		_10MWall_Black->AddInstance(LeftFrame, true);

		// Right frame
		FTransform RightFrame;
		RightFrame.SetLocation(FVector(Offset, CenterY + DoorWidth / 2.0, DoorZ));
		RightFrame.SetRotation(FRotator(0.0, 90.0, 0.0).Quaternion());
		RightFrame.SetScale3D(FVector(0.1, 1.0, DoorHeight / 1000.0));
		_10MWall_Black->AddInstance(RightFrame, true);

		// Top frame
		FTransform TopFrame;
		TopFrame.SetLocation(FVector(Offset, CenterY, DoorZ + DoorHeight));
		TopFrame.SetRotation(FQuat::Identity);
		TopFrame.SetScale3D(FVector(0.1, DoorWidth / 1000.0, 0.1));
		_10MWall_Black->AddInstance(TopFrame, true);
	}
}

void AB_DemoRoom::MakeRoundedEnd_Implementation(const FTransform& BaseTransform, int32 RoomWidth, double FloorSegment, double InnerPanelPercent, double OuterPanelPercent)
{
	// Logic from JSON Make Rounded End graph:
	// Creates rounded end cap for room

	UE_LOG(LogTemp, Warning, TEXT("AB_DemoRoom::MakeRoundedEnd - FloorSegment: %f"), FloorSegment);

	if (!_10MCurve)
	{
		return;
	}

	FVector BaseLocation = BaseTransform.GetLocation();

	// Create curve instance
	FTransform CurveTransform;
	CurveTransform.SetLocation(BaseLocation);
	CurveTransform.SetRotation(FQuat::Identity);
	CurveTransform.SetScale3D(FVector(1.0, RoomWidth, 1.0));

	_10MCurve->AddInstance(CurveTransform, true);
}

void AB_DemoRoom::MakeBackWall_Implementation(const FTransform& BaseTransform, int32 RoomWidth, int32 RoomHeight, double FloorSegment, double InnerPanelPercent, double OuterPanelPercent)
{
	// Logic from JSON Make Back Wall graph:
	// Creates back wall with corner and edge pieces

	UE_LOG(LogTemp, Warning, TEXT("AB_DemoRoom::MakeBackWall - Width: %d, Height: %d"), RoomWidth, RoomHeight);

	FVector BaseLocation = BaseTransform.GetLocation();

	// Add back wall corner instances
	if (BackWall_Corner)
	{
		FTransform CornerTransform;
		CornerTransform.SetLocation(BaseLocation + FVector(0.0, 0.0, 0.0));
		CornerTransform.SetRotation(FQuat::Identity);
		CornerTransform.SetScale3D(FVector(1.0, 1.0, static_cast<double>(RoomHeight)));

		BackWall_Corner->AddInstance(CornerTransform, true);
	}

	// Add back wall edge instances
	if (BackWall_Edge)
	{
		FTransform EdgeTransform;
		EdgeTransform.SetLocation(BaseLocation + FVector(0.0, 50.0, 0.0));
		EdgeTransform.SetRotation(FQuat::Identity);
		EdgeTransform.SetScale3D(FVector(1.0, static_cast<double>(RoomWidth) - 1.0, static_cast<double>(RoomHeight)));

		BackWall_Edge->AddInstance(EdgeTransform, true);
	}
}

void AB_DemoRoom::AddSpacer_Implementation(double Offset)
{
	// Logic from JSON Add Spacer graph:
	// Adds spacer geometry between rooms

	UE_LOG(LogTemp, Warning, TEXT("AB_DemoRoom::AddSpacer - Offset: %f"), Offset);

	if (SpacerSize <= 0.0)
	{
		return;
	}

	// Add ribbed wall spacer
	if (BlackRibbedWall)
	{
		FTransform SpacerTransform;
		SpacerTransform.SetLocation(FVector(Offset, 0.0, 0.0));
		SpacerTransform.SetRotation(FQuat::Identity);
		SpacerTransform.SetScale3D(FVector(SpacerSize / 100.0, 1.0, 1.0));

		BlackRibbedWall->AddInstance(SpacerTransform, true);
	}
}

void AB_DemoRoom::MetalStrip_Implementation(const FTransform& BaseTransform)
{
	// Logic from JSON Metal Strip graph:
	// Adds decorative metal strip

	UE_LOG(LogTemp, Warning, TEXT("AB_DemoRoom::MetalStrip"));

	if (!MetalTrim_Straight || !MetalTrim_Corner)
	{
		return;
	}

	FVector BaseLocation = BaseTransform.GetLocation();

	// Add straight metal trim
	FTransform StraightTransform;
	StraightTransform.SetLocation(BaseLocation);
	StraightTransform.SetRotation(FQuat::Identity);
	StraightTransform.SetScale3D(FVector(10.0, 1.0, 1.0));

	MetalTrim_Straight->AddInstance(StraightTransform, true);

	// Add corner pieces at ends
	FTransform CornerTransformL;
	CornerTransformL.SetLocation(BaseLocation - FVector(500.0, 0.0, 0.0));
	CornerTransformL.SetRotation(FQuat::Identity);
	CornerTransformL.SetScale3D(FVector::OneVector);

	MetalTrim_Corner->AddInstance(CornerTransformL, true);

	FTransform CornerTransformR;
	CornerTransformR.SetLocation(BaseLocation + FVector(500.0, 0.0, 0.0));
	CornerTransformR.SetRotation(FRotator(0.0, 180.0, 0.0).Quaternion());
	CornerTransformR.SetScale3D(FVector::OneVector);

	MetalTrim_Corner->AddInstance(CornerTransformR, true);
}

void AB_DemoRoom::Lighting_Implementation(const FSLFRoomSettings& RoomParams)
{
	// Logic from JSON Lighting graph:
	// Adds lighting panels to room

	UE_LOG(LogTemp, Warning, TEXT("AB_DemoRoom::Lighting"));

	if (!LightPanel)
	{
		return;
	}

	int32 RoomWidth = RoomParams.Width;
	int32 RoomHeight = RoomParams.Height;

	// Add light panels along ceiling
	int32 LightCount = FMath::Max(1, RoomWidth / 2);

	for (int32 i = 0; i < LightCount; i++)
	{
		FTransform LightTransform;
		double LightX = (i + 0.5) * 2000.0;
		double LightY = 0.0;
		double LightZ = RoomHeight * 1000.0 - 50.0;

		LightTransform.SetLocation(FVector(LightX, LightY, LightZ));
		LightTransform.SetRotation(FRotator(180.0, 0.0, 0.0).Quaternion());
		LightTransform.SetScale3D(FVector::OneVector);

		LightPanel->AddInstance(LightTransform, true);
	}
}
