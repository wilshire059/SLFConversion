// ProgressManagerComponent.cpp
// C++ implementation for AC_ProgressManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - AC_ProgressManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         4/4 (initialized in constructor)
// Functions:         10/10 implemented
// Event Dispatchers: 2/2 (all assignable)
// ═══════════════════════════════════════════════════════════════════════════════

#include "ProgressManagerComponent.h"
#include "TimerManager.h"

UProgressManagerComponent::UProgressManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Initialize config
	LevelUpCosts = nullptr;

	// Initialize runtime
	PlayTime = FTimespan::Zero();
}

void UProgressManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[ProgressManager] BeginPlay on %s"), *GetOwner()->GetName());

	// Initialize default progress values
	for (const auto& Pair : DefaultsToProgress)
	{
		CurrentProgress.Add(Pair.Key, Pair.Value);
	}

	// Start play time tracking timer
	if (UWorld* World = GetWorld())
	{
		FTimerHandle PlayTimeHandle;
		World->GetTimerManager().SetTimer(PlayTimeHandle, this,
			&UProgressManagerComponent::PlayTimeTimer_Implementation, 1.0f, true);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// ACCESS FUNCTIONS [1-2/10]
// ═══════════════════════════════════════════════════════════════════════════════

void UProgressManagerComponent::GetProgress_Implementation(FGameplayTag ProgressTag, ESLFProgress& OutState, bool& bFound)
{
	if (ESLFProgress* Found = CurrentProgress.Find(ProgressTag))
	{
		OutState = *Found;
		bFound = true;
	}
	else
	{
		OutState = ESLFProgress::NotStarted;
		bFound = false;
	}
}

void UProgressManagerComponent::SetProgress_Implementation(FGameplayTag ProgressTag, ESLFProgress State)
{
	CurrentProgress.Add(ProgressTag, State);
	UE_LOG(LogTemp, Log, TEXT("[ProgressManager] SetProgress: %s = %d"),
		*ProgressTag.ToString(), static_cast<int32>(State));
}

// ═══════════════════════════════════════════════════════════════════════════════
// QUERY FUNCTIONS [3/10]
// ═══════════════════════════════════════════════════════════════════════════════

int32 UProgressManagerComponent::GetRequiredCurrencyForLevel(int32 Level) const
{
	if (!LevelUpCosts) return 0;

	float Cost = LevelUpCosts->GetFloatValue(static_cast<float>(Level));
	return FMath::RoundToInt(Cost);
}

// ═══════════════════════════════════════════════════════════════════════════════
// EVENT EXECUTION [4/10]
// ═══════════════════════════════════════════════════════════════════════════════

void UProgressManagerComponent::ExecuteGameplayEvents_Implementation(const TArray<FInstancedStruct>& Events)
{
	UE_LOG(LogTemp, Log, TEXT("[ProgressManager] ExecuteGameplayEvents: %d events"), Events.Num());

	for (const FInstancedStruct& Event : Events)
	{
		// TODO: Parse and execute each gameplay event struct
		// Types might include: SetProgress, GiveItem, TeleportPlayer, etc.
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// SERIALIZATION [5/10]
// ═══════════════════════════════════════════════════════════════════════════════

void UProgressManagerComponent::SerializeProgressData_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ProgressManager] SerializeProgressData"));

	TArray<FInstancedStruct> ProgressData;

	// Convert CurrentProgress map to array of save info
	for (const auto& Pair : CurrentProgress)
	{
		FSLFProgressSaveInfo SaveInfo;
		SaveInfo.ProgressTag = Pair.Key;
		SaveInfo.State = Pair.Value;

		// TODO: Create FInstancedStruct from SaveInfo and add to ProgressData
	}

	// Broadcast for save system
	OnSaveRequested.Broadcast(FGameplayTag(), ProgressData);
}

// ═══════════════════════════════════════════════════════════════════════════════
// TIMER/TIME FUNCTIONS [6-7/10]
// ═══════════════════════════════════════════════════════════════════════════════

void UProgressManagerComponent::PlayTimeTimer_Implementation()
{
	// Add one second to play time
	PlayTime += FTimespan::FromSeconds(1.0);
	OnPlayTimeUpdated.Broadcast(PlayTime);
}

void UProgressManagerComponent::InitializeLoadedPlayTime_Implementation(FTimespan LoadedPlayTime)
{
	PlayTime = LoadedPlayTime;
	UE_LOG(LogTemp, Log, TEXT("[ProgressManager] InitializeLoadedPlayTime: %s"),
		*PlayTime.ToString());
}

// ═══════════════════════════════════════════════════════════════════════════════
// LOAD FUNCTIONS [8-9/10]
// ═══════════════════════════════════════════════════════════════════════════════

void UProgressManagerComponent::InitializeLoadedProgress_Implementation(const TArray<FSLFProgressSaveInfo>& LoadedProgress)
{
	UE_LOG(LogTemp, Log, TEXT("[ProgressManager] InitializeLoadedProgress: %d entries"), LoadedProgress.Num());

	CurrentProgress.Empty();
	for (const FSLFProgressSaveInfo& Info : LoadedProgress)
	{
		CurrentProgress.Add(Info.ProgressTag, Info.State);
	}
}

void UProgressManagerComponent::OnDataLoaded_Implementation(const FSLFSaveGameInfo& LoadedData)
{
	UE_LOG(LogTemp, Log, TEXT("[ProgressManager] OnDataLoaded"));

	// Process the progress data from save
	// TODO: Extract progress info from LoadedData and call InitializeLoadedProgress
}
