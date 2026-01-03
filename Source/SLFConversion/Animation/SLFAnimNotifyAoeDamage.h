// SLFAnimNotifyAoeDamage.h
// Area of effect damage during animation
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "SLFAnimNotifyAoeDamage.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFAnimNotifyAoeDamage : public UAnimNotify
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AoE")
	float Radius = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AoE")
	float Damage = 50.0f;

	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
