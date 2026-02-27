// SLFBossEncounter.cpp

#include "Blueprints/Actors/SLFBossEncounter.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SLFZoneManagerComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

ASLFBossEncounter::ASLFBossEncounter()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// Fog gate trigger
	FogGateTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("FogGateTrigger"));
	FogGateTrigger->SetupAttachment(Root);
	FogGateTrigger->SetBoxExtent(FVector(200, 400, 300));
	FogGateTrigger->SetCollisionProfileName(TEXT("Trigger"));
	FogGateTrigger->SetGenerateOverlapEvents(true);

	// Fog gate visual (placeholder — replace with VFX in editor)
	FogGateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FogGateMesh"));
	FogGateMesh->SetupAttachment(Root);
	FogGateMesh->SetHiddenInGame(true); // Hidden until VFX assigned
	FogGateMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Exit blocker (blocks player from leaving arena during fight)
	ExitBlocker = CreateDefaultSubobject<UBoxComponent>(TEXT("ExitBlocker"));
	ExitBlocker->SetupAttachment(Root);
	ExitBlocker->SetBoxExtent(FVector(50, 400, 300));
	ExitBlocker->SetCollisionProfileName(TEXT("BlockAll"));
	ExitBlocker->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Disabled until fight starts
}

void ASLFBossEncounter::BeginPlay()
{
	Super::BeginPlay();

	if (ArenaCenter.IsZero())
	{
		ArenaCenter = GetActorLocation() + BossSpawnOffset;
	}

	CheckAlreadyDefeated();

	if (!bAlreadyDefeated)
	{
		FogGateTrigger->OnComponentBeginOverlap.AddDynamic(this, &ASLFBossEncounter::OnFogGateOverlap);
	}
	else
	{
		// Boss already dead — hide fog gate
		FogGateMesh->SetHiddenInGame(true);
		FogGateTrigger->SetGenerateOverlapEvents(false);
	}
}

void ASLFBossEncounter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Monitor boss health for phase transitions during active encounter
	if (bEncounterActive && BossActor)
	{
		// Phase transition check is driven externally by boss health updates
		// Tick is used for arena boundary enforcement
	}
}

void ASLFBossEncounter::StartEncounter()
{
	if (bEncounterActive || bAlreadyDefeated) return;

	ACharacter* Boss = SpawnBoss();
	if (!Boss)
	{
		UE_LOG(LogTemp, Error, TEXT("BossEncounter [%s]: Failed to spawn boss"), *GetName());
		return;
	}

	bEncounterActive = true;
	CurrentPhase = ESLFBossPhase::Phase1;
	SetActorTickEnabled(true);

	if (bBlockExitDuringFight)
	{
		SetFogGateActive(true);
	}

	OnEncounterStarted.Broadcast();
	OnPhaseChanged.Broadcast(ESLFBossPhase::Phase1);

	UE_LOG(LogTemp, Log, TEXT("BossEncounter [%s]: Started — %s"),
		*GetName(), *BossConfig.BossName.ToString());
}

void ASLFBossEncounter::OnBossDefeated()
{
	if (!bEncounterActive) return;

	bEncounterActive = false;
	bAlreadyDefeated = true;
	CurrentPhase = ESLFBossPhase::NotBoss;
	SetActorTickEnabled(false);

	// Remove fog gate
	SetFogGateActive(false);
	FogGateMesh->SetHiddenInGame(true);
	FogGateTrigger->SetGenerateOverlapEvents(false);

	// Notify zone manager
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		USLFZoneManagerComponent* ZoneMgr = PC->FindComponentByClass<USLFZoneManagerComponent>();
		if (ZoneMgr && BossConfig.DefeatProgressTag.IsValid())
		{
			ZoneMgr->DefeatBoss(BossConfig.DefeatProgressTag);
		}
	}

	OnEncounterVictory.Broadcast();
	OnPhaseChanged.Broadcast(ESLFBossPhase::NotBoss);

	UE_LOG(LogTemp, Log, TEXT("BossEncounter [%s]: Boss defeated — %s"),
		*GetName(), *BossConfig.BossName.ToString());
}

