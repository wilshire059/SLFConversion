// SLFCaveMazeBuilder.cpp
// Design-driven dungeon maze: load layout JSON, build walls, overlay viz, gameplay actors.

#include "SLFCaveMazeBuilder.h"

#if WITH_EDITOR
#include "Core/Dungeon.h"
#include "Core/DungeonModel.h"
#include "Builders/CellFlow/CellFlowModel.h"
#include "Frameworks/FlowImpl/CellFlow/Lib/CellFlowLib.h"
#include "Blueprints/Actors/SLFShortcutGate.h"
#include "Blueprints/Actors/SLFTrapBase.h"
#include "Blueprints/Actors/SLFPuzzleMarker.h"
#include "Blueprints/Actors/SLFBossDoor.h"
#include "SLFEnums.h"

#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/PointLight.h"
#include "Engine/DirectionalLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/SceneCapture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/TextRenderActor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "Misc/FileHelper.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonReader.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "RenderingThread.h"
#include "CollisionQueryParams.h"

// Reuse SaveCaptureToPng from SLFConversion.cpp (declared static there).
// We duplicate the PNG save logic here to keep the builder self-contained.
static bool SaveCaptureToPngLocal(UTextureRenderTarget2D* RenderTarget, int32 Resolution,
	const FString& OutputFile, const FString& Label)
{
	FTextureRenderTargetResource* RTResource = RenderTarget->GameThread_GetRenderTargetResource();
	if (!RTResource)
	{
		UE_LOG(LogTemp, Error, TEXT("[CaveMaze] %s: No render target resource!"), *Label);
		return false;
	}

	TArray<FColor> Pixels;
	if (!RTResource->ReadPixels(Pixels, FReadSurfaceDataFlags(RCM_UNorm)))
	{
		UE_LOG(LogTemp, Error, TEXT("[CaveMaze] %s: ReadPixels failed!"), *Label);
		return false;
	}

	for (FColor& P : Pixels) P.A = 255;

	IImageWrapperModule& ImgModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> PngWrapper = ImgModule.CreateImageWrapper(EImageFormat::PNG);
	PngWrapper->SetRaw(Pixels.GetData(), Pixels.Num() * sizeof(FColor), Resolution, Resolution, ERGBFormat::BGRA, 8);
	TArray64<uint8> PngData = PngWrapper->GetCompressed(100);

	FString OutputDir = FPaths::GetPath(OutputFile);
	IFileManager::Get().MakeDirectory(*OutputDir, true);

	if (FFileHelper::SaveArrayToFile(PngData, *OutputFile))
	{
		UE_LOG(LogTemp, Warning, TEXT("[CaveMaze] %s: Saved %s (%.1f KB)"),
			*Label, *OutputFile, PngData.Num() / 1024.0f);
		return true;
	}
	UE_LOG(LogTemp, Error, TEXT("[CaveMaze] %s: Failed to save PNG!"), *Label);
	return false;
}

// ═══════════════════════════════════════════════════════════════════════════════
// Phase B: Overhead Floor Capture
// ═══════════════════════════════════════════════════════════════════════════════

