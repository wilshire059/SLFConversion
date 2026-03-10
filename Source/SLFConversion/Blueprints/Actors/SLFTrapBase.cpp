// SLFTrapBase.cpp
#include "SLFTrapBase.h"
#include "GameFramework/Character.h"
#include "Engine/DamageEvents.h"

ASLFTrapBase::ASLFTrapBase()
{
	PrimaryActorTick.bCanEverTick = false;

	TrapMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrapMesh"));
	RootComponent = TrapMesh;

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	TriggerVolume->SetupAttachment(RootComponent);
	TriggerVolume->SetBoxExtent(FVector(100.0f, 100.0f, 50.0f));
	TriggerVolume->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	TriggerVolume->SetGenerateOverlapEvents(true);
}

void ASLFTrapBase::BeginPlay()
{
	Super::BeginPlay();

	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ASLFTrapBase::OnTriggerOverlap);
}

void ASLFTrapBase::OnTriggerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bIsActive) return;

	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (Character)
	{
		ActivateTrap(Character);
	}
}

void ASLFTrapBase::ActivateTrap_Implementation(AActor* Victim)
{
	if (!bIsActive || !Victim) return;

	UE_LOG(LogTemp, Log, TEXT("[Trap] %s activated on %s (Type: %d, Damage: %.0f)"),
		*GetName(), *Victim->GetName(), (int32)TrapType, Damage);

	// Apply damage
	FDamageEvent DamageEvent;
	Victim->TakeDamage(Damage, DamageEvent, nullptr, this);

	bIsActive = false;

	if (bResets)
	{
		GetWorld()->GetTimerManager().SetTimer(ResetTimerHandle, this,
			&ASLFTrapBase::ResetTrap, ResetDelay, false);
	}
}

void ASLFTrapBase::ResetTrap_Implementation()
{
	bIsActive = true;
	UE_LOG(LogTemp, Log, TEXT("[Trap] %s reset"), *GetName());
}
