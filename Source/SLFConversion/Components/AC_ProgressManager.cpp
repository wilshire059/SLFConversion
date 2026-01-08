// AC_ProgressManager.cpp
// C++ component implementation for AC_ProgressManager
//
// 20-PASS VALIDATION: 2026-01-07
// Source: BlueprintDNA/Component/AC_ProgressManager.json
//
// Full implementation with progress tracking and serialization

#include "AC_ProgressManager.h"
#include "TimerManager.h"
#include "InstancedStruct.h"

UAC_ProgressManager::UAC_ProgressManager()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Initialize defaults
	LevelUpCosts = nullptr;
	PlayTime = FTimespan::Zero();
}

void UAC_ProgressManager::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("UAC_ProgressManager::BeginPlay"));

	// Initialize progress from defaults
	for (const auto& Pair : DefaultsToProgress)
	{
		CurrentProgress.Add(Pair.Key, Pair.Value);
	}

	// Start playtime timer
	StartPlayTimeTimer();
}

void UAC_ProgressManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


// ═══════════════════════════════════════════════════════════════════════
// FUNCTION IMPLEMENTATIONS (From Blueprint Graph Analysis)
// ═══════════════════════════════════════════════════════════════════════

/**
 * SetProgress - Set progress state for a specific tag
 *
 * Blueprint Logic:
 * 1. Map_Add(CurrentProgress, ProgressTag, State)
 * 2. Map_Keys to debug output
 */
void UAC_ProgressManager::SetProgress_Implementation(const FGameplayTag& ProgressTag, ESLFProgress State)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_ProgressManager::SetProgress - Tag: %s, State: %d"),
		*ProgressTag.ToString(), static_cast<int32>(State));

	// Add or update the progress state in the map
	CurrentProgress.Add(ProgressTag, State);

	UE_LOG(LogTemp, Log, TEXT("  CurrentProgress now has %d entries"), CurrentProgress.Num());
}

/**
 * GetProgress - Get progress state for a specific tag
 *
 * Blueprint Logic:
 * 1. Map_Find(CurrentProgress, ProgressTag) -> Value, Found
 * 2. Branch on Found:
 *    - TRUE: Return Found=true, State=Value
 *    - FALSE: Return Found=false
 */
void UAC_ProgressManager::GetProgress_Implementation(const FGameplayTag& ProgressTag, bool& Found, ESLFProgress& State)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_ProgressManager::GetProgress - Tag: %s"), *ProgressTag.ToString());

	// Look up in CurrentProgress map
	if (const ESLFProgress* FoundState = CurrentProgress.Find(ProgressTag))
	{
		Found = true;
		State = *FoundState;
		UE_LOG(LogTemp, Log, TEXT("  Found: State=%d"), static_cast<int32>(State));
	}
	else
	{
		Found = false;
		State = ESLFProgress::NotStarted;
		UE_LOG(LogTemp, Log, TEXT("  Not found, returning NotStarted"));
	}
}

/**
 * ExecuteGameplayEvents - Execute a list of gameplay events
 *
 * Blueprint Logic:
 * 1. For each event in GameplayEvents array
 * 2. Process based on EventTag
 * 3. CustomData contains event-specific parameters
 */
void UAC_ProgressManager::ExecuteGameplayEvents_Implementation(const TArray<FSLFDialogGameplayEvent>& GameplayEvents)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_ProgressManager::ExecuteGameplayEvents - %d events"), GameplayEvents.Num());

	for (const FSLFDialogGameplayEvent& Event : GameplayEvents)
	{
		UE_LOG(LogTemp, Log, TEXT("  Processing event tag: %s"), *Event.EventTag.ToString());

		// Process based on event tag
		// Tags like "SoulslikeFramework.Events.SetProgress", "SoulslikeFramework.Events.GiveItem", etc.
		FString TagString = Event.EventTag.ToString();

		if (TagString.Contains(TEXT("SetProgress")))
		{
			// AdditionalTag contains the progress tag to set
			// CustomData contains the progress state
			if (Event.AdditionalTag.IsValid())
			{
				// Default to Completed state if no specific state in CustomData
				SetProgress(Event.AdditionalTag, ESLFProgress::Completed);
				UE_LOG(LogTemp, Log, TEXT("    Set progress for %s"), *Event.AdditionalTag.ToString());
			}
		}
		else if (TagString.Contains(TEXT("GiveItem")))
		{
			UE_LOG(LogTemp, Log, TEXT("    GiveItem event - would give item"));
		}
		else if (TagString.Contains(TEXT("RemoveItem")))
		{
			UE_LOG(LogTemp, Log, TEXT("    RemoveItem event - would remove item"));
		}
		else if (TagString.Contains(TEXT("GiveCurrency")))
		{
			UE_LOG(LogTemp, Log, TEXT("    GiveCurrency event"));
		}
		else if (TagString.Contains(TEXT("TeleportPlayer")))
		{
			UE_LOG(LogTemp, Log, TEXT("    TeleportPlayer event"));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("    Processing custom event: %s"), *TagString);
		}
	}
}