void FSLFCaveMazeBuilder::CaptureCaveFloor(UWorld* World, int32 DungeonIndex, int32 Resolution)
{
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("[CaveMaze] CaptureCaveFloor: No world!"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[CaveMaze] CaptureCaveFloor: Dungeon %d, Resolution %d"), DungeonIndex + 1, Resolution);

	FBox DungeonBounds = ComputeDungeonBounds(World);
	if (!DungeonBounds.IsValid)
	{
		UE_LOG(LogTemp, Error, TEXT("[CaveMaze] CaptureCaveFloor: No valid bounds!"));
		return;
	}

	FVector Center = DungeonBounds.GetCenter();
	FVector Size = DungeonBounds.GetSize();

	// Hide ceiling geometry: primitive components with bounds center above dungeon midpoint
	float CeilingThreshold = Center.Z + 400.0f;
	TArray<UPrimitiveComponent*> HiddenComponents;

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		TArray<UPrimitiveComponent*> PrimComps;
		(*It)->GetComponents<UPrimitiveComponent>(PrimComps);
		for (UPrimitiveComponent* PC : PrimComps)
		{
			if (PC && PC->Bounds.Origin.Z > CeilingThreshold)
			{
				PC->SetVisibility(false);
				HiddenComponents.Add(PC);
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[CaveMaze]   Hidden %d ceiling components"), HiddenComponents.Num());

	float OrthoWidth = FMath::Max(Size.X, Size.Y) * 1.05f;
	float CaptureHeight = Center.Z + Size.Z * 0.5f + 5000.0f;

	FString OutputDir = FPaths::ProjectDir() / TEXT("MapCapture");
	IFileManager::Get().MakeDirectory(*OutputDir, true);

	// Shared render target
	UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>(GetTransientPackage());
	RenderTarget->RenderTargetFormat = RTF_RGBA8;
	RenderTarget->InitAutoFormat(Resolution, Resolution);
	RenderTarget->UpdateResourceImmediate(true);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Shot 1: Unlit floor (base color only — geometry clearly visible)
	{
		ASceneCapture2D* CaptureActor = World->SpawnActor<ASceneCapture2D>(
			ASceneCapture2D::StaticClass(),
			FVector(Center.X, Center.Y, CaptureHeight),
			FRotator(-90.0f, 0.0f, 0.0f),
			SpawnParams);

		if (CaptureActor)
		{
			USceneCaptureComponent2D* CC = CaptureActor->GetCaptureComponent2D();
			CC->ProjectionType = ECameraProjectionMode::Orthographic;
			CC->OrthoWidth = OrthoWidth;
			CC->TextureTarget = RenderTarget;
			CC->CaptureSource = ESceneCaptureSource::SCS_BaseColor;
			CC->bCaptureEveryFrame = false;
			CC->bCaptureOnMovement = false;
			CC->ShowFlags.SetMotionBlur(false);
			CC->ShowFlags.SetBloom(false);
			CC->ShowFlags.SetFog(false);
			CC->ShowFlags.SetVolumetricFog(false);

			CC->CaptureScene();
			FlushRenderingCommands();

			FString File = OutputDir / FString::Printf(TEXT("Dungeon_%02d_floor_unlit.png"), DungeonIndex + 1);
			SaveCaptureToPngLocal(RenderTarget, Resolution, File, TEXT("Floor unlit"));
			CaptureActor->Destroy();
		}
	}

	// Shot 2: Lit floor (with temporary directional light pointing down)
	{
		ADirectionalLight* TempLight = World->SpawnActor<ADirectionalLight>(
			FVector(Center.X, Center.Y, CaptureHeight - 1000.0f),
			FRotator(-90.0f, 0.0f, 0.0f), SpawnParams);

		if (TempLight)
		{
			TempLight->GetComponent()->SetIntensity(50000.0f);
			TempLight->GetComponent()->SetLightColor(FLinearColor(0.9f, 0.9f, 1.0f));
		}

		ASceneCapture2D* CaptureActor = World->SpawnActor<ASceneCapture2D>(
			ASceneCapture2D::StaticClass(),
			FVector(Center.X, Center.Y, CaptureHeight),
			FRotator(-90.0f, 0.0f, 0.0f),
			SpawnParams);

		if (CaptureActor)
		{
			USceneCaptureComponent2D* CC = CaptureActor->GetCaptureComponent2D();
			CC->ProjectionType = ECameraProjectionMode::Orthographic;
			CC->OrthoWidth = OrthoWidth;
			CC->TextureTarget = RenderTarget;
			CC->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
			CC->bCaptureEveryFrame = false;
			CC->bCaptureOnMovement = false;
			CC->ShowFlags.SetMotionBlur(false);
			CC->ShowFlags.SetBloom(false);
			CC->ShowFlags.SetEyeAdaptation(false);
			CC->ShowFlags.SetFog(false);

			CC->CaptureScene();
			FlushRenderingCommands();

			FString File = OutputDir / FString::Printf(TEXT("Dungeon_%02d_floor_lit.png"), DungeonIndex + 1);
			SaveCaptureToPngLocal(RenderTarget, Resolution, File, TEXT("Floor lit"));
			CaptureActor->Destroy();
		}

		if (TempLight) TempLight->Destroy();
	}

	// Restore hidden ceiling components
	for (UPrimitiveComponent* PC : HiddenComponents)
	{
		if (PC) PC->SetVisibility(true);
	}

	UE_LOG(LogTemp, Warning, TEXT("[CaveMaze] CaptureCaveFloor: DONE — restored %d ceiling components"), HiddenComponents.Num());
}

// ═══════════════════════════════════════════════════════════════════════════════
// Phase C: Cell Graph Export
// ═══════════════════════════════════════════════════════════════════════════════

void FSLFCaveMazeBuilder::ExportCellGraphToJson(ADungeon* Dungeon, int32 DungeonIndex,
	const FVector& DungeonOrigin, const FVector& GridSize)
{
	if (!Dungeon)
	{
		UE_LOG(LogTemp, Error, TEXT("[CaveMaze] ExportCellGraph: NULL dungeon"));
		return;
	}

	UCellFlowModel* CellModel = Cast<UCellFlowModel>(Dungeon->GetModel());
	if (!CellModel || !CellModel->CellGraph)
	{
		UE_LOG(LogTemp, Error, TEXT("[CaveMaze] ExportCellGraph: No CellFlowModel/CellGraph"));
		return;
	}

	UDAFlowCellGraph* CellGraph = CellModel->CellGraph;

	TSharedPtr<FJsonObject> Root = MakeShared<FJsonObject>();
	Root->SetNumberField(TEXT("dungeonIndex"), DungeonIndex);

	// Origin
	TArray<TSharedPtr<FJsonValue>> OriginArr;
	OriginArr.Add(MakeShared<FJsonValueNumber>(DungeonOrigin.X));
	OriginArr.Add(MakeShared<FJsonValueNumber>(DungeonOrigin.Y));
	OriginArr.Add(MakeShared<FJsonValueNumber>(DungeonOrigin.Z));
	Root->SetArrayField(TEXT("origin"), OriginArr);

	// Grid size
	TArray<TSharedPtr<FJsonValue>> GridArr;
	GridArr.Add(MakeShared<FJsonValueNumber>(GridSize.X));
	GridArr.Add(MakeShared<FJsonValueNumber>(GridSize.Y));
	GridArr.Add(MakeShared<FJsonValueNumber>(GridSize.Z));
	Root->SetArrayField(TEXT("gridSize"), GridArr);

	// Groups (rooms)
	TArray<TSharedPtr<FJsonValue>> GroupsArr;
	for (int32 GIdx = 0; GIdx < CellGraph->GroupNodes.Num(); GIdx++)
	{
		const FDAFlowCellGroupNode& Group = CellGraph->GroupNodes[GIdx];
		if (!Group.IsActive()) continue;

		TSharedPtr<FJsonObject> GObj = MakeShared<FJsonObject>();
		GObj->SetNumberField(TEXT("groupId"), Group.GroupId);
		GObj->SetNumberField(TEXT("groupHeight"), Group.GroupHeight);

		// Compute center from leaf nodes
		FVector2D Center2D = FVector2D::ZeroVector;
		float TotalArea = 0.0f;
		int32 LeafCount = 0;

		TArray<TSharedPtr<FJsonValue>> LeavesArr;
		for (int32 LeafIdx : Group.LeafNodes)
		{
			if (LeafIdx >= 0 && LeafIdx < CellGraph->LeafNodes.Num())
			{
				UDAFlowCellLeafNode* Leaf = CellGraph->LeafNodes[LeafIdx];
				if (Leaf)
				{
					Center2D += FVector2d(Leaf->GetCenter());
					float Area = Leaf->GetArea();
					TotalArea += Area;
					LeafCount++;

					TSharedPtr<FJsonObject> LObj = MakeShared<FJsonObject>();
					LObj->SetNumberField(TEXT("cellId"), LeafIdx);
					FVector2D LC = FVector2d(Leaf->GetCenter());
					TArray<TSharedPtr<FJsonValue>> LCArr;
					LCArr.Add(MakeShared<FJsonValueNumber>(DungeonOrigin.X + LC.X * GridSize.X));
					LCArr.Add(MakeShared<FJsonValueNumber>(DungeonOrigin.Y + LC.Y * GridSize.Y));
					LObj->SetArrayField(TEXT("center"), LCArr);
					LObj->SetNumberField(TEXT("area"), Area);
					LeavesArr.Add(MakeShared<FJsonValueObject>(LObj));
				}
			}
		}

		if (LeafCount == 0) continue;
		Center2D /= (float)LeafCount;

		FVector WorldCenter(
			DungeonOrigin.X + Center2D.X * GridSize.X,
			DungeonOrigin.Y + Center2D.Y * GridSize.Y,
			DungeonOrigin.Z + (float)Group.GroupHeight * GridSize.Z
		);
		float Radius = FMath::Sqrt(TotalArea) * GridSize.X * 0.5f;

		TArray<TSharedPtr<FJsonValue>> CenterArr;
		CenterArr.Add(MakeShared<FJsonValueNumber>(WorldCenter.X));
		CenterArr.Add(MakeShared<FJsonValueNumber>(WorldCenter.Y));
		CenterArr.Add(MakeShared<FJsonValueNumber>(WorldCenter.Z));
		GObj->SetArrayField(TEXT("center"), CenterArr);
		GObj->SetNumberField(TEXT("radius"), Radius);
		GObj->SetNumberField(TEXT("leafCount"), LeafCount);
		GObj->SetNumberField(TEXT("totalArea"), TotalArea);
		GObj->SetArrayField(TEXT("leaves"), LeavesArr);

		// Connected group IDs
		TArray<TSharedPtr<FJsonValue>> ConnsArr;
		for (int32 ConnIdx : Group.Connections)
		{
			ConnsArr.Add(MakeShared<FJsonValueNumber>(ConnIdx));
		}
		GObj->SetArrayField(TEXT("connections"), ConnsArr);

		GroupsArr.Add(MakeShared<FJsonValueObject>(GObj));
	}
	Root->SetArrayField(TEXT("groups"), GroupsArr);

	// Serialize to file
	FString OutputDir = FPaths::ProjectDir() / TEXT("MapCapture");
	IFileManager::Get().MakeDirectory(*OutputDir, true);
	FString OutputPath = OutputDir / FString::Printf(TEXT("dungeon_%02d_cellgraph.json"), DungeonIndex + 1);

	FString JsonStr;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonStr);
	FJsonSerializer::Serialize(Root.ToSharedRef(), Writer);
	FFileHelper::SaveStringToFile(JsonStr, *OutputPath);

	UE_LOG(LogTemp, Warning, TEXT("[CaveMaze] Exported cell graph: %d groups to %s"), GroupsArr.Num(), *OutputPath);
}

// ═══════════════════════════════════════════════════════════════════════════════
// Phase C: Template Layout Generation
// ═══════════════════════════════════════════════════════════════════════════════

void FSLFCaveMazeBuilder::GenerateTemplateLayout(UWorld* World, int32 DungeonIndex)
{
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("[CaveMaze] GenerateTemplateLayout: No world!"));
		return;
	}

	// Find the ADungeon actor
	ADungeon* Dungeon = FindDungeonActor(World);
	if (!Dungeon)
	{
		UE_LOG(LogTemp, Error, TEXT("[CaveMaze] GenerateTemplateLayout: No ADungeon found!"));
		return;
	}

	UCellFlowModel* CellModel = Cast<UCellFlowModel>(Dungeon->GetModel());
	if (!CellModel || !CellModel->CellGraph)
	{
		UE_LOG(LogTemp, Error, TEXT("[CaveMaze] GenerateTemplateLayout: No CellFlowModel!"));
		return;
	}

	UDAFlowCellGraph* CellGraph = CellModel->CellGraph;
	FVector DungeonOrigin = Dungeon->GetActorLocation();
	FVector GridSize(300, 300, 250); // Must match commandlet

	// Build template JSON with rooms pre-filled
	TSharedPtr<FJsonObject> Root = MakeShared<FJsonObject>();
	Root->SetNumberField(TEXT("dungeonIndex"), DungeonIndex);

	TArray<TSharedPtr<FJsonValue>> OriginArr;
	OriginArr.Add(MakeShared<FJsonValueNumber>(DungeonOrigin.X));
	OriginArr.Add(MakeShared<FJsonValueNumber>(DungeonOrigin.Y));
	OriginArr.Add(MakeShared<FJsonValueNumber>(DungeonOrigin.Z));
	Root->SetArrayField(TEXT("origin"), OriginArr);

	FBox Bounds = ComputeDungeonBounds(World);
	FVector Extent = Bounds.IsValid ? Bounds.GetSize() * 0.5f : FVector(6000, 6000, 2000);
	TArray<TSharedPtr<FJsonValue>> ExtArr;
	ExtArr.Add(MakeShared<FJsonValueNumber>(Extent.X));
	ExtArr.Add(MakeShared<FJsonValueNumber>(Extent.Y));
	ExtArr.Add(MakeShared<FJsonValueNumber>(Extent.Z));
	Root->SetArrayField(TEXT("extent"), ExtArr);

	// Rooms from cell graph groups
	TArray<TSharedPtr<FJsonValue>> RoomsArr;
	for (int32 GIdx = 0; GIdx < CellGraph->GroupNodes.Num(); GIdx++)
	{
		const FDAFlowCellGroupNode& Group = CellGraph->GroupNodes[GIdx];
		if (!Group.IsActive()) continue;

		FVector2D Center2D = FVector2D::ZeroVector;
		float TotalArea = 0.0f;
		int32 LeafCount = 0;

		for (int32 LeafIdx : Group.LeafNodes)
		{
			if (LeafIdx >= 0 && LeafIdx < CellGraph->LeafNodes.Num())
			{
				UDAFlowCellLeafNode* Leaf = CellGraph->LeafNodes[LeafIdx];
				if (Leaf)
				{
					Center2D += FVector2d(Leaf->GetCenter());
					TotalArea += Leaf->GetArea();
					LeafCount++;
				}
			}
		}
		if (LeafCount == 0) continue;
		Center2D /= (float)LeafCount;

		FVector WorldCenter(
			DungeonOrigin.X + Center2D.X * GridSize.X,
			DungeonOrigin.Y + Center2D.Y * GridSize.Y,
			DungeonOrigin.Z + (float)Group.GroupHeight * GridSize.Z
		);
		float Radius = FMath::Sqrt(TotalArea) * GridSize.X * 0.5f;

		TSharedPtr<FJsonObject> RObj = MakeShared<FJsonObject>();
		RObj->SetNumberField(TEXT("groupId"), Group.GroupId);
		RObj->SetStringField(TEXT("designation"), TEXT("Corridor")); // Default — Claude fills in
		TArray<TSharedPtr<FJsonValue>> CArr;
		CArr.Add(MakeShared<FJsonValueNumber>(WorldCenter.X));
		CArr.Add(MakeShared<FJsonValueNumber>(WorldCenter.Y));
		CArr.Add(MakeShared<FJsonValueNumber>(WorldCenter.Z));
		RObj->SetArrayField(TEXT("center"), CArr);
		RObj->SetNumberField(TEXT("radius"), Radius);

		// Connected groups
		TArray<TSharedPtr<FJsonValue>> ConnsArr;
		for (int32 ConnIdx : Group.Connections)
		{
			ConnsArr.Add(MakeShared<FJsonValueNumber>(ConnIdx));
		}
		RObj->SetArrayField(TEXT("connectedRoomIds"), ConnsArr);

		RoomsArr.Add(MakeShared<FJsonValueObject>(RObj));
	}
	Root->SetArrayField(TEXT("rooms"), RoomsArr);

	// Empty corridors and walls arrays — Claude fills these in
	Root->SetArrayField(TEXT("corridors"), TArray<TSharedPtr<FJsonValue>>());
	Root->SetArrayField(TEXT("walls"), TArray<TSharedPtr<FJsonValue>>());

	// Serialize
	FString OutputDir = FPaths::ProjectDir() / TEXT("MapCapture");
	IFileManager::Get().MakeDirectory(*OutputDir, true);
	FString OutputPath = OutputDir / FString::Printf(TEXT("dungeon_%02d_layout.json"), DungeonIndex + 1);

	FString JsonStr;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonStr);
	FJsonSerializer::Serialize(Root.ToSharedRef(), Writer);
	FFileHelper::SaveStringToFile(JsonStr, *OutputPath);

	UE_LOG(LogTemp, Warning, TEXT("[CaveMaze] Template layout: %d rooms -> %s"), RoomsArr.Num(), *OutputPath);
}

