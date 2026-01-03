// AN_AoeDamage.h
// C++ Animation Notify for AN_AoeDamage
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Animation/AN_AoeDamage.json
// Parent: UAnimNotify
// Variables: 7 | Functions: 2

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LevelSequence.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "AN_AoeDamage.generated.h"

// Forward declarations for Blueprint classes
class AB_BaseProjectile;
class AB_Interactable;
class UB_Action;
class UB_Buff;
class UB_StatusEffect;

UCLASS()
class SLFCONVERSION_API UAN_AoeDamage : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAN_AoeDamage();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (7)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	double MinDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	double MaxDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FVector OverlapOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	double Radius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	TSubclassOf<UObject> OptionalClassFilter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	bool DrawDebug;

	// Notify callback
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;

	// ═══════════════════════════════════════════════════════════════════════
	// HELPER FUNCTIONS (2)
	// ═══════════════════════════════════════════════════════════════════════

};
