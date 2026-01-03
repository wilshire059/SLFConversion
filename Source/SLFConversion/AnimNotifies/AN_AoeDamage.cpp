// AN_AoeDamage.cpp
// C++ Animation Notify implementation for AN_AoeDamage
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - applies AOE damage via sphere overlap

#include "AnimNotifies/AN_AoeDamage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

UAN_AoeDamage::UAN_AoeDamage()
	: MinDamage(10.0)
	, MaxDamage(20.0)
	, OverlapOffset(FVector::ZeroVector)
	, Radius(100.0)
	, DrawDebug(false)
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(255, 50, 50, 255); // Red for damage
#endif
}

void UAN_AoeDamage::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	UWorld* World = Owner->GetWorld();
	if (!World)
	{
		return;
	}

	// Calculate overlap center (owner location + offset)
	FVector OverlapCenter = Owner->GetActorLocation() + OverlapOffset;

	// Draw debug sphere if enabled
	if (DrawDebug)
	{
		UKismetSystemLibrary::DrawDebugSphere(
			World,
			OverlapCenter,
			Radius,
			12,
			FLinearColor::Red,
			2.0f,
			2.0f
		);
	}

	// Sphere overlap for actors
	TArray<AActor*> OverlappedActors;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Owner);

	bool bHit = UKismetSystemLibrary::SphereOverlapActors(
		Owner,
		OverlapCenter,
		Radius,
		ObjectTypes,
		OptionalClassFilter,
		ActorsToIgnore,
		OverlappedActors
	);

	if (!bHit)
	{
		return;
	}

	// Apply damage to each overlapped actor
	for (AActor* HitActor : OverlappedActors)
	{
		if (!HitActor || HitActor == Owner)
		{
			continue;
		}

		// Random damage in range
		float Damage = FMath::RandRange(MinDamage, MaxDamage);

		UGameplayStatics::ApplyDamage(
			HitActor,
			Damage,
			Owner->GetInstigatorController(),
			Owner,
			nullptr // DamageType
		);

		UE_LOG(LogTemp, Log, TEXT("UAN_AoeDamage::Notify - Applied %.2f damage to %s"), Damage, *HitActor->GetName());
	}
}

FString UAN_AoeDamage::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("AOE Damage (%.0f-%.0f)"), MinDamage, MaxDamage);
}