// ═══════════════════════════════════════════════════════════════════════════════
// Phase D: Overlay Visualization
// ═══════════════════════════════════════════════════════════════════════════════

void FSLFCaveMazeBuilder::GenerateOverlay(UWorld* World, int32 DungeonIndex)
{
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("[CaveMaze] GenerateOverlay: No world!"));
		return;
	}

	FSLFCaveLayout Layout;
	if (!LoadLayoutFromJson(DungeonIndex, Layout))
	{
		UE_LOG(LogTemp, Error, TEXT("[CaveMaze] GenerateOverlay: Failed to load layout JSON!"));
		return;
	}

	// Clear existing overlay actors first
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		if ((*It)->Tags.Contains(FName(TEXT("LayoutOverlay"))))
		{
			(*It)->Destroy();
		}
	}

	int32 MarkersSpawned = 0;

	// Room center markers (colored spheres)
	for (const FSLFRoomDesign& Room : Layout.Rooms)
	{
		FLinearColor Color = GetDesignationColor(Room.Designation);
		AActor* Marker = SpawnOverlayMarker(World, Room.Center + FVector(0, 0, 50), Color, Room.Radius * 0.3f, true);
		if (Marker)
		{
			Marker->Tags.Add(FName(TEXT("LayoutOverlay")));
			Marker->SetFolderPath(FName(TEXT("Dungeon/Overlay")));
			MarkersSpawned++;

			// Text label
			ATextRenderActor* TextActor = World->SpawnActor<ATextRenderActor>(
				Room.Center + FVector(0, 0, 200.0f), FRotator(-90.0f, 0.0f, 0.0f));
			if (TextActor)
			{
				TextActor->GetTextRender()->SetText(FText::FromString(
					FString::Printf(TEXT("G%d: %s"), Room.GroupId, *GetDesignationName(Room.Designation))));
				TextActor->GetTextRender()->SetWorldSize(80.0f);
				TextActor->GetTextRender()->SetTextRenderColor(FColor::White);
				TextActor->GetTextRender()->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
				TextActor->Tags.Add(FName(TEXT("LayoutOverlay")));
				TextActor->SetFolderPath(FName(TEXT("Dungeon/Overlay")));
				MarkersSpawned++;
			}
		}
	}

	// Wall segments (red translucent planes)
	for (const FSLFWallSegment& Wall : Layout.Walls)
	{
		FVector WallCenter = (Wall.Start + Wall.End) * 0.5f;
		float WallLength = FVector::Dist(Wall.Start, Wall.End);

		AActor* Marker = SpawnOverlayMarker(World, WallCenter + FVector(0, 0, 30), FLinearColor(1, 0, 0, 0.5f), WallLength * 0.5f, false);
		if (Marker)
		{
			// Rotate plane to match wall direction
			FVector Dir = (Wall.End - Wall.Start).GetSafeNormal();
			FRotator Rot = Dir.Rotation();
			Marker->SetActorRotation(Rot);
			Marker->Tags.Add(FName(TEXT("LayoutOverlay")));
			Marker->SetFolderPath(FName(TEXT("Dungeon/Overlay")));
			MarkersSpawned++;
		}
	}

	// Corridor center lines (green)
	for (const FSLFCorridorDesign& Corridor : Layout.Corridors)
	{
		// Find the room centers for start/end
		FVector StartPos = FVector::ZeroVector;
		FVector EndPos = FVector::ZeroVector;
		for (const FSLFRoomDesign& R : Layout.Rooms)
		{
			if (R.GroupId == Corridor.FromRoomId) StartPos = R.Center;
			if (R.GroupId == Corridor.ToRoomId) EndPos = R.Center;
		}

		// Draw waypoints as green markers
		TArray<FVector> Points;
		Points.Add(StartPos);
		Points.Append(Corridor.Waypoints);
		Points.Add(EndPos);

		FLinearColor CorridorColor = Corridor.bIsShortcut ? FLinearColor(1, 0, 1, 0.5f) : FLinearColor(0, 1, 0, 0.5f);
		for (int32 P = 0; P < Points.Num() - 1; P++)
		{
			FVector MidPt = (Points[P] + Points[P + 1]) * 0.5f;
			float Len = FVector::Dist(Points[P], Points[P + 1]);
			AActor* Marker = SpawnOverlayMarker(World, MidPt + FVector(0, 0, 20), CorridorColor, Len * 0.3f, false);
			if (Marker)
			{
				FVector Dir = (Points[P + 1] - Points[P]).GetSafeNormal();
				Marker->SetActorRotation(Dir.Rotation());
				Marker->Tags.Add(FName(TEXT("LayoutOverlay")));
				Marker->SetFolderPath(FName(TEXT("Dungeon/Overlay")));
				MarkersSpawned++;
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[CaveMaze] GenerateOverlay: %d markers spawned"), MarkersSpawned);
}

// ═══════════════════════════════════════════════════════════════════════════════
// Phase E: Wall Construction
// ═══════════════════════════════════════════════════════════════════════════════

void FSLFCaveMazeBuilder::BuildCaveMaze(UWorld* World, int32 DungeonIndex)
{
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("[CaveMaze] BuildCaveMaze: No world!"));
		return;
	}

	FSLFCaveLayout Layout;
	if (!LoadLayoutFromJson(DungeonIndex, Layout))
	{
		UE_LOG(LogTemp, Error, TEXT("[CaveMaze] BuildCaveMaze: Failed to load layout JSON!"));
		return;
	}

	FBox Bounds = ComputeDungeonBounds(World);

	int32 WallsBuilt = 0;
	for (const FSLFWallSegment& Wall : Layout.Walls)
	{
		// Find floor and ceiling at wall center
		FVector WallCenter = (Wall.Start + Wall.End) * 0.5f;

		// Use the wall's Z from the JSON as a reference point — rooms are at different heights
		float RefZ = WallCenter.Z;

		// Try line traces first (works in editor, may fail in commandlet)
		float FloorZ = TraceFloorHeight(World, WallCenter, RefZ + 500.0f, RefZ - 3000.0f);
		float CeilingZ = TraceCeilingHeight(World, WallCenter, RefZ - 500.0f, RefZ + 3000.0f);

		// Fallback: use wall Z as floor reference, add fixed height
		if (FloorZ == FLT_MIN) FloorZ = RefZ - 200.0f;
		if (CeilingZ == FLT_MAX) CeilingZ = RefZ + 1400.0f;

		// Allow height override
		if (Wall.HeightOverride > 0.0f)
		{
			CeilingZ = FloorZ + Wall.HeightOverride;
		}

		AActor* WallActor = SpawnWallSegment(World, Wall, FloorZ, CeilingZ);
		if (WallActor)
		{
			WallActor->Tags.Add(FName(TEXT("CaveMazeWall")));
			if (!Wall.Tag.IsEmpty())
			{
				WallActor->Tags.Add(FName(*Wall.Tag));
			}
			WallActor->SetFolderPath(FName(TEXT("Dungeon/MazeWalls")));
			WallsBuilt++;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[CaveMaze] BuildCaveMaze: %d walls built from %d segments"), WallsBuilt, Layout.Walls.Num());
}

// ═══════════════════════════════════════════════════════════════════════════════
// Phase E: Gameplay Placement
// ═══════════════════════════════════════════════════════════════════════════════

void FSLFCaveMazeBuilder::PlaceCaveGameplay(UWorld* World, int32 DungeonIndex)
{
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("[CaveMaze] PlaceCaveGameplay: No world!"));
		return;
	}

	FSLFCaveLayout Layout;
	if (!LoadLayoutFromJson(DungeonIndex, Layout))
	{
		UE_LOG(LogTemp, Error, TEXT("[CaveMaze] PlaceCaveGameplay: Failed to load layout JSON!"));
		return;
	}

	FRandomStream Rng(DungeonIndex * 7777 + 42);
	int32 ActorsPlaced = 0;

	for (const FSLFRoomDesign& Room : Layout.Rooms)
	{
		switch (Room.Designation)
		{
		case ESLFRoomDesignation::Entrance:
		{
			// PlayerStart
			APlayerStart* PS = World->SpawnActor<APlayerStart>(Room.Center + FVector(0, 0, 500.0f), FRotator::ZeroRotator);
			if (PS)
			{
				PS->SetFolderPath(FName(TEXT("Dungeon/Gameplay")));
				ActorsPlaced++;
			}

			// Warm ambient lights
			for (int32 L = 0; L < 4; L++)
			{
				float Angle = L * PI * 0.5f;
				FVector LightPos = Room.Center + FVector(FMath::Cos(Angle) * Room.Radius * 0.5f,
					FMath::Sin(Angle) * Room.Radius * 0.5f, 300.0f);
				APointLight* Light = World->SpawnActor<APointLight>(LightPos, FRotator::ZeroRotator);
				if (Light)
				{
					Light->PointLightComponent->SetIntensity(200000.0f);
					Light->PointLightComponent->SetAttenuationRadius(Room.Radius * 2.0f);
					Light->PointLightComponent->SetLightColor(FLinearColor(0.8f, 0.7f, 0.5f));
					Light->SetFolderPath(FName(TEXT("Dungeon/Lights")));
					Light->Tags.Add(FName(TEXT("CaveMazeWall"))); // So ClearCaveMaze removes them
					ActorsPlaced++;
				}
			}
			break;
		}

		case ESLFRoomDesignation::BossArena:
		{
			// Boss spawn marker
			AActor* BossMarker = World->SpawnActor(AActor::StaticClass(),
				&Room.Center, &FRotator::ZeroRotator);
			if (BossMarker)
			{
				BossMarker->Tags.Add(FName(TEXT("EnemySpawnPoint")));
				BossMarker->Tags.Add(FName(TEXT("BossEnemy")));
				BossMarker->Tags.Add(FName(TEXT("CaveMazeWall")));
				BossMarker->SetFolderPath(FName(TEXT("Dungeon/SpawnPoints")));
				ActorsPlaced++;
			}

			// Boss fog gate — placed at room edge toward entrance
			FVector EntranceDir = FVector::ZeroVector;
			for (const FSLFRoomDesign& Other : Layout.Rooms)
			{
				if (Other.Designation == ESLFRoomDesignation::Entrance)
				{
					EntranceDir = (Other.Center - Room.Center).GetSafeNormal();
					break;
				}
			}
			FVector GatePos = Room.Center + EntranceDir * Room.Radius * 0.8f;
			ASLFBossDoor* BossGate = World->SpawnActor<ASLFBossDoor>(GatePos, EntranceDir.Rotation());
			if (BossGate)
			{
				BossGate->bRequiresPuzzle = true;
				BossGate->DungeonTier = static_cast<ESLFDungeonTier>(FMath::Min(DungeonIndex, 2));
				BossGate->Tags.Add(FName(TEXT("CaveMazeWall")));
				BossGate->SetFolderPath(FName(TEXT("Dungeon/BossGates")));
				ActorsPlaced++;
			}

			// Ominous purple lights
			for (int32 L = 0; L < 5; L++)
			{
				float Angle = L * PI * 0.4f;
				FVector LightPos = Room.Center + FVector(FMath::Cos(Angle) * Room.Radius * 0.6f,
					FMath::Sin(Angle) * Room.Radius * 0.6f, 250.0f);
				APointLight* Light = World->SpawnActor<APointLight>(LightPos, FRotator::ZeroRotator);
				if (Light)
				{
					Light->PointLightComponent->SetIntensity(150000.0f);
					Light->PointLightComponent->SetAttenuationRadius(4000.0f);
					Light->PointLightComponent->SetLightColor(FLinearColor(0.4f, 0.2f, 0.6f));
					Light->PointLightComponent->SetCastShadows(true);
					Light->Tags.Add(FName(TEXT("CaveMazeWall")));
					Light->SetFolderPath(FName(TEXT("Dungeon/Lights")));
					ActorsPlaced++;
				}
			}
			break;
		}

		case ESLFRoomDesignation::Treasure:
		{
			// Treasure pickup marker at center
			FVector TreasurePos = Room.Center + FVector(0, 0, 50.0f);
			AActor* Treasure = World->SpawnActor(AActor::StaticClass(), &TreasurePos, &FRotator::ZeroRotator);
			if (Treasure)
			{
				Treasure->Tags.Add(FName(TEXT("TreasurePickup")));
				Treasure->Tags.Add(FName(TEXT("RareLoot")));
				Treasure->Tags.Add(FName(TEXT("CaveMazeWall")));
				Treasure->SetFolderPath(FName(TEXT("Dungeon/Treasure")));
				ActorsPlaced++;
			}

			// Warm gold light
			APointLight* Light = World->SpawnActor<APointLight>(Room.Center + FVector(0, 0, 250.0f), FRotator::ZeroRotator);
			if (Light)
			{
				Light->PointLightComponent->SetIntensity(120000.0f);
				Light->PointLightComponent->SetAttenuationRadius(3500.0f);
				Light->PointLightComponent->SetLightColor(FLinearColor(0.6f, 0.5f, 0.2f));
				Light->Tags.Add(FName(TEXT("CaveMazeWall")));
				Light->SetFolderPath(FName(TEXT("Dungeon/Lights")));
				ActorsPlaced++;
			}
			break;
		}

		case ESLFRoomDesignation::TrapRoom:
		{
			int32 TrapCount = 1 + Rng.RandRange(0, 1);
			for (int32 T = 0; T < TrapCount; T++)
			{
				float Angle = Rng.FRandRange(0.0f, 2.0f * PI);
				float Dist = Room.Radius * Rng.FRandRange(0.2f, 0.5f);
				FVector TrapPos = Room.Center + FVector(FMath::Cos(Angle) * Dist, FMath::Sin(Angle) * Dist, 50.0f);

				ASLFTrapBase* Trap = World->SpawnActor<ASLFTrapBase>(TrapPos, FRotator::ZeroRotator);
				if (Trap)
				{
					Trap->TrapType = static_cast<ESLFTrapType>(Rng.RandRange(0, 4));
					Trap->Damage = 30.0f + DungeonIndex * 15.0f;
					Trap->Tags.Add(FName(TEXT("CaveMazeWall")));
					Trap->SetFolderPath(FName(TEXT("Dungeon/Traps")));
					ActorsPlaced++;
				}
			}

			// Red-orange warning light
			APointLight* Light = World->SpawnActor<APointLight>(Room.Center + FVector(0, 0, 200.0f), FRotator::ZeroRotator);
			if (Light)
			{
				Light->PointLightComponent->SetIntensity(80000.0f);
				Light->PointLightComponent->SetAttenuationRadius(2500.0f);
				Light->PointLightComponent->SetLightColor(FLinearColor(0.8f, 0.3f, 0.1f));
				Light->Tags.Add(FName(TEXT("CaveMazeWall")));
				Light->SetFolderPath(FName(TEXT("Dungeon/Lights")));
				ActorsPlaced++;
			}
			break;
		}

		case ESLFRoomDesignation::AmbushRoom:
		{
			int32 EnemyCount = Rng.RandRange(2, 3);
			for (int32 E = 0; E < EnemyCount; E++)
			{
				float Angle = Rng.FRandRange(0.0f, 2.0f * PI);
				float Dist = Room.Radius * Rng.FRandRange(0.4f, 0.7f);
				FVector SpawnPos = Room.Center + FVector(FMath::Cos(Angle) * Dist, FMath::Sin(Angle) * Dist, 90.0f);

				AActor* Marker = World->SpawnActor(AActor::StaticClass(), &SpawnPos, &FRotator::ZeroRotator);
				if (Marker)
				{
					Marker->Tags.Add(FName(TEXT("EnemySpawnPoint")));
					Marker->Tags.Add(FName(TEXT("CaveMazeWall")));
					Marker->SetFolderPath(FName(TEXT("Dungeon/SpawnPoints")));
					ActorsPlaced++;
				}
			}

			// Dim blue light
			APointLight* Light = World->SpawnActor<APointLight>(Room.Center + FVector(0, 0, 200.0f), FRotator::ZeroRotator);
			if (Light)
			{
				Light->PointLightComponent->SetIntensity(40000.0f);
				Light->PointLightComponent->SetAttenuationRadius(2000.0f);
				Light->PointLightComponent->SetLightColor(FLinearColor(0.2f, 0.3f, 0.5f));
				Light->Tags.Add(FName(TEXT("CaveMazeWall")));
				Light->SetFolderPath(FName(TEXT("Dungeon/Lights")));
				ActorsPlaced++;
			}
			break;
		}

		case ESLFRoomDesignation::PuzzleRoom:
		{
			ASLFPuzzleMarker* Lever = World->SpawnActor<ASLFPuzzleMarker>(Room.Center, FRotator::ZeroRotator);
			if (Lever)
			{
				Lever->PuzzleType = static_cast<ESLFPuzzleType>(0);
				Lever->RequiredObjectives = 1;
				Lever->Tags.Add(FName(TEXT("CaveMazeWall")));
				Lever->SetFolderPath(FName(TEXT("Dungeon/Puzzles")));
				ActorsPlaced++;
			}

			// Gold highlight light
			APointLight* Light = World->SpawnActor<APointLight>(Room.Center + FVector(0, 0, 250.0f), FRotator::ZeroRotator);
			if (Light)
			{
				Light->PointLightComponent->SetIntensity(100000.0f);
				Light->PointLightComponent->SetAttenuationRadius(3000.0f);
				Light->PointLightComponent->SetLightColor(FLinearColor(0.7f, 0.6f, 0.3f));
				Light->Tags.Add(FName(TEXT("CaveMazeWall")));
				Light->SetFolderPath(FName(TEXT("Dungeon/Lights")));
				ActorsPlaced++;
			}
			break;
		}

		case ESLFRoomDesignation::ShortcutHub:
		{
			// Find the entrance room for shortcut direction
			FVector EntranceCenter = FVector::ZeroVector;
			for (const FSLFRoomDesign& Other : Layout.Rooms)
			{
				if (Other.Designation == ESLFRoomDesignation::Entrance)
				{
					EntranceCenter = Other.Center;
					break;
				}
			}
			FVector GateDir = (EntranceCenter - Room.Center).GetSafeNormal();

			ASLFShortcutGate* Gate = World->SpawnActor<ASLFShortcutGate>(Room.Center, GateDir.Rotation());
			if (Gate)
			{
				Gate->Tags.Add(FName(TEXT("CaveMazeWall")));
				Gate->SetFolderPath(FName(TEXT("Dungeon/Shortcuts")));
				ActorsPlaced++;
			}

			// Teal light
			APointLight* Light = World->SpawnActor<APointLight>(Room.Center + FVector(0, 0, 200.0f), FRotator::ZeroRotator);
			if (Light)
			{
				Light->PointLightComponent->SetIntensity(80000.0f);
				Light->PointLightComponent->SetAttenuationRadius(2500.0f);
				Light->PointLightComponent->SetLightColor(FLinearColor(0.2f, 0.6f, 0.5f));
				Light->Tags.Add(FName(TEXT("CaveMazeWall")));
				Light->SetFolderPath(FName(TEXT("Dungeon/Lights")));
				ActorsPlaced++;
			}
			break;
		}

		case ESLFRoomDesignation::HiddenRoom:
		{
			// Secret treasure — higher quality
			FVector TreasurePos = Room.Center + FVector(0, 0, 50.0f);
			AActor* Treasure = World->SpawnActor(AActor::StaticClass(), &TreasurePos, &FRotator::ZeroRotator);
			if (Treasure)
			{
				Treasure->Tags.Add(FName(TEXT("TreasurePickup")));
				Treasure->Tags.Add(FName(TEXT("LegendaryLoot")));
				Treasure->Tags.Add(FName(TEXT("CaveMazeWall")));
				Treasure->SetFolderPath(FName(TEXT("Dungeon/Treasure")));
				ActorsPlaced++;
			}

			// Ethereal blue-white light
			APointLight* Light = World->SpawnActor<APointLight>(Room.Center + FVector(0, 0, 200.0f), FRotator::ZeroRotator);
			if (Light)
			{
				Light->PointLightComponent->SetIntensity(60000.0f);
				Light->PointLightComponent->SetAttenuationRadius(2000.0f);
				Light->PointLightComponent->SetLightColor(FLinearColor(0.5f, 0.6f, 0.9f));
				Light->Tags.Add(FName(TEXT("CaveMazeWall")));
				Light->SetFolderPath(FName(TEXT("Dungeon/Lights")));
				ActorsPlaced++;
			}
			break;
		}

		default: // Corridor, Junction — just a basic light
		{
			APointLight* Light = World->SpawnActor<APointLight>(Room.Center + FVector(0, 0, 200.0f), FRotator::ZeroRotator);
			if (Light)
			{
				Light->PointLightComponent->SetIntensity(40000.0f);
				Light->PointLightComponent->SetAttenuationRadius(2000.0f);
				Light->PointLightComponent->SetLightColor(FLinearColor(0.3f, 0.4f, 0.5f));
				Light->Tags.Add(FName(TEXT("CaveMazeWall")));
				Light->SetFolderPath(FName(TEXT("Dungeon/Lights")));
				ActorsPlaced++;
			}
			break;
		}
		}
	}

	// Dungeon-wide exponential height fog
	{
		FVector FogPos = Layout.DungeonOrigin + FVector(0, 0, -200);
		AExponentialHeightFog* Fog = World->SpawnActor<AExponentialHeightFog>(FogPos, FRotator::ZeroRotator);
		if (Fog)
		{
			Fog->GetComponent()->SetFogDensity(0.025f);
			Fog->GetComponent()->SetFogHeightFalloff(0.12f);
			Fog->GetComponent()->SetFogInscatteringColor(FLinearColor(0.08f, 0.12f, 0.18f));
			Fog->GetComponent()->SetStartDistance(100.0f);
			Fog->Tags.Add(FName(TEXT("CaveMazeWall")));
			Fog->SetFolderPath(FName(TEXT("Dungeon/Atmosphere")));
			ActorsPlaced++;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[CaveMaze] PlaceCaveGameplay: %d actors placed"), ActorsPlaced);
}

// ═══════════════════════════════════════════════════════════════════════════════
// Cleanup
// ═══════════════════════════════════════════════════════════════════════════════

void FSLFCaveMazeBuilder::ClearCaveMaze(UWorld* World, int32 DungeonIndex)
{
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("[CaveMaze] ClearCaveMaze: No world!"));
		return;
	}

	int32 Destroyed = 0;
	TArray<AActor*> ToDestroy;

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (Actor->Tags.Contains(FName(TEXT("CaveMazeWall"))) || Actor->Tags.Contains(FName(TEXT("LayoutOverlay"))))
		{
			ToDestroy.Add(Actor);
		}
	}

	for (AActor* Actor : ToDestroy)
	{
		Actor->Destroy();
		Destroyed++;
	}

	UE_LOG(LogTemp, Warning, TEXT("[CaveMaze] ClearCaveMaze: Destroyed %d actors"), Destroyed);
}

