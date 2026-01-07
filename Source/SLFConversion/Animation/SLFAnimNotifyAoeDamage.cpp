// SLFAnimNotifyAoeDamage.cpp
#include "SLFAnimNotifyAoeDamage.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Engine/DamageEvents.h"

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

	// Sphere overlap to find all actors in range
	TArray<AActor*> OverlappedActors;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Owner);

	bool bHit = UKismetSystemLibrary::SphereOverlapActors(
		Owner->GetWorld(),
		Owner->GetActorLocation(),
		Radius,
		TArray<TEnumAsByte<EObjectTypeQuery>>(),
		nullptr,
		ActorsToIgnore,
		OverlappedActors
	);

	if (bHit)
	{
		for (AActor* HitActor : OverlappedActors)
		{
			if (!HitActor) continue;
			if (HitActor == Owner) continue;

			// Apply point damage to each actor
			FPointDamageEvent DamageEvent;
			DamageEvent.Damage = Damage;
			DamageEvent.HitInfo.ImpactPoint = HitActor->GetActorLocation();
			DamageEvent.HitInfo.ImpactNormal = (Owner->GetActorLocation() - HitActor->GetActorLocation()).GetSafeNormal();
			DamageEvent.ShotDirection = -DamageEvent.HitInfo.ImpactNormal;
			DamageEvent.DamageTypeClass = UDamageType::StaticClass();

			HitActor->TakeDamage(Damage, DamageEvent, Owner->GetInstigatorController(), Owner);

			UE_LOG(LogTemp, Verbose, TEXT("[AN_AoeDamage] Applied %.0f damage to %s"), Damage, *HitActor->GetName());
		}
	}
}