void ASLFBossEncounter::TransitionToPhase(ESLFBossPhase NewPhase)
{
	if (CurrentPhase == NewPhase) return;

	ESLFBossPhase OldPhase = CurrentPhase;
	CurrentPhase = NewPhase;
	OnPhaseChanged.Broadcast(NewPhase);

	UE_LOG(LogTemp, Log, TEXT("BossEncounter [%s]: Phase %d -> %d"),
		*GetName(), (int32)OldPhase, (int32)NewPhase);
}

void ASLFBossEncounter::CheckPhaseTransition(float CurrentHealthPercent)
{
	if (!bEncounterActive) return;

	// Check thresholds in reverse order (highest phase first)
	for (int32 i = BossConfig.PhaseThresholds.Num() - 1; i >= 0; --i)
	{
		ESLFBossPhase TargetPhase = static_cast<ESLFBossPhase>(i + 2); // Phase2=2, Phase3=3
		if (CurrentHealthPercent <= BossConfig.PhaseThresholds[i] && CurrentPhase < TargetPhase)
		{
			TransitionToPhase(TargetPhase);
			break;
		}
	}

	// Check for defeat
	if (CurrentHealthPercent <= 0.0f)
	{
		OnBossDefeated();
	}
}

void ASLFBossEncounter::ResetEncounter()
{
	if (BossActor)
	{
		BossActor->OnDestroyed.RemoveDynamic(this, &ASLFBossEncounter::HandleBossDeath);
		BossActor->Destroy();
		BossActor = nullptr;
	}

	bEncounterActive = false;
	CurrentPhase = ESLFBossPhase::NotBoss;
	SetActorTickEnabled(false);
	SetFogGateActive(false);

	// Re-enable trigger for next attempt
	if (!bAlreadyDefeated)
	{
		FogGateTrigger->SetGenerateOverlapEvents(true);
	}
}

void ASLFBossEncounter::SetFogGateActive(bool bActive)
{
	if (ExitBlocker)
	{
		ExitBlocker->SetCollisionEnabled(bActive ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
	}
}

void ASLFBossEncounter::OnFogGateOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* PlayerChar = Cast<ACharacter>(OtherActor);
	if (!PlayerChar) return;

	APlayerController* PC = Cast<APlayerController>(PlayerChar->GetController());
	if (!PC) return;

	// Only trigger for the local player
	if (!bEncounterActive && !bAlreadyDefeated)
	{
		StartEncounter();
	}
}

ACharacter* ASLFBossEncounter::SpawnBoss()
{
	if (!BossConfig.BossClass || !GetWorld()) return nullptr;

	FVector SpawnLoc = GetActorLocation() + BossSpawnOffset;
	FRotator SpawnRot = GetActorRotation();
	SpawnRot.Yaw += 180.0f; // Face toward entrance

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ACharacter* Boss = GetWorld()->SpawnActor<ACharacter>(BossConfig.BossClass, SpawnLoc, SpawnRot, SpawnParams);
	if (Boss)
	{
		BossActor = Boss;
		Boss->OnDestroyed.AddDynamic(this, &ASLFBossEncounter::HandleBossDeath);

		UE_LOG(LogTemp, Log, TEXT("BossEncounter: Spawned boss %s at %s"),
			*BossConfig.BossClass->GetName(), *SpawnLoc.ToString());
	}

	return Boss;
}

void ASLFBossEncounter::HandleBossDeath(AActor* DeadActor)
{
	if (DeadActor == BossActor)
	{
		OnBossDefeated();
	}
}

void ASLFBossEncounter::CheckAlreadyDefeated()
{
	if (!BossConfig.DefeatProgressTag.IsValid()) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;

	USLFZoneManagerComponent* ZoneMgr = PC->FindComponentByClass<USLFZoneManagerComponent>();
	if (ZoneMgr && ZoneMgr->IsBossDefeated(BossConfig.DefeatProgressTag))
	{
		bAlreadyDefeated = true;
	}
}