// ═══════════════════════════════════════════════════════════════════════════════
// Private Helpers
// ═══════════════════════════════════════════════════════════════════════════════

bool FSLFCaveMazeBuilder::LoadLayoutFromJson(int32 DungeonIndex, FSLFCaveLayout& OutLayout)
{
	FString InputPath = FPaths::ProjectDir() / TEXT("MapCapture") /
		FString::Printf(TEXT("dungeon_%02d_layout.json"), DungeonIndex + 1);

	FString JsonStr;
	if (!FFileHelper::LoadFileToString(JsonStr, *InputPath))
	{
		UE_LOG(LogTemp, Error, TEXT("[CaveMaze] Failed to load: %s"), *InputPath);
		return false;
	}

	TSharedPtr<FJsonObject> Root;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonStr);
	if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[CaveMaze] Failed to parse JSON: %s"), *InputPath);
		return false;
	}

	OutLayout.DungeonIndex = Root->GetIntegerField(TEXT("dungeonIndex"));

	// Origin
	const TArray<TSharedPtr<FJsonValue>>* OriginArr;
	if (Root->TryGetArrayField(TEXT("origin"), OriginArr) && OriginArr->Num() >= 3)
	{
		OutLayout.DungeonOrigin = FVector(
			(*OriginArr)[0]->AsNumber(), (*OriginArr)[1]->AsNumber(), (*OriginArr)[2]->AsNumber());
	}

	// Extent
	const TArray<TSharedPtr<FJsonValue>>* ExtArr;
	if (Root->TryGetArrayField(TEXT("extent"), ExtArr) && ExtArr->Num() >= 3)
	{
		OutLayout.DungeonExtent = FVector(
			(*ExtArr)[0]->AsNumber(), (*ExtArr)[1]->AsNumber(), (*ExtArr)[2]->AsNumber());
	}

	// Rooms
	const TArray<TSharedPtr<FJsonValue>>* RoomsArr;
	if (Root->TryGetArrayField(TEXT("rooms"), RoomsArr))
	{
		for (const TSharedPtr<FJsonValue>& RVal : *RoomsArr)
		{
			TSharedPtr<FJsonObject> RObj = RVal->AsObject();
			if (!RObj) continue;

			FSLFRoomDesign Room;
			Room.GroupId = RObj->GetIntegerField(TEXT("groupId"));
			Room.Designation = ParseDesignation(RObj->GetStringField(TEXT("designation")));
			Room.Radius = RObj->GetNumberField(TEXT("radius"));

			const TArray<TSharedPtr<FJsonValue>>* CArr;
			if (RObj->TryGetArrayField(TEXT("center"), CArr) && CArr->Num() >= 3)
			{
				Room.Center = FVector((*CArr)[0]->AsNumber(), (*CArr)[1]->AsNumber(), (*CArr)[2]->AsNumber());
			}

			const TArray<TSharedPtr<FJsonValue>>* ConArr;
			if (RObj->TryGetArrayField(TEXT("connectedRoomIds"), ConArr))
			{
				for (const TSharedPtr<FJsonValue>& CV : *ConArr)
				{
					Room.ConnectedRoomIds.Add((int32)CV->AsNumber());
				}
			}

			OutLayout.Rooms.Add(Room);
		}
	}

	// Corridors
	const TArray<TSharedPtr<FJsonValue>>* CorrArr;
	if (Root->TryGetArrayField(TEXT("corridors"), CorrArr))
	{
		for (const TSharedPtr<FJsonValue>& CVal : *CorrArr)
		{
			TSharedPtr<FJsonObject> CObj = CVal->AsObject();
			if (!CObj) continue;

			FSLFCorridorDesign Corridor;
			Corridor.FromRoomId = CObj->GetIntegerField(TEXT("from"));
			Corridor.ToRoomId = CObj->GetIntegerField(TEXT("to"));
			Corridor.Width = CObj->GetNumberField(TEXT("width"));
			CObj->TryGetBoolField(TEXT("isShortcut"), Corridor.bIsShortcut);

			const TArray<TSharedPtr<FJsonValue>>* WPArr;
			if (CObj->TryGetArrayField(TEXT("waypoints"), WPArr))
			{
				for (const TSharedPtr<FJsonValue>& WP : *WPArr)
				{
					const TArray<TSharedPtr<FJsonValue>>& WPCoords = WP->AsArray();
					if (WPCoords.Num() >= 3)
					{
						Corridor.Waypoints.Add(FVector(
							WPCoords[0]->AsNumber(), WPCoords[1]->AsNumber(), WPCoords[2]->AsNumber()));
					}
				}
			}

			OutLayout.Corridors.Add(Corridor);
		}
	}

	// Walls
	const TArray<TSharedPtr<FJsonValue>>* WallsArr;
	if (Root->TryGetArrayField(TEXT("walls"), WallsArr))
	{
		for (const TSharedPtr<FJsonValue>& WVal : *WallsArr)
		{
			TSharedPtr<FJsonObject> WObj = WVal->AsObject();
			if (!WObj) continue;

			FSLFWallSegment Wall;

			const TArray<TSharedPtr<FJsonValue>>* SArr;
			if (WObj->TryGetArrayField(TEXT("start"), SArr) && SArr->Num() >= 3)
			{
				Wall.Start = FVector((*SArr)[0]->AsNumber(), (*SArr)[1]->AsNumber(), (*SArr)[2]->AsNumber());
			}

			const TArray<TSharedPtr<FJsonValue>>* EArr;
			if (WObj->TryGetArrayField(TEXT("end"), EArr) && EArr->Num() >= 3)
			{
				Wall.End = FVector((*EArr)[0]->AsNumber(), (*EArr)[1]->AsNumber(), (*EArr)[2]->AsNumber());
			}

			Wall.Thickness = WObj->GetNumberField(TEXT("thickness"));
			WObj->TryGetStringField(TEXT("tag"), Wall.Tag);
			WObj->TryGetBoolField(TEXT("naturalRock"), Wall.bNaturalRock);

			double HeightOvr = 0;
			if (WObj->TryGetNumberField(TEXT("heightOverride"), HeightOvr))
			{
				Wall.HeightOverride = (float)HeightOvr;
			}

			OutLayout.Walls.Add(Wall);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[CaveMaze] Loaded layout: %d rooms, %d corridors, %d walls from %s"),
		OutLayout.Rooms.Num(), OutLayout.Corridors.Num(), OutLayout.Walls.Num(), *InputPath);
	return true;
}

