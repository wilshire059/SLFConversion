// AN_WorldCameraShake.h
// C++ Animation Notify for AN_WorldCameraShake
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Animation/AN_WorldCameraShake.json
// Parent: UAnimNotify
// Variables: 5 | Functions: 2

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LevelSequence.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "AN_WorldCameraShake.generated.h"

// Forward declarations for Blueprint classes
class AB_BaseProjectile;
class AB_Interactable;
class UB_Action;
class UB_Buff;
class UB_StatusEffect;

UCLASS()
class SLFCONVERSION_API UAN_WorldCameraShake : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAN_WorldCameraShake();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (5)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	TSubclassOf<UObject> CameraShake;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	double InnerRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	double OuterRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	double Falloff;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	bool OrientTowardsEpicenter;

	// Notify callback
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;

	// ═══════════════════════════════════════════════════════════════════════
	// HELPER FUNCTIONS (2)
	// ═══════════════════════════════════════════════════════════════════════

};
