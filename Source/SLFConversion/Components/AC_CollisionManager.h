// AC_CollisionManager.h
// C++ component for AC_CollisionManager
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Component/AC_CollisionManager.json
// Variables: 11 | Functions: 5 | Dispatchers: 1

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AC_CollisionManager.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;
class UPrimaryDataAsset;

// Event Dispatcher Declarations
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAC_CollisionManager_OnActorTraced, AActor*, Actor, FHitResult, Hit, double, Multiplier);

UCLASS(ClassGroup=(SoulslikeFramework), meta=(BlueprintSpawnableComponent))
class SLFCONVERSION_API UAC_CollisionManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UAC_CollisionManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (11)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	FName TraceSocketStart;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	FName TraceSocketEnd;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	double TraceRadius;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	USceneComponent* TargetMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceTypes;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	TEnumAsByte<EDrawDebugTrace::Type> TraceDebugMode;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	TArray<AActor*> TracedActors;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	double DamageMultiplier;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	double TraceSizeMultiplier;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	FVector LastStartPosition;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	FVector LastEndPosition;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	bool bTraceActive;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_CollisionManager_OnActorTraced OnActorTraced;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (5)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_CollisionManager")
	void GetTraceLocations(FVector& OutTraceStartLocation, FVector& OutTraceEndLocation);
	virtual void GetTraceLocations_Implementation(FVector& OutTraceStartLocation, FVector& OutTraceEndLocation);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_CollisionManager")
	void SubsteppedTrace(double StepSize);
	virtual void SubsteppedTrace_Implementation(double StepSize);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_CollisionManager")
	void ProcessTrace(const TArray<FHitResult>& Hits);
	virtual void ProcessTrace_Implementation(const TArray<FHitResult>& Hits);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_CollisionManager")
	void GetMultipliers(double& OutDamageMultiplier, double& OutTraceSizeMultiplier);
	virtual void GetMultipliers_Implementation(double& OutDamageMultiplier, double& OutTraceSizeMultiplier);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_CollisionManager")
	void SetMultipliers(double InDamageMultiplier, double InTraceSizeMultiplier);
	virtual void SetMultipliers_Implementation(double InDamageMultiplier, double InTraceSizeMultiplier);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_CollisionManager")
	void ToggleTrace(bool bEnable);
	virtual void ToggleTrace_Implementation(bool bEnable);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_CollisionManager")
	void InitializeTracePoints();
	virtual void InitializeTracePoints_Implementation();
};