ADungeon* FSLFCaveMazeBuilder::FindDungeonActor(UWorld* World)
{
	for (TActorIterator<ADungeon> It(World); It; ++It)
	{
		return *It;
	}
	return nullptr;
}

FBox FSLFCaveMazeBuilder::ComputeDungeonBounds(UWorld* World)
{
	FBox Bounds(ForceInit);
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor || Actor->IsA(AWorldSettings::StaticClass())) continue;

		FVector Origin, Extent;
		Actor->GetActorBounds(false, Origin, Extent);
		if (Extent.GetMax() > 0.1f)
		{
			Bounds += FBox(Origin - Extent, Origin + Extent);
		}
	}
	return Bounds;
}

float FSLFCaveMazeBuilder::TraceFloorHeight(UWorld* World, FVector Position, float StartZ, float EndZ)
{
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.bTraceComplex = true;

	FVector TraceStart(Position.X, Position.Y, StartZ);
	FVector TraceEnd(Position.X, Position.Y, EndZ);

	if (World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldStatic, Params))
	{
		return Hit.ImpactPoint.Z;
	}
	return FLT_MIN;
}

float FSLFCaveMazeBuilder::TraceCeilingHeight(UWorld* World, FVector Position, float StartZ, float EndZ)
{
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.bTraceComplex = true;

	FVector TraceStart(Position.X, Position.Y, StartZ);
	FVector TraceEnd(Position.X, Position.Y, EndZ);

	if (World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldStatic, Params))
	{
		return Hit.ImpactPoint.Z;
	}
	return FLT_MAX;
}

