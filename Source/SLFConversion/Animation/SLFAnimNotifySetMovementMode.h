// SLFAnimNotifySetMovementMode.h
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SLFAnimNotifySetMovementMode.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFAnimNotifySetMovementMode : public UAnimNotify
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	TEnumAsByte<EMovementMode> NewMovementMode = MOVE_Walking;

	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
