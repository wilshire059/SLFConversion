// SLFAnimNotifySpawnProjectile.cpp
#include "SLFAnimNotifySpawnProjectile.h"

FString USLFAnimNotifySpawnProjectile::GetNotifyName_Implementation() const
{
	return TEXT("Spawn Projectile");
}

void USLFAnimNotifySpawnProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp || !ProjectileClass) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	FVector SpawnLocation = MeshComp->GetSocketLocation(SpawnSocketName);
	FRotator SpawnRotation = Owner->GetActorRotation();

	UE_LOG(LogTemp, Log, TEXT("[AN_SpawnProjectile] Spawning %s at socket %s"),
		*ProjectileClass->GetName(), *SpawnSocketName.ToString());

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Owner;
	SpawnParams.Instigator = Cast<APawn>(Owner);

	Owner->GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
}