AActor* FSLFCaveMazeBuilder::SpawnWallSegment(UWorld* World, const FSLFWallSegment& Wall, float FloorZ, float CeilingZ)
{
	// Load cube mesh
	UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (!CubeMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("[CaveMaze] SpawnWallSegment: Failed to load Cube mesh!"));
		return nullptr;
	}

	// Load rock material
	UMaterialInterface* WallMat = nullptr;
	if (Wall.bNaturalRock)
	{
		WallMat = LoadObject<UMaterialInterface>(nullptr,
			TEXT("/Game/Wasteland/Materials/Material_Instances/MI_Rock_03.MI_Rock_03"));
		if (!WallMat)
		{
			WallMat = LoadObject<UMaterialInterface>(nullptr,
				TEXT("/Game/WastelandEnvironment/Materials/Material_Instances/MI_Rock_01.MI_Rock_01"));
		}
	}

	FVector WallCenter = (Wall.Start + Wall.End) * 0.5f;
	float WallLength = FVector::Dist(Wall.Start, Wall.End);
	float WallHeight = CeilingZ - FloorZ;
	FVector Dir = (Wall.End - Wall.Start).GetSafeNormal();
	FRotator Rot = Dir.Rotation();

	// Place at midpoint between floor and ceiling
	FVector SpawnPos(WallCenter.X, WallCenter.Y, FloorZ + WallHeight * 0.5f);

	AStaticMeshActor* WallActor = World->SpawnActor<AStaticMeshActor>(SpawnPos, Rot);
	if (!WallActor) return nullptr;

	UStaticMeshComponent* MeshComp = WallActor->GetStaticMeshComponent();
	MeshComp->SetStaticMesh(CubeMesh);

	if (WallMat)
	{
		MeshComp->SetMaterial(0, WallMat);
	}

	// Cube is 100x100x100, scale to match wall dimensions
	// X = along wall direction (length), Y = perpendicular (thickness), Z = height
	FVector Scale(
		WallLength / 100.0f,
		Wall.Thickness / 100.0f,
		WallHeight / 100.0f
	);
	WallActor->SetActorScale3D(Scale);

	// Collision
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComp->SetCollisionProfileName(TEXT("BlockAll"));

	return WallActor;
}

