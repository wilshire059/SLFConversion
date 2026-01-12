// AIC_SoulslikeFramework.h
// C++ class for Blueprint AIC_SoulslikeFramework
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - AIC_SoulslikeFramework
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0
// Functions:         1/1 migrated (InitializeBehavior)
// Event Dispatchers: 0
// Interfaces:        BPI_AIC (1 function)
// Components:        AIPerceptionComponent
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/_AI/Misc/AIC_SoulslikeFramework
//
// 20-PASS VALIDATION: 2026-01-11 - Fixed inheritance from UObject -> AAIController
// PURPOSE: AI Controller that runs behavior trees and handles perception

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Interfaces/BPI_AIC.h"
#include "AIC_SoulslikeFramework.generated.h"

// Forward declarations
class UBehaviorTree;
class UAIPerceptionComponent;

/**
 * Soulslike AI Controller - handles behavior tree and perception
 * Implements BPI_AIC interface (InitializeBehavior)
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API AAIC_SoulslikeFramework : public AAIController, public IBPI_AIC
{
	GENERATED_BODY()

public:
	AAIC_SoulslikeFramework();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;

public:
	// ═══════════════════════════════════════════════════════════════════
	// COMPONENTS
	// ═══════════════════════════════════════════════════════════════════

	/** AI Perception component for sensing targets */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;

	// ═══════════════════════════════════════════════════════════════════
	// BPI_AIC INTERFACE IMPLEMENTATION (1 function)
	// ═══════════════════════════════════════════════════════════════════

	/** [1/1] Initialize and run behavior tree */
	virtual void InitializeBehavior_Implementation(UBehaviorTree* BehaviorTree) override;
};
