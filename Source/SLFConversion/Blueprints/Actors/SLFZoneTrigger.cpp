// SLFZoneTrigger.cpp

#include "Blueprints/Actors/SLFZoneTrigger.h"
#include "Components/BoxComponent.h"
#include "Components/SLFZoneManagerComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

ASLFZoneTrigger::ASLFZoneTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	TriggerVolume->SetupAttachment(Root);
	TriggerVolume->SetBoxExtent(FVector(500, 500, 300));
	TriggerVolume->SetCollisionProfileName(TEXT("Trigger"));
	TriggerVolume->SetGenerateOverlapEvents(true);
}

void ASLFZoneTrigger::BeginPlay()
{
	Super::BeginPlay();
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ASLFZoneTrigger::OnPlayerEnterZone);
}

void ASLFZoneTrigger::OnPlayerEnterZone(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* PlayerChar = Cast<ACharacter>(OtherActor);
	if (!PlayerChar) return;

	APlayerController* PC = Cast<APlayerController>(PlayerChar->GetController());
	if (!PC) return;

	// Update zone manager
	USLFZoneManagerComponent* ZoneMgr = PC->FindComponentByClass<USLFZoneManagerComponent>();
	if (ZoneMgr && ZoneTag.IsValid())
	{
		ZoneMgr->SetCurrentZone(ZoneTag);
	}

	UE_LOG(LogTemp, Log, TEXT("ZoneTrigger: Player entered %s"), *ZoneTag.ToString());
}
