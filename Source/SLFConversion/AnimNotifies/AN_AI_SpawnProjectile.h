// AN_AI_SpawnProjectile.h
// C++ Animation Notify for AN_AI_SpawnProjectile
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Animation/AN_AI_SpawnProjectile.json
// Parent: UAnimNotify
// Variables: 3 | Functions: 2

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LevelSequence.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "AN_AI_SpawnProjectile.generated.h"

// Forward declarations for Blueprint classes
class AB_BaseProjectile;
class AB_Interactable;
class UB_Action;
class UB_Buff;
class UB_StatusEffect;

UCLASS()
class SLFCONVERSION_API UAN_AI_SpawnProjectile : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAN_AI_SpawnProjectile();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (3)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	TSubclassOf<AB_BaseProjectile> ProjectileClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FName SpawnSocketName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	bool UseSocketRotation;

	// Notify callback
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;

	// ═══════════════════════════════════════════════════════════════════════
	// HELPER FUNCTIONS (2)
	// ═══════════════════════════════════════════════════════════════════════

};
