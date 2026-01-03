// ANS_AI_FistTrace.h
// C++ Animation Notify for ANS_AI_FistTrace
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Animation/ANS_AI_FistTrace.json
// Parent: UAnimNotifyState
// Variables: 5 | Functions: 4

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LevelSequence.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "ANS_AI_FistTrace.generated.h"

// Forward declarations for Blueprint classes
class AB_BaseProjectile;
class AB_Interactable;
class UB_Action;
class UB_Buff;
class UB_StatusEffect;

UCLASS()
class SLFCONVERSION_API UANS_AI_FistTrace : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UANS_AI_FistTrace();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (5)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	ESLFTraceType TraceType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DEBUG (In Editor Only)")
	double DebugTraceThickness;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DEBUG (In Editor Only)")
	FLinearColor DebugTraceColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DEBUG (In Editor Only)")
	double DebugTraceDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DEBUG (in Editor Only)")
	double DebugRadius;

	// Notify state callbacks
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;

	// ═══════════════════════════════════════════════════════════════════════
	// HELPER FUNCTIONS (4)
	// ═══════════════════════════════════════════════════════════════════════

};