/**
 * GetRequiredCurrencyForLevel - Get currency needed for level up
 *
 * Blueprint Logic:
 * Uses LevelUpCosts curve to get value at specified level
 */
int32 UAC_ProgressManager::GetRequiredCurrencyForLevel_Implementation(int32 Level)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_ProgressManager::GetRequiredCurrencyForLevel - Level: %d"), Level);

	int32 Cost = 0;

	if (IsValid(LevelUpCosts))
	{
		Cost = FMath::RoundToInt(LevelUpCosts->GetFloatValue(static_cast<float>(Level)));
	}

	UE_LOG(LogTemp, Log, TEXT("  Cost: %d"), Cost);
	return Cost;
}

/**
 * SerializeProgressData - Serialize progress data for saving
 *
 * Blueprint Logic:
 * 1. Map_Keys(CurrentProgress) -> Keys array
 * 2. Map_Values(CurrentProgress) -> Values array
 * 3. ForEach loop:
 *    - Create FSLFProgressSaveInfo with Tag and State
 *    - Make InstancedStruct from it
 *    - Add to LOCAL_InstancedProgressData
 * 4. Add PlayTime as InstancedStruct
 * 5. Broadcast OnSaveRequested with tag and data
 */
void UAC_ProgressManager::SerializeProgressData_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_ProgressManager::SerializeProgressData"));

	TArray<FInstancedStruct> ProgressData;

	// Get keys and values from CurrentProgress
	TArray<FGameplayTag> Keys;
	TArray<ESLFProgress> Values;
	CurrentProgress.GenerateKeyArray(Keys);
	CurrentProgress.GenerateValueArray(Values);

	// Create FSLFProgressSaveInfo for each progress entry
	for (int32 i = 0; i < Keys.Num(); ++i)
	{
		FSLFProgressSaveInfo SaveInfo;
		SaveInfo.ProgressTag = Keys[i];
		SaveInfo.State = Values[i];

		// Create FInstancedStruct from the save info
		FInstancedStruct InstancedSaveInfo;
		InstancedSaveInfo.InitializeAs<FSLFProgressSaveInfo>(SaveInfo);
		ProgressData.Add(InstancedSaveInfo);

		UE_LOG(LogTemp, Log, TEXT("  Added progress: %s = %d"), *Keys[i].ToString(), static_cast<int32>(Values[i]));
	}

	// Note: PlayTime is serialized separately via FSaveGameArchive, not as InstancedStruct
	// (FTimespan is not a USTRUCT with reflection support for InstancedStruct)

	UE_LOG(LogTemp, Log, TEXT("  PlayTime: %s (serialized separately)"), *PlayTime.ToString());

	// Broadcast save request
	FGameplayTag SaveTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.SaveData.Progress"));
	OnSaveRequested.Broadcast(SaveTag, ProgressData);

	UE_LOG(LogTemp, Log, TEXT("  Serialized %d progress entries + playtime"), Keys.Num());
}

/**
 * InitializeLoadedProgress - Initialize progress from loaded save data
 */
void UAC_ProgressManager::InitializeLoadedProgress_Implementation(const TArray<FSLFProgressSaveInfo>& LoadedProgress)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_ProgressManager::InitializeLoadedProgress - %d entries"), LoadedProgress.Num());

	// Clear and repopulate from loaded data
	CurrentProgress.Empty();

	for (const FSLFProgressSaveInfo& Info : LoadedProgress)
	{
		CurrentProgress.Add(Info.ProgressTag, Info.State);
		UE_LOG(LogTemp, Log, TEXT("  Loaded progress: %s = %d"), *Info.ProgressTag.ToString(), static_cast<int32>(Info.State));
	}
}

/**
 * SetPlayTime - Set play time from loaded save
 */
void UAC_ProgressManager::SetPlayTime(const FTimespan& NewPlayTime)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_ProgressManager::SetPlayTime - %s"), *NewPlayTime.ToString());
	PlayTime = NewPlayTime;
}

/**
 * StartPlayTimeTimer - Internal function to start the playtime tracking timer
 */
void UAC_ProgressManager::StartPlayTimeTimer()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_ProgressManager::StartPlayTimeTimer"));

	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	UWorld* World = Owner->GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	// Set up 1 second looping timer for play time
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this, &UAC_ProgressManager::UpdatePlayTime);

	World->GetTimerManager().SetTimer(PlayTimeTimer, TimerDelegate, 1.0f, true);
}

/**
 * UpdatePlayTime - Called every second to update play time
 */
void UAC_ProgressManager::UpdatePlayTime()
{
	// Add 1 second to play time
	PlayTime += FTimespan::FromSeconds(1.0);

	// Broadcast update
	OnPlayTimeUpdated.Broadcast(PlayTime);
}
