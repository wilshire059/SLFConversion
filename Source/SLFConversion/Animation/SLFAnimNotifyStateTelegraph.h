// SLFAnimNotifyStateTelegraph.h
// Spawns a neon glow VFX on weapon/hand sockets to telegraph incoming attacks.
// Placed before each weapon trace window by AddMultipleWeaponTracesToMontage().

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "SLFAnimNotifyStateTelegraph.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFAnimNotifyStateTelegraph : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	/** Socket to attach the telegraph VFX to */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telegraph")
	FName AttachSocket = "weapon_r";

	/** Second bone for dual-point telegraph (NAME_None = single point only) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telegraph")
	FName SecondSocket = "hand_r";

	/** Niagara system to spawn. If null, uses built-in NS_NeonTelegraph. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telegraph")
	class UNiagaraSystem* TelegraphVFX = nullptr;

	/** Neon glow color */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telegraph")
	FLinearColor TelegraphColor = FLinearColor(1.0f, 0.4f, 0.05f, 1.0f);  // Hot orange

	/** Scale of the VFX */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telegraph")
	float VFXScale = 1.0f;

	// Runtime
	UPROPERTY(Transient)
	class UNiagaraComponent* ActiveVFX1 = nullptr;

	UPROPERTY(Transient)
	class UNiagaraComponent* ActiveVFX2 = nullptr;

	virtual FString GetNotifyName_Implementation() const override;
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
