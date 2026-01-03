// AN_CameraShake.h
// C++ Animation Notify for AN_CameraShake
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Animation/AN_CameraShake.json
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
#include "Camera/CameraShakeBase.h"
#include "AN_CameraShake.generated.h"

UCLASS()
class SLFCONVERSION_API UAN_CameraShake : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAN_CameraShake();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (2) - from Blueprint JSON
	// ═══════════════════════════════════════════════════════════════════════

	/** Camera shake class to play */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	TSubclassOf<UCameraShakeBase> CameraShake;

	/** Scale of the camera shake */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	double Scale;

	// Notify callback
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;

	// ═══════════════════════════════════════════════════════════════════════
	// HELPER FUNCTIONS (2)
	// ═══════════════════════════════════════════════════════════════════════

};
