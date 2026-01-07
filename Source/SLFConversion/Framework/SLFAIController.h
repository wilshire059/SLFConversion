// SLFAIController.h
// C++ base for AIC_SoulslikeFramework - AI Controller
//
// 20-PASS VALIDATION: 2026-01-06 - Full interface implementation
// Source: BlueprintDNA/Blueprint/AIC_SoulslikeFramework.json
// Parent: AAIController
// Variables: 0 | Functions: 1 (interface) | Dispatchers: 0 | Components: 1

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Interfaces/BPI_AIC.h"
#include "Perception/AIPerceptionComponent.h"
#include "SLFAIController.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFAIController : public AAIController, public IBPI_AIC
{
	GENERATED_BODY()

public:
	ASLFAIController();

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	// ═══════════════════════════════════════════════════════════════════════════════
	// COMPONENTS
	// Note: AIPerception is defined in Blueprint SCS
	// Use inherited GetAIPerceptionComponent() from AAIController
	// ═══════════════════════════════════════════════════════════════════════════════

	// ═══════════════════════════════════════════════════════════════════════════════
	// BPI_AIC INTERFACE IMPLEMENTATIONS [1/1]
	// ═══════════════════════════════════════════════════════════════════════════════

	/** Initialize behavior tree - called from AC_AI_BehaviorManager */
	virtual void InitializeBehavior_Implementation(UBehaviorTree* BehaviorTree) override;
};
