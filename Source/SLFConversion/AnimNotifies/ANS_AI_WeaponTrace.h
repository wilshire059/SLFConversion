// ANS_AI_WeaponTrace.h
// C++ Animation Notify for ANS_AI_WeaponTrace
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Animation/ANS_AI_WeaponTrace.json
// Parent: UAnimNotifyState
// Variables: 3 | Functions: 3

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LevelSequence.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "ANS_AI_WeaponTrace.generated.h"

// Forward declarations for Blueprint classes
class AB_BaseProjectile;
class AB_Interactable;
class UB_Action;
class UB_Buff;
class UB_StatusEffect;

UCLASS()
class SLFCONVERSION_API UANS_AI_WeaponTrace : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UANS_AI_WeaponTrace();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (3)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	TArray<FName> TracedWeaponSockets;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	double DamageMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	double TraceSizeMultiplier;

	// Notify state callbacks
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;

	// ═══════════════════════════════════════════════════════════════════════
	// HELPER FUNCTIONS (3)
	// ═══════════════════════════════════════════════════════════════════════

};
