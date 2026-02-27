// SLFLevelStreamManager.cpp
// Handles dungeon entrance/exit transitions with level streaming + screen fade.

#include "SLFLevelStreamManager.h"
#include "Components/BoxComponent.h"
#include "Engine/LevelStreamingDynamic.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ASLFLevelStreamManager::ASLFLevelStreamManager()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.25f; // Check 4 times per second

	// Root scene component
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// Trigger volume — kept for visualization but proximity check is primary
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	TriggerVolume->SetupAttachment(Root);
	TriggerVolume->SetBoxExtent(FVector(800.0f, 800.0f, 400.0f));
	TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerVolume->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	TriggerVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerVolume->SetGenerateOverlapEvents(true);

	DungeonDisplayName = TEXT("Unknown Dungeon");
}

void ASLFLevelStreamManager::BeginPlay()
{
	Super::BeginPlay();

	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ASLFLevelStreamManager::OnTriggerOverlap);

	// If this is an exit trigger, set the return location to our own position
	if (bIsDungeonExit)
	{
		OverworldReturnLocation = GetActorLocation();
	}

	UE_LOG(LogTemp, Warning, TEXT("[LevelStreamMgr] %s: %s at (%.0f, %.0f, %.0f) -> %s  TriggerExtent=(%.0f,%.0f,%.0f) OverlapEvents=%d"),
		bIsDungeonExit ? TEXT("EXIT") : TEXT("ENTRANCE"),
		*DungeonDisplayName, GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z,
		*DungeonLevelName,
		TriggerVolume->GetUnscaledBoxExtent().X, TriggerVolume->GetUnscaledBoxExtent().Y, TriggerVolume->GetUnscaledBoxExtent().Z,
		TriggerVolume->GetGenerateOverlapEvents() ? 1 : 0);
}

void ASLFLevelStreamManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bTransitionInProgress) return;

	// Proximity-based fallback: if overlap events fail, detect player by distance
	ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!PlayerChar) return;

	float Dist = FVector::Dist(GetActorLocation(), PlayerChar->GetActorLocation());
	if (Dist < 800.0f) // Within 8 meters
	{
		UE_LOG(LogTemp, Warning, TEXT("[LevelStreamMgr] Player within proximity (%.0f units) of %s — triggering enter"),
			Dist, *DungeonDisplayName);

		if (bIsDungeonExit)
		{
			ExitDungeon();
		}
		else
		{
			EnterDungeon();
		}
	}
}

void ASLFLevelStreamManager::OnTriggerOverlap(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (bTransitionInProgress) return;

	// Only respond to the player character
	ACharacter* PlayerChar = Cast<ACharacter>(OtherActor);
	if (!PlayerChar || !PlayerChar->IsPlayerControlled()) return;

	UE_LOG(LogTemp, Warning, TEXT("[LevelStreamMgr] Player entered %s trigger: %s"),
		bIsDungeonExit ? TEXT("exit") : TEXT("entrance"), *DungeonDisplayName);

	if (bIsDungeonExit)
	{
		ExitDungeon();
	}
	else
	{
		EnterDungeon();
	}
}

void ASLFLevelStreamManager::EnterDungeon()
{
	if (bTransitionInProgress || DungeonLevelName.IsEmpty()) return;
	bTransitionInProgress = true;

	// Cache player character reference NOW while we know it's valid
	CachedPlayerChar = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	UE_LOG(LogTemp, Warning, TEXT("[LevelStreamMgr] Entering dungeon: %s (PlayerChar=%s)"),
		*DungeonDisplayName, CachedPlayerChar ? *CachedPlayerChar->GetName() : TEXT("NULL"));

	// Calculate where the player will spawn inside the dungeon
	FVector DungeonSpawnPos = DungeonWorldOffset + DungeonSpawnOffset;
	PendingTeleportLocation = DungeonSpawnPos;

	// Fade screen to black
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC && PC->PlayerCameraManager)
	{
		PC->PlayerCameraManager->StartCameraFade(0.0f, 1.0f, FadeDuration, FLinearColor::Black, false, true);
	}

	// Stream in the dungeon level
	if (!bDungeonLoaded)
	{
		bool bSuccess = false;
		StreamedLevel = ULevelStreamingDynamic::LoadLevelInstance(
			GetWorld(),
			DungeonLevelName,
			DungeonWorldOffset,
			FRotator::ZeroRotator,
			bSuccess
		);

		if (bSuccess && StreamedLevel)
		{
			StreamedLevel->OnLevelLoaded.AddDynamic(this, &ASLFLevelStreamManager::OnDungeonLevelLoaded);
			bDungeonLoaded = true;
			UE_LOG(LogTemp, Warning, TEXT("[LevelStreamMgr] Dungeon level streaming started: %s"), *DungeonLevelName);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[LevelStreamMgr] Failed to stream dungeon: %s"), *DungeonLevelName);
			bTransitionInProgress = false;
		}
	}
	else
	{
		// Already loaded — just teleport
		OnDungeonLevelLoaded();
	}

	// Update save/load state
	{
		APlayerController* SavePC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (SavePC)
		{
			UActorComponent* SaveComp = SavePC->GetComponentByClass(
				LoadClass<UActorComponent>(nullptr, TEXT("/Script/SLFConversion.USaveLoadManagerComponent")));
			if (SaveComp)
			{
				FStrProperty* LevelNameProp = FindFProperty<FStrProperty>(SaveComp->GetClass(), TEXT("CurrentLevelName"));
				if (LevelNameProp) LevelNameProp->SetPropertyValue_InContainer(SaveComp, DungeonLevelName);
				FBoolProperty* InDungeonProp = FindFProperty<FBoolProperty>(SaveComp->GetClass(), TEXT("bIsInDungeon"));
				if (InDungeonProp) InDungeonProp->SetPropertyValue_InContainer(SaveComp, true);
				FStrProperty* DungeonNameProp = FindFProperty<FStrProperty>(SaveComp->GetClass(), TEXT("CurrentDungeonName"));
				if (DungeonNameProp) DungeonNameProp->SetPropertyValue_InContainer(SaveComp, DungeonDisplayName);
			}
		}
	}
}