AActor* FSLFCaveMazeBuilder::SpawnOverlayMarker(UWorld* World, FVector Position, FLinearColor Color, float Size, bool bIsSphere)
{
	UStaticMesh* Mesh = nullptr;
	if (bIsSphere)
	{
		Mesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	}
	else
	{
		Mesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Plane.Plane"));
	}

	if (!Mesh)
	{
		UE_LOG(LogTemp, Error, TEXT("[CaveMaze] SpawnOverlayMarker: Failed to load mesh!"));
		return nullptr;
	}

	AStaticMeshActor* Actor = World->SpawnActor<AStaticMeshActor>(Position, FRotator::ZeroRotator);
	if (!Actor) return nullptr;

	UStaticMeshComponent* MeshComp = Actor->GetStaticMeshComponent();
	MeshComp->SetStaticMesh(Mesh);

	// Scale: Sphere and Plane are 100 units, scale to match desired size
	float ScaleFactor = Size / 100.0f;
	if (bIsSphere)
	{
		Actor->SetActorScale3D(FVector(ScaleFactor));
	}
	else
	{
		Actor->SetActorScale3D(FVector(ScaleFactor, ScaleFactor * 0.2f, 1.0f));
	}

	// Create colored dynamic material
	UMaterialInterface* BaseMat = LoadObject<UMaterialInterface>(nullptr,
		TEXT("/Engine/EngineMaterials/DefaultMaterial.DefaultMaterial"));
	if (BaseMat)
	{
		UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(BaseMat, Actor);
		DynMat->SetVectorParameterValue(TEXT("BaseColor"), Color);
		MeshComp->SetMaterial(0, DynMat);
	}

	// No collision on overlay markers
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	return Actor;
}

