// AN_LaunchField.h
// C++ Animation Notify for AN_LaunchField
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Animation/AN_LaunchField.json
// Parent: UAnimNotify
// Variables: 6 | Functions: 2

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LevelSequence.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "AN_LaunchField.generated.h"

// Forward declarations for Blueprint classes
class AB_BaseProjectile;
class AB_Interactable;
class UB_Action;
class UB_Buff;
class UB_StatusEffect;

UCLASS()
class SLFCONVERSION_API UAN_LaunchField : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAN_LaunchField();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (6)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FVector OverlapOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	double OverlapRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	TArray<TEnumAsByte<EObjectTypeQuery>> OverlapObjectTypes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	TSubclassOf<UObject> OverlapOptionalClassFilter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	bool OverlapDrawDebug;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	double LaunchStrength;

	// Notify callback
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;

	// ═══════════════════════════════════════════════════════════════════════
	// HELPER FUNCTIONS (2)
	// ═══════════════════════════════════════════════════════════════════════

};
