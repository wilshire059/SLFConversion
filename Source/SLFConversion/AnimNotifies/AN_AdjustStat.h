// AN_AdjustStat.h
// C++ Animation Notify for AN_AdjustStat
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Animation/AN_AdjustStat.json
// Parent: UAnimNotify
// Variables: 4 | Functions: 2

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LevelSequence.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "AN_AdjustStat.generated.h"

// Forward declarations for Blueprint classes
class AB_BaseProjectile;
class AB_Interactable;
class UB_Action;
class UB_Buff;
class UB_StatusEffect;

UCLASS()
class SLFCONVERSION_API UAN_AdjustStat : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAN_AdjustStat();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (4)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FGameplayTag StatTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	ESLFValueType ValueType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	double Change;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	bool TriggerRegen;

	// Notify callback
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;

	// ═══════════════════════════════════════════════════════════════════════
	// HELPER FUNCTIONS (2)
	// ═══════════════════════════════════════════════════════════════════════

};
