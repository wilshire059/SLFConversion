// SaveLoadManagerComponent.cpp
// C++ implementation for AC_SaveLoadManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - AC_SaveLoadManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         13/13 (initialized in constructor)
// Functions:         23/23 implemented
// Event Dispatchers: 2/2 (all assignable)
// ═══════════════════════════════════════════════════════════════════════════════

#include "SaveLoadManagerComponent.h"
#include "StatManagerComponent.h"
#include "ProgressManagerComponent.h"
#include "InventoryManagerComponent.h"
#include "EquipmentManagerComponent.h"
#include "Blueprints/SG_SoulslikeFramework.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

USaveLoadManagerComponent::USaveLoadManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Initialize config
	AutosaveTimer = 300.0f; // 5 minutes

	// Initialize runtime
	CurrentSaveSlot = TEXT("");
	bAutoSaveNeeded = false;
	bCanResave = true;

	// Initialize component references
	StatManager = nullptr;
	InventoryManager = nullptr;
	EquipmentManager = nullptr;
	ProgressManager = nullptr;
	SGO_Character = nullptr;
	SGO_Slots = nullptr;
}

void USaveLoadManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] BeginPlay on %s"), *GetOwner()->GetName());

	Initialize();
}

// ═══════════════════════════════════════════════════════════════════════════════
// SAVE OPERATIONS [1-6/23]
// ═══════════════════════════════════════════════════════════════════════════════

void USaveLoadManagerComponent::SaveToSlot_Implementation(const FString& SlotName)
{
	UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] SaveToSlot: %s"), *SlotName);

	CurrentSaveSlot = SlotName;
	SerializeAllData();

	// Save the game
	if (SGO_Character)
	{
		UGameplayStatics::SaveGameToSlot(SGO_Character, SlotName, 0);
		OnSaveCompleted.Broadcast();
	}

	bAutoSaveNeeded = false;
}

void USaveLoadManagerComponent::SaveToCheckpoint_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] SaveToCheckpoint"));

	if (!CurrentSaveSlot.IsEmpty())
	{
		SaveToSlot(CurrentSaveSlot);
	}
}

void USaveLoadManagerComponent::AutoSave_Implementation()
{
	if (!bAutoSaveNeeded || !bCanResave) return;

	UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] AutoSave triggered"));
	SaveToCheckpoint();
}

void USaveLoadManagerComponent::SerializeAllData_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] SerializeAllData"));

	// Get spawn transform
	if (AActor* Owner = GetOwner())
	{
		SaveData.SpawnTransform = Owner->GetActorTransform();
	}

	// Serialize stats
	if (StatManager)
	{
		StatManager->SerializeStatsData();
		SaveData.Level = StatManager->Level;
	}

	// Serialize progress
	if (ProgressManager)
	{
		ProgressManager->SerializeProgressData();
		SaveData.PlayTime = ProgressManager->PlayTime;
	}

	// Serialize inventory
	if (UInventoryManagerComponent* InvMgr = Cast<UInventoryManagerComponent>(InventoryManager))
	{
		InvMgr->SerializeInventoryStorageCurrencyData(SaveData.InventoryData);
	}

	// Serialize equipment
	if (UEquipmentManagerComponent* EquipMgr = Cast<UEquipmentManagerComponent>(EquipmentManager))
	{
		EquipMgr->SerializeEquipmentData(SaveData.EquipmentData);
	}
}

void USaveLoadManagerComponent::AddToSaveData_Implementation(FGameplayTag DataTag, const FInstancedStruct& InData)
{
	UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] AddToSaveData: %s"), *DataTag.ToString());

	DataCache.Add(InData);
}

