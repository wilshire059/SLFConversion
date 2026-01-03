// SLFAnimNotifyStateWeaponTrace.h
// Duration-based weapon trace for hit detection
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - ANS_WeaponTrace
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         2/2 (socket names)
// Functions:         3/3 (Begin, Tick, End)
// Event Dispatchers: 0/0
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "SLFAnimNotifyStateWeaponTrace.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFAnimNotifyStateWeaponTrace : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	FName StartSocketName = "weapon_start";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	FName EndSocketName = "weapon_end";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	float TraceRadius = 10.0f;

	virtual FString GetNotifyName_Implementation() const override;
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
