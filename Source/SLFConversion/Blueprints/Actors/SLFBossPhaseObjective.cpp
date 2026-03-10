// SLFBossPhaseObjective.cpp
#include "SLFBossPhaseObjective.h"

ASLFBossPhaseObjective::ASLFBossPhaseObjective()
{
	PrimaryActorTick.bCanEverTick = false;

	ObjectiveMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObjectiveMesh"));
	RootComponent = ObjectiveMesh;

	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(RootComponent);
	InteractionSphere->SetSphereRadius(150.0f);
	InteractionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

void ASLFBossPhaseObjective::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
}

float ASLFBossPhaseObjective::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDestroyed) return 0.0f;

	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	Health -= ActualDamage;

	UE_LOG(LogTemp, Log, TEXT("[BossObjective] %s took %.0f damage (%.0f/%.0f HP)"),
		*GetName(), ActualDamage, Health, MaxHealth);

	if (Health <= 0.0f)
	{
		DestroyObjective();
	}

	return ActualDamage;
}

void ASLFBossPhaseObjective::DestroyObjective()
{
	if (bIsDestroyed) return;

	bIsDestroyed = true;
	UE_LOG(LogTemp, Warning, TEXT("[BossObjective] %s DESTROYED"), *GetName());

	OnObjectiveDestroyed.Broadcast(this);

	// Hide mesh
	if (ObjectiveMesh)
	{
		ObjectiveMesh->SetVisibility(false);
		ObjectiveMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}