FLinearColor FSLFCaveMazeBuilder::GetDesignationColor(ESLFRoomDesignation Designation)
{
	switch (Designation)
	{
	case ESLFRoomDesignation::Entrance:     return FLinearColor(0.0f, 1.0f, 0.0f);   // Green
	case ESLFRoomDesignation::BossArena:    return FLinearColor(1.0f, 0.0f, 0.0f);   // Red
	case ESLFRoomDesignation::Treasure:     return FLinearColor(1.0f, 0.84f, 0.0f);  // Gold
	case ESLFRoomDesignation::TrapRoom:     return FLinearColor(1.0f, 0.5f, 0.0f);   // Orange
	case ESLFRoomDesignation::AmbushRoom:   return FLinearColor(0.8f, 0.2f, 0.2f);   // Dark red
	case ESLFRoomDesignation::PuzzleRoom:   return FLinearColor(0.0f, 0.5f, 1.0f);   // Blue
	case ESLFRoomDesignation::ShortcutHub:  return FLinearColor(0.5f, 0.0f, 1.0f);   // Purple
	case ESLFRoomDesignation::Corridor:     return FLinearColor(0.5f, 0.5f, 0.5f);   // Gray
	case ESLFRoomDesignation::Junction:     return FLinearColor(0.7f, 0.7f, 0.7f);   // Light gray
	case ESLFRoomDesignation::HiddenRoom:   return FLinearColor(0.0f, 1.0f, 1.0f);   // Cyan
	default:                                return FLinearColor(1.0f, 1.0f, 1.0f);
	}
}

FString FSLFCaveMazeBuilder::GetDesignationName(ESLFRoomDesignation Designation)
{
	switch (Designation)
	{
	case ESLFRoomDesignation::Entrance:     return TEXT("Entrance");
	case ESLFRoomDesignation::BossArena:    return TEXT("BossArena");
	case ESLFRoomDesignation::Treasure:     return TEXT("Treasure");
	case ESLFRoomDesignation::TrapRoom:     return TEXT("TrapRoom");
	case ESLFRoomDesignation::AmbushRoom:   return TEXT("AmbushRoom");
	case ESLFRoomDesignation::PuzzleRoom:   return TEXT("PuzzleRoom");
	case ESLFRoomDesignation::ShortcutHub:  return TEXT("ShortcutHub");
	case ESLFRoomDesignation::Corridor:     return TEXT("Corridor");
	case ESLFRoomDesignation::Junction:     return TEXT("Junction");
	case ESLFRoomDesignation::HiddenRoom:   return TEXT("HiddenRoom");
	default:                                return TEXT("Unknown");
	}
}

ESLFRoomDesignation FSLFCaveMazeBuilder::ParseDesignation(const FString& Name)
{
	if (Name.Equals(TEXT("Entrance"), ESearchCase::IgnoreCase))     return ESLFRoomDesignation::Entrance;
	if (Name.Equals(TEXT("BossArena"), ESearchCase::IgnoreCase))    return ESLFRoomDesignation::BossArena;
	if (Name.Equals(TEXT("Treasure"), ESearchCase::IgnoreCase))     return ESLFRoomDesignation::Treasure;
	if (Name.Equals(TEXT("TrapRoom"), ESearchCase::IgnoreCase))     return ESLFRoomDesignation::TrapRoom;
	if (Name.Equals(TEXT("AmbushRoom"), ESearchCase::IgnoreCase))   return ESLFRoomDesignation::AmbushRoom;
	if (Name.Equals(TEXT("PuzzleRoom"), ESearchCase::IgnoreCase))   return ESLFRoomDesignation::PuzzleRoom;
	if (Name.Equals(TEXT("ShortcutHub"), ESearchCase::IgnoreCase))  return ESLFRoomDesignation::ShortcutHub;
	if (Name.Equals(TEXT("Corridor"), ESearchCase::IgnoreCase))     return ESLFRoomDesignation::Corridor;
	if (Name.Equals(TEXT("Junction"), ESearchCase::IgnoreCase))     return ESLFRoomDesignation::Junction;
	if (Name.Equals(TEXT("HiddenRoom"), ESearchCase::IgnoreCase))   return ESLFRoomDesignation::HiddenRoom;
	return ESLFRoomDesignation::Corridor;
}

#endif // WITH_EDITOR
