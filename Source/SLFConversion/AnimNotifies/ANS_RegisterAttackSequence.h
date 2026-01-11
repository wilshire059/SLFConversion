// ANS_RegisterAttackSequence.h
// C++ Animation Notify for ANS_RegisterAttackSequence
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Animation/ANS_RegisterAttackSequence.json
// Parent: UAnimNotifyState
// Variables: 1 | Functions: 3

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_RegisterAttackSequence.generated.h"

// Forward declarations
class UAC_CombatManager;

UCLASS()
class SLFCONVERSION_API UANS_RegisterAttackSequence : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UANS_RegisterAttackSequence();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (1)
	// ═══════════════════════════════════════════════════════════════════════

	/** The montage section name to register for combo continuation (e.g., "Light_02", "Heavy_01") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FName QueuedSection;

	// Notify state callbacks
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;

	// ═══════════════════════════════════════════════════════════════════════
	// HELPER FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════

	/** Get CombatManager from actor (checks both actor and controller) */
	UAC_CombatManager* GetCombatManager(AActor* Owner);
};