void ASLFLevelStreamManager::ExitDungeon()
{
	if (bTransitionInProgress) return;
	bTransitionInProgress = true;

	UE_LOG(LogTemp, Warning, TEXT("[LevelStreamMgr] Exiting dungeon: %s"), *DungeonDisplayName);

	// Teleport player back to overworld
	ExecuteTransition(OverworldReturnLocation + FVector(0, 0, 100.0f));

	// Unload dungeon level
	if (StreamedLevel)
	{
		StreamedLevel->SetShouldBeLoaded(false);
		StreamedLevel->SetShouldBeVisible(false);
		bDungeonLoaded = false;
		StreamedLevel = nullptr;
		UE_LOG(LogTemp, Warning, TEXT("[LevelStreamMgr] Dungeon level unload requested"));
	}

	// Update save/load state
	{
		APlayerController* SavePC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (SavePC)
		{
			UActorComponent* SaveComp = SavePC->GetComponentByClass(
				LoadClass<UActorComponent>(nullptr, TEXT("/Script/SLFConversion.USaveLoadManagerComponent")));
			if (SaveComp)
			{
				FBoolProperty* InDungeonProp = FindFProperty<FBoolProperty>(SaveComp->GetClass(), TEXT("bIsInDungeon"));
				if (InDungeonProp) InDungeonProp->SetPropertyValue_InContainer(SaveComp, false);
			}
		}
	}
}

void ASLFLevelStreamManager::OnDungeonLevelLoaded()
{
	UE_LOG(LogTemp, Warning, TEXT("[LevelStreamMgr] Dungeon level loaded: %s — teleporting player now"), *DungeonLevelName);

	// Teleport the player using cached reference
	ACharacter* PlayerChar = CachedPlayerChar;
	if (!PlayerChar)
	{
		PlayerChar = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	}
	if (!PlayerChar)
	{
		// Last resort: get pawn from controller
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC) PlayerChar = Cast<ACharacter>(PC->GetPawn());
	}

	if (PlayerChar)
	{
		PlayerChar->SetActorLocation(PendingTeleportLocation);
		UE_LOG(LogTemp, Warning, TEXT("[LevelStreamMgr] Teleported %s to (%.0f, %.0f, %.0f)"),
			*PlayerChar->GetName(),
			PendingTeleportLocation.X, PendingTeleportLocation.Y, PendingTeleportLocation.Z);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[LevelStreamMgr] NO PLAYER CHARACTER FOUND — cannot teleport!"));
	}

	// Fade back in after a short delay
	GetWorldTimerManager().SetTimer(FadeTimerHandle, this,
		&ASLFLevelStreamManager::OnFadeInComplete, 0.5f, false);
}

void ASLFLevelStreamManager::ExecuteTransition(FVector TargetLocation)
{
	// Legacy path — not used in new flow but kept for ExitDungeon
	PendingTeleportLocation = TargetLocation;

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC && PC->PlayerCameraManager)
	{
		PC->PlayerCameraManager->StartCameraFade(0.0f, 1.0f, FadeDuration, FLinearColor::Black, false, true);
	}

	GetWorldTimerManager().SetTimer(FadeTimerHandle, this,
		&ASLFLevelStreamManager::OnFadeOutComplete, FadeDuration, false);
}

void ASLFLevelStreamManager::OnFadeOutComplete()
{
	ACharacter* PlayerChar = CachedPlayerChar;
	if (!PlayerChar) PlayerChar = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!PlayerChar)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC) PlayerChar = Cast<ACharacter>(PC->GetPawn());
	}

	if (PlayerChar)
	{
		PlayerChar->SetActorLocation(PendingTeleportLocation);
		UE_LOG(LogTemp, Warning, TEXT("[LevelStreamMgr] Teleported %s to (%.0f, %.0f, %.0f)"),
			*PlayerChar->GetName(),
			PendingTeleportLocation.X, PendingTeleportLocation.Y, PendingTeleportLocation.Z);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[LevelStreamMgr] OnFadeOutComplete: NO PLAYER — cannot teleport!"));
	}

	if (bDungeonLoaded || bIsDungeonExit)
	{
		GetWorldTimerManager().SetTimer(FadeTimerHandle, this,
			&ASLFLevelStreamManager::OnFadeInComplete, 0.3f, false);
	}
}

void ASLFLevelStreamManager::OnFadeInComplete()
{
	// Fade back in
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC && PC->PlayerCameraManager)
	{
		PC->PlayerCameraManager->StartCameraFade(1.0f, 0.0f, FadeDuration, FLinearColor::Black, false, false);
	}

	bTransitionInProgress = false;
	UE_LOG(LogTemp, Warning, TEXT("[LevelStreamMgr] Transition complete"));
}
