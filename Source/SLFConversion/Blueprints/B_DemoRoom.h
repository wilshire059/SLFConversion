// B_DemoRoom.h
// C++ class for Blueprint B_DemoRoom
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_DemoRoom.json
// Parent: Actor -> AActor
// Variables: 39 | Functions: 13 | Dispatchers: 0

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "InputMappingContext.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "SkeletalMergingLibrary.h"
#include "GeometryCollection/GeometryCollectionObject.h"
#include "Field/FieldSystemObjects.h"
#include "B_DemoRoom.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;


// Event Dispatchers


UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API AB_DemoRoom : public AActor
{
	GENERATED_BODY()

public:
	AB_DemoRoom();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (39)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* _10MWall;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* _10MCurve;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* _10MPillar;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* PillarCurve;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* _10MWall_Black;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* _10MPillar_Black;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* _10MEdge_White;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* _10MWall_Colour;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* _10MEdge_Colour;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* Floor_Corner;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* Floor_Edge;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* UELogo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	TArray<FSLFRoomSettings> Rooms;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	double SegmentCounter;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* BackWall_Corner;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* BackWall_Edge;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* FloorFilter;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* _10MWall_Grey;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* _10MWall_Ceiling;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* BlackRibbedWall;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* BlackRibbedCurve;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	double SpacerSize;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* WhiteRibbedWall;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* Pipe;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* Trim_End;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* Trim_Straight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* Trim_Curve;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* Trim;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* LightPanel;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* PillarCurveCorner;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* MetalTrim_Corner;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* MetalTrim_Straight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	bool DrawEndWall;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	bool DrawStartWall;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* _10MPillar2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* PillarCurveCornerBlack;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* _10MPillar2Black;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* _10MEdgeBlack;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* isikdevLogo;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (13)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_DemoRoom")
	void CreateRoom(const FSLFRoomSettings& RoomParams, double InSegmentCounter);
	virtual void CreateRoom_Implementation(const FSLFRoomSettings& RoomParams, double InSegmentCounter);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_DemoRoom")
	void Floor();
	virtual void Floor_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_DemoRoom")
	void CreateInstances();
	virtual void CreateInstances_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_DemoRoom")
	void EndWall(const FTransform& WallPosition, int32 Width, int32 Height);
	virtual void EndWall_Implementation(const FTransform& WallPosition, int32 Width, int32 Height);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_DemoRoom")
	void MakePillar(const FTransform& BasePosition, int32 Width, int32 Height, double PillarThickness, bool SquareExterior, bool White);
	virtual void MakePillar_Implementation(const FTransform& BasePosition, int32 Width, int32 Height, double PillarThickness, bool SquareExterior, bool White);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_DemoRoom")
	void MakeDoor(int32 RoomWidth, int32 RoomHeight, double Offset);
	virtual void MakeDoor_Implementation(int32 RoomWidth, int32 RoomHeight, double Offset);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_DemoRoom")
	void MakeRoundedEnd(const FTransform& BaseTransform, int32 RoomWidth, double FloorSegment, double InnerPanelPercent, double OuterPanelPercent);
	virtual void MakeRoundedEnd_Implementation(const FTransform& BaseTransform, int32 RoomWidth, double FloorSegment, double InnerPanelPercent, double OuterPanelPercent);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_DemoRoom")
	void MakeBackWall(const FTransform& BaseTransform, int32 RoomWidth, int32 RoomHeight, double FloorSegment, double InnerPanelPercent, double OuterPanelPercent);
	virtual void MakeBackWall_Implementation(const FTransform& BaseTransform, int32 RoomWidth, int32 RoomHeight, double FloorSegment, double InnerPanelPercent, double OuterPanelPercent);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_DemoRoom")
	void AddSpacer(double Offset);
	virtual void AddSpacer_Implementation(double Offset);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_DemoRoom")
	void MetalStrip(const FTransform& BaseTransform);
	virtual void MetalStrip_Implementation(const FTransform& BaseTransform);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_DemoRoom")
	void Lighting(const FSLFRoomSettings& RoomParams);
	virtual void Lighting_Implementation(const FSLFRoomSettings& RoomParams);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_DemoRoom")
	void Redraw();
	virtual void Redraw_Implementation();
};
