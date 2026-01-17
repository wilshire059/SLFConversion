// B_Ladder.h
// C++ class for Blueprint B_Ladder
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/B_Ladder.json
// Parent: B_Interactable_C -> AB_Interactable
// Variables: 16 | Functions: 2 | Components: 7

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/B_Interactable.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "B_Ladder.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API AB_Ladder : public AB_Interactable
{
	GENERATED_BODY()

public:
	AB_Ladder();

	// ═══════════════════════════════════════════════════════════════════════
	// COMPONENTS (7) - from JSON Components.Hierarchy
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UInstancedStaticMeshComponent* Barz;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UInstancedStaticMeshComponent* Pole;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UInstancedStaticMeshComponent* Connections;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* ClimbingCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* BottomCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* TopCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* TopdownCollision;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (16) - from JSON Variables.List
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	double LadderHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	double BarOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	int32 OvershootCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	int32 ConnectionCount;

	// Note: Blueprint uses "BarAtZero?" - C++ uses bBarAtZero (UE naming convention)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (DisplayName = "BarAtZero?"))
	bool bBarAtZero;

	// Note: Blueprint uses "AddFinishBar?" - C++ uses bAddFinishBar
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (DisplayName = "AddFinishBar?"))
	bool bAddFinishBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Collision (Adjust to Your Needs)")
	FVector TopCollisionExtent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Collision (Adjust to Your Needs)")
	double TopCollisionOffsetX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Collision (Adjust to Your Needs)")
	double TopCollisionOffsetZ;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Collision (Adjust to Your Needs)")
	FVector BottomCollisionExtent;

	// Private variable in Blueprint (IsPrivate: true)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 Cache_LastBarIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Collision (Adjust to Your Needs)")
	double BottomCollisionOffsetX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Collision (Adjust to Your Needs)")
	double BottomCollisionOffsetZ;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Collision (Adjust to Your Needs)")
	FVector TopdownCollisionExtent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Collision (Adjust to Your Needs)")
	double TopdownCollisionOffsetX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Collision (Adjust to Your Needs)")
	double TopdownCollisionOffsetZ;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (2) - from JSON FunctionSignatures.Functions
	// ═══════════════════════════════════════════════════════════════════════

	// Returns the calculated pole height based on LadderHeight, BarOffset, OvershootCount
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Ladder")
	double GetPoleHeight();
	virtual double GetPoleHeight_Implementation();

	// Creates the ladder geometry: bars, poles, connections, and collision boxes
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Ladder")
	void CreateLadder();
	virtual void CreateLadder_Implementation();

	// ═══════════════════════════════════════════════════════════════════════
	// INTERFACE OVERRIDES
	// ═══════════════════════════════════════════════════════════════════════

	/** Override OnInteract to call TryClimbLadder on the interacting actor */
	virtual void OnInteract_Implementation(AActor* InteractingActor) override;

protected:
	// Called when construction script runs
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

	// ═══════════════════════════════════════════════════════════════════════
	// OVERLAP EVENT HANDLERS - Controls ladder climbing state
	// ═══════════════════════════════════════════════════════════════════════

	/** Called when something overlaps the climbing collision zone */
	UFUNCTION()
	void OnClimbingCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** Called when something begins overlapping the bottom zone */
	UFUNCTION()
	void OnBottomCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Called when something ends overlapping the bottom zone */
	UFUNCTION()
	void OnBottomCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** Called when something begins overlapping the top zone */
	UFUNCTION()
	void OnTopCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Called when something ends overlapping the top zone */
	UFUNCTION()
	void OnTopCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** Called when something ends overlapping the topdown zone */
	UFUNCTION()
	void OnTopdownCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
