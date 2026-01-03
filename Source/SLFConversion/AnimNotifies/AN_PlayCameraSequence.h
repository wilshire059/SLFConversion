// AN_PlayCameraSequence.h
// C++ Animation Notify for AN_PlayCameraSequence
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Animation/AN_PlayCameraSequence.json
// Parent: UAnimNotify
// Variables: 2 | Functions: 2

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LevelSequence.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "AN_PlayCameraSequence.generated.h"

// Forward declarations for Blueprint classes
class AB_BaseProjectile;
class AB_Interactable;
class UB_Action;
class UB_Buff;
class UB_StatusEffect;

UCLASS()
class SLFCONVERSION_API UAN_PlayCameraSequence : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAN_PlayCameraSequence();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	ULevelSequence* Sequence;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FMovieSceneSequencePlaybackSettings Settings;

	// Notify callback
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;

	// ═══════════════════════════════════════════════════════════════════════
	// HELPER FUNCTIONS (2)
	// ═══════════════════════════════════════════════════════════════════════

};