void USaveLoadManagerComponent::UpdateSaveData_Implementation(FGameplayTag DataTag)
{
	UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] UpdateSaveData: %s"), *DataTag.ToString());

	// Update specific data category based on tag
	FString TagString = DataTag.ToString();

	if (TagString.Contains(TEXT("Stats")))
	{
		if (StatManager)
		{
			StatManager->SerializeStatsData();
			SaveData.Level = StatManager->Level;
		}
	}
	else if (TagString.Contains(TEXT("Inventory")))
	{
		if (UInventoryManagerComponent* InvMgr = Cast<UInventoryManagerComponent>(InventoryManager))
		{
			InvMgr->SerializeInventoryStorageCurrencyData(SaveData.InventoryData);
		}
	}
	else if (TagString.Contains(TEXT("Equipment")))
	{
		if (UEquipmentManagerComponent* EquipMgr = Cast<UEquipmentManagerComponent>(EquipmentManager))
		{
			EquipMgr->SerializeEquipmentData(SaveData.EquipmentData);
		}
	}
	else if (TagString.Contains(TEXT("Progress")))
	{
		if (ProgressManager)
		{
			ProgressManager->SerializeProgressData();
			SaveData.PlayTime = ProgressManager->PlayTime;
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// LOAD OPERATIONS [7-11/23]
// ═══════════════════════════════════════════════════════════════════════════════

void USaveLoadManagerComponent::LoadFromSlot_Implementation(const FString& SlotName)
{
	UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] LoadFromSlot: %s"), *SlotName);

	if (!DoesSaveExist(SlotName))
	{
		UE_LOG(LogTemp, Warning, TEXT("[SaveLoadManager] Save slot does not exist: %s"), *SlotName);
		return;
	}

	CurrentSaveSlot = SlotName;
	SGO_Character = UGameplayStatics::LoadGameFromSlot(SlotName, 0);

	if (SGO_Character)
	{
		LoadDataFromSave();
	}
}

void USaveLoadManagerComponent::LoadDataFromSave_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] LoadDataFromSave"));

	// Extract FSLFSaveGameInfo from SGO_Character
	if (USG_SoulslikeFramework* SaveGame = Cast<USG_SoulslikeFramework>(SGO_Character))
	{
		SaveData = SaveGame->GetSavedData();
		UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Loaded save data from slot: %s"), *SaveData.SlotName);
	}

	InitializeLoadedData();
}

void USaveLoadManagerComponent::InitializeLoadedData_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] InitializeLoadedData"));

	ApplyLoadedData();
	OnDataLoaded.Broadcast(SaveData);
}

void USaveLoadManagerComponent::ApplyLoadedData_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] ApplyLoadedData"));

	// Apply stats
	if (StatManager)
	{
		StatManager->InitializeLoadedStats(SaveData.StatsData);
		StatManager->Level = SaveData.Level;
	}

	// Apply progress
	if (ProgressManager)
	{
		ProgressManager->InitializeLoadedPlayTime(SaveData.PlayTime);
	}

	// Apply spawn transform
	if (AActor* Owner = GetOwner())
	{
		Owner->SetActorTransform(SaveData.SpawnTransform);
	}
}

bool USaveLoadManagerComponent::DoesSaveExist_Implementation(const FString& SlotName)
{
	return UGameplayStatics::DoesSaveGameExist(SlotName, 0);
}

// ═══════════════════════════════════════════════════════════════════════════════
// SLOT MANAGEMENT [12-16/23]
// ═══════════════════════════════════════════════════════════════════════════════

void USaveLoadManagerComponent::CreateNewSlot_Implementation(const FString& SlotName)
{
	UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] CreateNewSlot: %s"), *SlotName);

	CurrentSaveSlot = SlotName;

	// Create new save game object using our SG_SoulslikeFramework class
	SGO_Character = UGameplayStatics::CreateSaveGameObject(USG_SoulslikeFramework::StaticClass());

	if (USG_SoulslikeFramework* SaveGame = Cast<USG_SoulslikeFramework>(SGO_Character))
	{
		SaveData.SlotName = SlotName;
		SaveGame->SetSavedData(SaveData);
	}

	InitializeNewGame();
}

void USaveLoadManagerComponent::DeleteSlot_Implementation(const FString& SlotName)
{
	UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] DeleteSlot: %s"), *SlotName);

	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		UGameplayStatics::DeleteGameInSlot(SlotName, 0);
	}
}

void USaveLoadManagerComponent::GetAllSlots_Implementation(TArray<FString>& OutSlots)
{
	UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] GetAllSlots"));

	OutSlots.Empty();

	// Check for common slot names
	for (int32 i = 0; i < 10; ++i)
	{
		FString SlotName = FString::Printf(TEXT("SaveSlot_%d"), i);
		if (DoesSaveExist(SlotName))
		{
			OutSlots.Add(SlotName);
		}
	}
}

void USaveLoadManagerComponent::SetActiveSlot_Implementation(const FString& SlotName)
{
	CurrentSaveSlot = SlotName;
	UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] SetActiveSlot: %s"), *SlotName);
}

FString USaveLoadManagerComponent::GetActiveSlot_Implementation()
{
	return CurrentSaveSlot;
}

// ═══════════════════════════════════════════════════════════════════════════════
// AUTOSAVE CONTROL [17-20/23]
// ═══════════════════════════════════════════════════════════════════════════════

