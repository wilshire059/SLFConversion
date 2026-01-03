// SLFChaosForceField.cpp
#include "SLFChaosForceField.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

ASLFChaosForceField::ASLFChaosForceField()
{


	// Set default duration
	Duration = 10.0f;

	UE_LOG(LogTemp, Log, TEXT("[ChaosForceField] Created"));
}

void ASLFChaosForceField::BeginPlay()
{
	Super::BeginPlay();

	// Update collision radius
	if (ForceFieldCollision)
	{
		if (ForceFieldCollision) ForceFieldCollision->SetSphereRadius(Radius);
	}

	// Scale mesh to match radius
	if (ForceFieldMesh)
	{
		float Scale = Radius / 50.0f;  // Assuming default sphere is 50 units
		if (ForceFieldMesh) ForceFieldMesh->SetWorldScale3D(FVector(Scale));
	}

	UE_LOG(LogTemp, Log, TEXT("[ChaosForceField] BeginPlay - Radius: %.1f, PushForce: %.1f"), Radius, PushForce);
}

void ASLFChaosForceField::OnEffectTick_Implementation(float DeltaTime)
{
	Super::OnEffectTick_Implementation(DeltaTime);

	// Rotate the force field mesh
	if (ForceFieldMesh)
	{
		FRotator NewRotation = ForceFieldMesh->GetRelativeRotation();
		NewRotation.Yaw += RotationSpeed * DeltaTime;
		if (ForceFieldMesh) ForceFieldMesh->SetRelativeRotation(NewRotation);
	}

	// Apply damage and push to actors in field
	for (AActor* Actor : ActorsInField)
	{
		if (Actor && !Actor->IsPendingKillPending())
		{
			// Apply damage
			if (DamagePerSecond > 0.0f)
			{
				UGameplayStatics::ApplyDamage(Actor, DamagePerSecond * DeltaTime, nullptr, this, nullptr);
			}

			// Push away
			PushActorAway(Actor);
		}
	}
}

void ASLFChaosForceField::OnActorEnterField_Implementation(AActor* OtherActor)
{
	if (!OtherActor || OtherActor == OwnerActor)
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (Character)
	{
		ActorsInField.AddUnique(OtherActor);
		UE_LOG(LogTemp, Log, TEXT("[ChaosForceField] Actor entered: %s"), *OtherActor->GetName());
	}
}

void ASLFChaosForceField::PushActorAway_Implementation(AActor* ActorToPush)
{
	if (!ActorToPush)
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(ActorToPush);
	if (Character)
	{
		FVector Direction = (ActorToPush->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		FVector Force = Direction * PushForce;
		Character->LaunchCharacter(Force * GetWorld()->GetDeltaSeconds(), false, false);
	}
}
