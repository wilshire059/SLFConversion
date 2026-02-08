// W_Settings_PlayerCard.cpp
// C++ Widget implementation for W_Settings_PlayerCard
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session

#include "Widgets/W_Settings_PlayerCard.h"
#include "Components/TextBlock.h"
#include "Components/StatManagerComponent.h"
#include "Components/ProgressManagerComponent.h"
#include "Kismet/GameplayStatics.h"

UW_Settings_PlayerCard::UW_Settings_PlayerCard(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, PlayerLevel(1)
	, LevelText(nullptr)
	, PlayTimeText(nullptr)
	, ZoneNameText(nullptr)
	, PlayerClassText(nullptr)
{
}

void UW_Settings_PlayerCard::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	// CRITICAL: Reset PlayerLevel AFTER Super::NativeConstruct().
	// The Blueprint's Event Construct graph (still active) runs inside Super and reads
	// Level from the pawn's StatManager Blueprint component (UAC_StatManager with CDO Level=999).
	// It sets PlayerLevel=999 and LevelText="999". We must override that here.
	PlayerLevel = 1;

	// Get initial level from player pawn's StatManager
	APawn* PlayerPawn = GetOwningPlayerPawn();
	if (PlayerPawn)
	{
		UStatManagerComponent* StatManager = PlayerPawn->FindComponentByClass<UStatManagerComponent>();
		if (StatManager)
		{
			PlayerLevel = StatManager->Level;
			StatManager->OnLevelUpdated.AddDynamic(this, &UW_Settings_PlayerCard::EventOnLevelUpdated);
			UE_LOG(LogTemp, Log, TEXT("[W_Settings_PlayerCard] Bound to UStatManagerComponent, level: %d"), PlayerLevel);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[W_Settings_PlayerCard] No UStatManagerComponent on pawn, using default level: %d"), PlayerLevel);
		}

	}

	// Bind to ProgressManager for play time updates
	// NOTE: ProgressManager is on the PlayerController, NOT the Pawn
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		UProgressManagerComponent* ProgressManager = PC->FindComponentByClass<UProgressManagerComponent>();
		if (ProgressManager)
		{
			ProgressManager->OnPlayTimeUpdated.AddDynamic(this, &UW_Settings_PlayerCard::EventOnPlayTimeUpdated);
			UE_LOG(LogTemp, Log, TEXT("[W_Settings_PlayerCard] Bound to UProgressManagerComponent on PC for PlayTime"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_Settings_PlayerCard] No UProgressManagerComponent on PlayerController"));
		}
	}

	// CRITICAL: Always set the text here to override whatever the Blueprint Construct set
	if (LevelText)
	{
		LevelText->SetText(FText::AsNumber(PlayerLevel));
		UE_LOG(LogTemp, Log, TEXT("[W_Settings_PlayerCard] Set LevelText to: %d"), PlayerLevel);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Settings_PlayerCard] LevelText widget is NULL"));
	}

	UE_LOG(LogTemp, Log, TEXT("[W_Settings_PlayerCard] NativeConstruct complete, PlayerLevel=%d"), PlayerLevel);
}

void UW_Settings_PlayerCard::NativeDestruct()
{
	// Unbind from StatManager (on Pawn)
	APawn* PlayerPawn = GetOwningPlayerPawn();
	if (PlayerPawn)
	{
		UStatManagerComponent* StatManager = PlayerPawn->FindComponentByClass<UStatManagerComponent>();
		if (StatManager)
		{
			StatManager->OnLevelUpdated.RemoveDynamic(this, &UW_Settings_PlayerCard::EventOnLevelUpdated);
		}
	}

	// Unbind from ProgressManager (on PlayerController)
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		UProgressManagerComponent* ProgressManager = PC->FindComponentByClass<UProgressManagerComponent>();
		if (ProgressManager)
		{
			ProgressManager->OnPlayTimeUpdated.RemoveDynamic(this, &UW_Settings_PlayerCard::EventOnPlayTimeUpdated);
		}
	}

	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("[W_Settings_PlayerCard] NativeDestruct"));
}

void UW_Settings_PlayerCard::CacheWidgetReferences()
{
	// BindWidgetOptional handles this automatically, but log for debugging
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_PlayerCard] Widget bindings - LevelText: %s, PlayTimeText: %s"),
		LevelText ? TEXT("OK") : TEXT("NULL"),
		PlayTimeText ? TEXT("OK") : TEXT("NULL"));
}

void UW_Settings_PlayerCard::EventOnLevelUpdated_Implementation(int32 NewLevel)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_PlayerCard] EventOnLevelUpdated: %d"), NewLevel);

	// Update local variable
	PlayerLevel = NewLevel;

	// Update LevelText widget (bp_only logic: Set PlayerLevel, then SetText on LevelText)
	if (LevelText)
	{
		LevelText->SetText(FText::AsNumber(PlayerLevel));
		UE_LOG(LogTemp, Log, TEXT("[W_Settings_PlayerCard] Updated LevelText to: %d"), PlayerLevel);
	}
}

void UW_Settings_PlayerCard::EventOnPlayTimeUpdated_Implementation(FTimespan NewTime)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_PlayerCard] EventOnPlayTimeUpdated"));

	// Update PlayTimeText widget with formatted time
	if (PlayTimeText)
	{
		// Format as HH:MM:SS
		FString TimeString = FString::Printf(TEXT("%02d:%02d:%02d"),
			FMath::FloorToInt(NewTime.GetTotalHours()),
			NewTime.GetMinutes(),
			NewTime.GetSeconds());
		PlayTimeText->SetText(FText::FromString(TimeString));
	}
}
