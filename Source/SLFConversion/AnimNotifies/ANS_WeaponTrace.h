// ANS_WeaponTrace.h
// C++ Animation Notify for ANS_WeaponTrace
//
// 20-PASS VALIDATION: 2026-01-12 - Uses CollisionManager on attached weapons
// Parent: UAnimNotifyState
// Enables CollisionManager tracing on equipped weapons during attack animations

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "SLFEnums.h"
#include "ANS_WeaponTrace.generated.h"

UCLASS()
class SLFCONVERSION_API UANS_WeaponTrace : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UANS_WeaponTrace();

	// ═══════════════════════════════════════════════════════════════════════
	// CONFIGURATION
	// ═══════════════════════════════════════════════════════════════════════

	/** Which hand/weapon to trace (used for selecting which weapons to enable) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Trace")
	ESLFTraceType TraceType;

	/** Damage multiplier for this attack (e.g., 1.5 for heavy attack) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Trace")
	double DamageMultiplier;

	// ═══════════════════════════════════════════════════════════════════════
	// NOTIFY CALLBACKS
	// ═══════════════════════════════════════════════════════════════════════

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;
};