void USaveLoadManagerComponent::StartAutosaveTimer_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] StartAutosaveTimer: %.0f seconds"), AutosaveTimer);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(AutosaveTimerHandle, this,
			&USaveLoadManagerComponent::OnAutosaveTimerTick_Implementation,
			AutosaveTimer, true);
	}
}

void USaveLoadManagerComponent::StopAutosaveTimer_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] StopAutosaveTimer"));

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AutosaveTimerHandle);
	}
}

void USaveLoadManagerComponent::OnAutosaveTimerTick_Implementation()
{
	AutoSave();
}

void USaveLoadManagerComponent::SetAutoSaveNeeded_Implementation(bool bNeeded)
{
	bAutoSaveNeeded = bNeeded;
}

// ═══════════════════════════════════════════════════════════════════════════════
// INITIALIZATION [21-23/23]
// ═══════════════════════════════════════════════════════════════════════════════

void USaveLoadManagerComponent::Initialize_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Initialize"));

	CacheComponentReferences();
	StartAutosaveTimer();
}

void USaveLoadManagerComponent::InitializeNewGame_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] InitializeNewGame"));

	// Reset all data to defaults
	SaveData = FSLFSaveGameInfo();
	SaveData.Level = 1;
	SaveData.PlayTime = FTimespan::Zero();

	// Save initial state
	SaveToCheckpoint();
}

void USaveLoadManagerComponent::CacheComponentReferences_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] CacheComponentReferences"));

	if (AActor* Owner = GetOwner())
	{
		StatManager = Owner->FindComponentByClass<UStatManagerComponent>();
		ProgressManager = Owner->FindComponentByClass<UProgressManagerComponent>();

		// Generic component lookup for inventory/equipment
		TArray<UActorComponent*> AllComponents;
		Owner->GetComponents(AllComponents);

		for (UActorComponent* Comp : AllComponents)
		{
			FString ClassName = Comp->GetClass()->GetName();
			if (ClassName.Contains(TEXT("Inventory")))
				InventoryManager = Comp;
			else if (ClassName.Contains(TEXT("Equipment")))
				EquipmentManager = Comp;
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// ADDITIONAL EVENTS
// ═══════════════════════════════════════════════════════════════════════════════

void USaveLoadManagerComponent::EventAttemptAutosave()
{
	UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] EventAttemptAutosave"));

	if (bAutoSaveNeeded)
	{
		SaveToCheckpoint();
		bAutoSaveNeeded = false;
	}
}

bool USaveLoadManagerComponent::IsSaveDataValid() const
{
	// Check if we have valid save data loaded
	return SGO_Character != nullptr;
}

void USaveLoadManagerComponent::EventSetLoadedData(USaveGame* InSaveGame)
{
	UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] EventSetLoadedData"));

	SGO_Character = InSaveGame;
	if (InSaveGame)
	{
		// Trigger load events to apply data
		OnDataLoaded.Broadcast(SaveData);
	}
}

void USaveLoadManagerComponent::LoadDataAsync(const FString& SlotName)
{
	UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] LoadDataAsync: %s"), *SlotName);

	// Async load using GameplayStatics
	FAsyncLoadGameFromSlotDelegate LoadDelegate;
	LoadDelegate.BindLambda([this](const FString& Slot, const int32 UserIndex, USaveGame* LoadedGame)
	{
		if (LoadedGame)
		{
			EventSetLoadedData(LoadedGame);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[SaveLoadManager] Failed to async load slot"));
		}
	});

	UGameplayStatics::AsyncLoadGameFromSlot(SlotName, 0, LoadDelegate);
}

void USaveLoadManagerComponent::EventTryPreloadData(int32 SlotIndex)
{
	UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] EventTryPreloadData: Slot %d"), SlotIndex);

	// Construct slot name and preload
	FString SlotName = FString::Printf(TEXT("SaveSlot_%d"), SlotIndex);
	LoadDataAsync(SlotName);
}

void USaveLoadManagerComponent::EventUpdateActiveSlot(int32 SlotIndex)
{
	UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] EventUpdateActiveSlot: %d"), SlotIndex);

	CurrentSaveSlot = FString::Printf(TEXT("SaveSlot_%d"), SlotIndex);
}

void USaveLoadManagerComponent::EventRemoveSpawnedActorFromSaveData(AActor* Actor)
{
	UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] EventRemoveSpawnedActorFromSaveData: %s"),
		Actor ? *Actor->GetName() : TEXT("null"));

	// Would remove actor from the SpawnedActorData array
	// This prevents respawning items that have been picked up, etc.
}
