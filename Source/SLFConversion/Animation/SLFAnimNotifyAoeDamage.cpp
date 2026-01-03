// SLFAnimNotifyAoeDamage.cpp
#include "SLFAnimNotifyAoeDamage.h"
#include "Kismet/KismetSystemLibrary.h"

FString USLFAnimNotifyAoeDamage::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("AoE Damage (%.0f radius)"), Radius);
}

void USLFAnimNotifyAoeDamage::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	UE_LOG(LogTemp, Log, TEXT("[AN_AoeDamage] Dealing %.0f damage in %.0f radius"), Damage, Radius);
	// TODO: Sphere overlap, apply damage to all hit actors
}
