// AN_FootstepTrace.h
// C++ Animation Notify for AN_FootstepTrace
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA_v2/Animation/AN_FootstepTrace.json
// Parent: UAnimNotify
// Variables: 6 | Functions: 2

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Sound/SoundBase.h"
#include "NiagaraSystem.h"
#include "AN_FootstepTrace.generated.h"

UCLASS()
class SLFCONVERSION_API UAN_FootstepTrace : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAN_FootstepTrace();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (6) - from Blueprint JSON
	// ═══════════════════════════════════════════════════════════════════════

	/** Socket name on mesh to trace from (e.g., foot_l, foot_r) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FName FootSocket;

	/** Length of the downward trace */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	float TraceLength;

	/** Object types to trace against */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceType;

	/** Debug trace visualization mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	TEnumAsByte<EDrawDebugTrace::Type> DebugTrace;

	/** Map of physical surface type to sound asset */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	TMap<TEnumAsByte<EPhysicalSurface>, TSoftObjectPtr<USoundBase>> Sounds;

	/** Map of physical surface type to VFX asset */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	TMap<TEnumAsByte<EPhysicalSurface>, TSoftObjectPtr<UNiagaraSystem>> Effects;

	// Notify callback
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;
};
