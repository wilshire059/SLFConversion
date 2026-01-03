// CollisionManagerComponent.h
// C++ base class for AC_CollisionManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - AC_CollisionManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         11/11 migrated
// Functions:         9/9 migrated (excluding ExecuteUbergraph)
// Event Dispatchers: 1/1 migrated (OnActorTraced)
// Graphs:            7 (logic in functions)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/Components/AC_CollisionManager
//
// PURPOSE: Weapon collision tracing for melee combat - sphere traces between sockets

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CollisionManagerComponent.generated.h"

// ═══════════════════════════════════════════════════════════════════════════════
// EVENT DISPATCHERS: 1/1 migrated
// ═══════════════════════════════════════════════════════════════════════════════

/** [1/1] Called when an actor is traced during weapon collision
 * @param Actor - The actor that was hit
 * @param Hit - Hit result data
 * @param Multiplier - Damage multiplier for this hit
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorTraced, AActor*, Actor, FHitResult, Hit, double, Multiplier);

UCLASS(ClassGroup = (Soulslike), meta = (BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class SLFCONVERSION_API UCollisionManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCollisionManagerComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 11/11 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Config Variables (4) ---

	/** [1/11] Socket name for trace start point */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FName TraceSocketStart;

	/** [2/11] Socket name for trace end point */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FName TraceSocketEnd;

	/** [3/11] Radius of sphere trace */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	double TraceRadius;

	/** [4/11] Object types to trace against */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceTypes;

	/** [5/11] Debug draw mode for traces */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TEnumAsByte<EDrawDebugTrace::Type> TraceDebugMode;

	// --- Runtime Variables (6) ---

	/** [6/11] Target mesh component to trace sockets from */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	USceneComponent* TargetMesh;

	/** [7/11] Array of actors already traced this swing */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	TArray<AActor*> TracedActors;

	/** [8/11] Damage multiplier for current attack */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	double DamageMultiplier;

	/** [9/11] Trace size multiplier for current attack */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	double TraceSizeMultiplier;

	/** [10/11] Last trace start position (for substepping) */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	FVector LastStartPosition;

	/** [11/11] Last trace end position (for substepping) */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	FVector LastEndPosition;

	// ═══════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS: 1/1 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/1] Broadcast when a new actor is traced */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnActorTraced OnActorTraced;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 9/9 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Multiplier Functions (2) ---

	/** [1/9] Set damage and trace size multipliers
	 * @param InDamageMultiplier - Damage multiplier to set
	 * @param InTraceSizeMultiplier - Trace size multiplier to set
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Collision Manager|Multipliers")
	void SetMultipliers(double InDamageMultiplier, double InTraceSizeMultiplier);
	virtual void SetMultipliers_Implementation(double InDamageMultiplier, double InTraceSizeMultiplier);

	/** [2/9] Get current multipliers
	 * @param OutDamageMultiplier - Current damage multiplier
	 * @param OutTraceSizeMultiplier - Current trace size multiplier
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Collision Manager|Multipliers")
	void GetMultipliers(double& OutDamageMultiplier, double& OutTraceSizeMultiplier);
	virtual void GetMultipliers_Implementation(double& OutDamageMultiplier, double& OutTraceSizeMultiplier);

	// --- Trace Functions (4) ---

	/** [3/9] Get trace start and end locations from sockets
	 * @param OutTraceStart - Start position (TraceSocketStart)
	 * @param OutTraceEnd - End position (TraceSocketEnd)
	 */
	UFUNCTION(BlueprintPure, Category = "Collision Manager|Trace")
	void GetTraceLocations(FVector& OutTraceStart, FVector& OutTraceEnd) const;

	/** [4/9] Perform a substepped trace to avoid missing fast weapon swings
	 * Interpolates between last and current positions based on step size
	 * @param StepSize - Distance between substeps
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Collision Manager|Trace")
	void SubsteppedTrace(double StepSize);
	virtual void SubsteppedTrace_Implementation(double StepSize);

	/** [5/9] Process trace results, filter duplicates, broadcast events
	 * @param HitResults - Array of hit results from trace
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Collision Manager|Trace")
	void ProcessTrace(const TArray<FHitResult>& HitResults);
	virtual void ProcessTrace_Implementation(const TArray<FHitResult>& HitResults);

	/** [6/9] Toggle trace enabled state
	 * @param bEnabled - True to enable tracing, false to disable
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Collision Manager|Trace")
	void ToggleTrace(bool bEnabled);
	virtual void ToggleTrace_Implementation(bool bEnabled);

	// --- Initialization (1) ---

	/** [7/9] Initialize trace points (called on BeginPlay)
	 * Finds skeletal or static mesh component for socket locations
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Collision Manager|Init")
	void InitializeTracePoints();
	virtual void InitializeTracePoints_Implementation();

	// --- Lifecycle (2) - handled by virtual overrides ---
	// [8/9] ReceiveBeginPlay -> BeginPlay override
	// [9/9] ReceiveTick -> TickComponent override
};
