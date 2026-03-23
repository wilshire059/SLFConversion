// SLFAnimNotifyStateWeaponTrace.h
// Duration-based weapon trace for hit detection
//
// Two modes:
//   1. Two-socket mode (default): Traces between StartSocketName and EndSocketName.
//   2. Directional reach mode: When WeaponReach > 0, traces from StartSocketName
//      along the socket's local axis (WeaponDirectionAxis) for WeaponReach cm.
//      Use this for mesh-baked weapons where you can't place a socket at the tip.
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - ANS_WeaponTrace
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         7 (sockets, radius, reach, axis, negate, debug, damage)
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
	// --- Socket configuration ---

	/** Socket at weapon base/hilt (used by both modes) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace|Sockets")
	FName StartSocketName = "weapon_start";

	/** Socket at weapon tip (two-socket mode only, ignored when WeaponReach > 0) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace|Sockets")
	FName EndSocketName = "weapon_end";

	// --- Trace shape ---

	/** Sphere trace radius in world-space cm */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace|Shape")
	float TraceRadius = 30.0f;

	// --- Directional reach mode (for mesh-baked weapons) ---

	/** World-space cm to extend from StartSocket along WeaponDirectionAxis.
	 *  When > 0, uses directional reach instead of two-socket mode.
	 *  Set this to the weapon's visual length in world space. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace|Reach")
	float WeaponReach = 0.0f;

	/** Bone used to compute weapon direction (bone-to-bone mode).
	 *  When set, direction = (StartSocket - DirectionBone).Normalized,
	 *  i.e., from this bone THROUGH the start socket and beyond into the weapon.
	 *  Example: "lowerarm_r" traces from elbow through hand toward blade tip.
	 *  When NAME_None, falls back to socket local axis (WeaponDirectionAxis). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace|Reach")
	FName DirectionBoneName = NAME_None;

	/** Which local axis of the StartSocket points along the weapon blade.
	 *  Only used when WeaponReach > 0 AND DirectionBoneName is None.
	 *  X = socket forward, Y = socket right, Z = socket up. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace|Reach")
	TEnumAsByte<EAxis::Type> WeaponDirectionAxis = EAxis::X;

	/** Negate the direction axis (e.g., trace along -X instead of +X).
	 *  Only used when WeaponReach > 0. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace|Reach")
	bool bNegateDirection = false;

	// --- AI Damage overrides ---

	/** Base damage for AI attackers (overrides the default 50). Set per-attack for varied damage. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace|Damage")
	float OverrideDamage = -1.0f;

	/** Base poise damage for AI attackers (overrides the default 25). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace|Damage")
	float OverridePoiseDamage = -1.0f;

	/** Seconds between clearing the hit list, allowing re-hits on the same target.
	 *  0 = single hit per target per trace (default for normal attacks).
	 *  Set > 0 on specific long/whirlwind montages only. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace|Damage")
	float HitResetInterval = 0.0f;

	// --- Slash VFX ---

	/** Niagara system to spawn as a slash trail during the trace window.
	 *  Defaults to NS_RibbonTrail if not set. Set to None to disable. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace|VFX")
	class UNiagaraSystem* SlashVFX = nullptr;

	/** Color for the slash trail (Convergence purple by default) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace|VFX")
	FLinearColor SlashColor = FLinearColor(0.4f, 0.1f, 0.6f, 1.0f);

	/** Scale multiplier for the slash VFX */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace|VFX")
	float SlashVFXScale = 1.0f;

	// --- Debug ---

	/** Draw debug trace spheres in PIE for visual tuning */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace|Debug")
	bool bDrawDebugTrace = true;

	// --- Runtime state ---

	UPROPERTY(Transient)
	TArray<AActor*> HitActors;

	UPROPERTY(Transient)
	class UNiagaraComponent* ActiveSlashComponent = nullptr;

	float HitResetTimer = 0.0f;

	/** Previous frame's start position for swing-arc tracing */
	FVector PreviousStartPos = FVector::ZeroVector;
	bool bHasPreviousPos = false;

	virtual FString GetNotifyName_Implementation() const override;
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
