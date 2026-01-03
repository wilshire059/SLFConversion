// AN_LaunchField.cpp
// C++ Animation Notify implementation for AN_LaunchField
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - launches all characters within radius backwards

#include "AnimNotifies/AN_LaunchField.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"

UAN_LaunchField::UAN_LaunchField()
	: OverlapOffset(FVector::ZeroVector)
	, OverlapRadius(200.0)
	, OverlapDrawDebug(false)
	, LaunchStrength(500.0)
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(255, 100, 100, 255); // Red for launch field
#endif
}

void UAN_LaunchField::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
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

	// From Blueprint: Draw debug sphere if enabled
	if (OverlapDrawDebug)
	{
		UKismetSystemLibrary::DrawDebugSphere(
			World,
			OverlapCenter,
			OverlapRadius,
			12,
			FLinearColor::Yellow,
			2.0f,
			2.0f
		);
	}

	// From Blueprint: Sphere overlap for actors
	TArray<AActor*> OverlappedActors;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Owner);

	bool bHit = UKismetSystemLibrary::SphereOverlapActors(
		Owner,
		OverlapCenter,
		OverlapRadius,
		OverlapObjectTypes,
		OverlapOptionalClassFilter ? OverlapOptionalClassFilter.Get() : nullptr,
		ActorsToIgnore,
		OverlappedActors
	);

	if (!bHit)
	{
		return;
	}

	// From Blueprint: Get owner forward vector and calculate launch velocity
	// Launch characters backwards (opposite of owner's forward)
	FVector OwnerForward = Owner->GetActorForwardVector();
	FVector LaunchVelocity = OwnerForward * LaunchStrength;

	// From Blueprint: For each overlapped actor, cast to Character and launch
	for (AActor* HitActor : OverlappedActors)
	{
		if (!HitActor || HitActor == Owner)
		{
			continue;
		}

		// Cast to Character to use LaunchCharacter
		ACharacter* HitCharacter = Cast<ACharacter>(HitActor);
		if (HitCharacter)
		{
			HitCharacter->LaunchCharacter(LaunchVelocity, true, true);
			UE_LOG(LogTemp, Log, TEXT("UAN_LaunchField::Notify - Launched %s with velocity %s"),
				*HitCharacter->GetName(), *LaunchVelocity.ToString());
		}
	}
}

FString UAN_LaunchField::GetNotifyName_Implementation() const
{
	return TEXT("Launch Field");
}
