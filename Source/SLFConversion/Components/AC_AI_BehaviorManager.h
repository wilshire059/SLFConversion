// AC_AI_BehaviorManager.h
// C++ component for AC_AI_BehaviorManager
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Component/AC_AI_BehaviorManager.json
// Variables: 11 | Functions: 7 | Dispatchers: 0

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AC_AI_BehaviorManager.generated.h"

// Forward declarations
class AB_PatrolPath;
class UAnimMontage;
class UDataTable;
class UPrimaryDataAsset;

// Event Dispatcher Declarations


UCLASS(ClassGroup=(SoulslikeFramework), meta=(BlueprintSpawnableComponent))
class SLFCONVERSION_API UAC_AI_BehaviorManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UAC_AI_BehaviorManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (11)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Config")
	UBehaviorTree* Behavior;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Config")
	ESLFAIStates CurrentState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Config")
	AB_PatrolPath* PatrolPath;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	ESLFAIStates PreviousState;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	AActor* CurrentTarget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat Related")
	double MaxChaseDistanceThreshold;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat Related")
	double AttackDistanceThreshold;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat Related")
	double StrafeDistanceThreshold;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat Related")
	double SpeedAdjustDistanceThreshold;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat Related")
	double MinimumStrafePointDistanceThreshold;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat Related")
	TSet<FSLFAiStrafeInfo> StrafeMethods;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (7)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_AI_BehaviorManager")
	void GetBlackboard(UBlackboardComponent*& OutReturnValue, UBlackboardComponent*& OutReturnValue_1);
	virtual void GetBlackboard_Implementation(UBlackboardComponent*& OutReturnValue, UBlackboardComponent*& OutReturnValue_1);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_AI_BehaviorManager")
	void SetKeyValue(const FName& KeyName, const FInstancedStruct& Data);
	virtual void SetKeyValue_Implementation(const FName& KeyName, const FInstancedStruct& Data);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_AI_BehaviorManager")
	void SetState(ESLFAIStates NewState, const FInstancedStruct& Data);
	virtual void SetState_Implementation(ESLFAIStates NewState, const FInstancedStruct& Data);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_AI_BehaviorManager")
	void SetTarget(AActor* Target);
	virtual void SetTarget_Implementation(AActor* Target);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_AI_BehaviorManager")
	AActor* GetTarget();
	virtual AActor* GetTarget_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_AI_BehaviorManager")
	ESLFAIStates GetState(ESLFAIStateHandle Handle);
	virtual ESLFAIStates GetState_Implementation(ESLFAIStateHandle Handle);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_AI_BehaviorManager")
	void SetPatrolPath(AB_PatrolPath* InPatrolPath);
	virtual void SetPatrolPath_Implementation(AB_PatrolPath* InPatrolPath);
};
