// SLFAnimNotifyStateWeaponTrace.cpp
#include "SLFAnimNotifyStateWeaponTrace.h"

FString USLFAnimNotifyStateWeaponTrace::GetNotifyName_Implementation() const
{
	return TEXT("Weapon Trace");
}

void USLFAnimNotifyStateWeaponTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp) return;
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	UE_LOG(LogTemp, Log, TEXT("[ANS_WeaponTrace] Begin on %s - Duration: %.2fs"), *Owner->GetName(), TotalDuration);

	// TODO: Enable weapon trace via CombatManager
}

void USLFAnimNotifyStateWeaponTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!MeshComp) return;

	// Get socket positions for trace
	FVector StartPos = MeshComp->GetSocketLocation(StartSocketName);
	FVector EndPos = MeshComp->GetSocketLocation(EndSocketName);

	// TODO: Perform sphere trace between sockets, process hits via CombatManager
}

void USLFAnimNotifyStateWeaponTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	UE_LOG(LogTemp, Log, TEXT("[ANS_WeaponTrace] End"));

	// TODO: Disable weapon trace, clear hit actors list
}
