// SLFAnimNotifyTryGuard.h
// C++ base for AN_TryGuard - Check and process guard during animation
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - AN_TryGuard
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0
// Functions:         2/2 (Received_Notify, GetNotifyName)
// Event Dispatchers: 0/0
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "SLFAnimNotifyTryGuard.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFAnimNotifyTryGuard : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
