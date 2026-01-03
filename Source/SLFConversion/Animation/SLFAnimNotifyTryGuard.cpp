// SLFAnimNotifyTryGuard.cpp
#include "SLFAnimNotifyTryGuard.h"

FString USLFAnimNotifyTryGuard::GetNotifyName_Implementation() const
{
	return TEXT("Try Guard");
}

void USLFAnimNotifyTryGuard::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	UE_LOG(LogTemp, Log, TEXT("[AN_TryGuard] Notify on %s"), *Owner->GetName());

	// TODO: Get InputBuffer component, call QueueAction with Guard tag
}
