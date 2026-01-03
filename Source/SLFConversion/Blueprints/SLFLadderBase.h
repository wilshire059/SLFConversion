// SLFLadderBase.h
// C++ base class for B_Ladder
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - B_Ladder
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         16/16 migrated
// Functions:         2/2 migrated (excluding UserConstructionScript and EventGraph)
// Event Dispatchers: 0/0
// Graphs:            4 (logic in functions)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Ladder
//
// PURPOSE: Ladder actor - climbable with procedural bar generation
// PARENT: B_Interactable

#pragma once

#include "CoreMinimal.h"
#include "SLFInteractableBase.h"
#include "Components/SceneComponent.h"
#include "SLFLadderBase.generated.h"

/**
 * Ladder actor - climbable with procedural bar generation
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFLadderBase : public ASLFInteractableBase
{
	GENERATED_BODY()

public:
	/** Default scene root for Blueprint components */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> DefaultSceneRoot;

	ASLFLadderBase();

protected:
	virtual void BeginPlay() override;

public:
	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 16/16 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Basic Config (6) ---

	/** [1/16] Total height of the ladder */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ladder|Config")
	double LadderHeight;

	/** [2/16] Offset between ladder bars */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ladder|Config")
	double BarOffset;

	/** [3/16] Number of overshoot bars at top */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ladder|Config")
	int32 OvershootCount;

	/** [4/16] Number of connection points */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ladder|Config")
	int32 ConnectionCount;

	/** [5/16] Whether to include a bar at Z=0 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ladder|Config")
	bool bBarAtZero;

	/** [6/16] Whether to add a finishing bar at top */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ladder|Config")
	bool bAddFinishBar;

	// --- Top Collision Settings (3) ---

	/** [7/16] Collision extent for top collision box */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ladder|Collision")
	FVector TopCollisionExtent;

	/** [8/16] X offset for top collision */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ladder|Collision")
	double TopCollisionOffsetX;

	/** [9/16] Z offset for top collision */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ladder|Collision")
	double TopCollisionOffsetZ;

	// --- Bottom Collision Settings (3) ---

	/** [10/16] Collision extent for bottom collision box */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ladder|Collision")
	FVector BottomCollisionExtent;

	/** [11/16] X offset for bottom collision */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ladder|Collision")
	double BottomCollisionOffsetX;

	/** [12/16] Z offset for bottom collision */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ladder|Collision")
	double BottomCollisionOffsetZ;

	// --- Topdown Collision Settings (3) ---

	/** [13/16] Collision extent for topdown collision box */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ladder|Collision")
	FVector TopdownCollisionExtent;

	/** [14/16] X offset for topdown collision */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ladder|Collision")
	double TopdownCollisionOffsetX;

	/** [15/16] Z offset for topdown collision */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ladder|Collision")
	double TopdownCollisionOffsetZ;

	// --- Runtime (1) ---

	/** [16/16] Cached last bar index */
	UPROPERTY(BlueprintReadWrite, Category = "Ladder|Runtime")
	int32 Cache_LastBarIndex;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 2/2 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/2] Get the calculated pole height
	 * @return Height of the ladder pole
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ladder")
	double GetPoleHeight();
	virtual double GetPoleHeight_Implementation();

	/** [2/2] Create the ladder mesh and components
	 * Called from construction script to generate procedural ladder
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ladder")
	void CreateLadder();
	virtual void CreateLadder_Implementation();
};
