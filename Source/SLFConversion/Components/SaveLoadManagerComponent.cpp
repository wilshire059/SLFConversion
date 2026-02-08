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
#include "Components/AC_InventoryManager.h"
#include "Components/AC_EquipmentManager.h"
#include "Blueprints/SG_SoulslikeFramework.h"
#include "Blueprints/SLFStatBase.h"
#include "Interfaces/BPI_GameInstance.h"
#include "GameFramework/PC_SoulslikeFramework.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "EngineUtils.h" // TActorIterator

USaveLoadManagerComponent::USaveLoadManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Initialize config
	AutosaveTimer = 60.0f; // 1 minute (debug - change back to 300 for release)

	// Initialize runtime
	CurrentSaveSlot = TEXT("");
	bAutoSaveNeeded = false;
	bCanResave = true;

	// Initialize component references
	StatManager = nullptr;
	InventoryManager = nullptr;
	EquipmentManager = nullptr;
	ProgressManager = nullptr;
	PawnInventoryManager = nullptr;
	PawnEquipmentManager = nullptr;
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

	// Create SGO_Character if it doesn't exist yet (failsafe for new games)
	if (!SGO_Character)
	{
		SGO_Character = UGameplayStatics::CreateSaveGameObject(USG_SoulslikeFramework::StaticClass());
		UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Created new SGO_Character for save"));
	}

	// Sync SaveData into SGO_Character before writing to disk
	if (USG_SoulslikeFramework* SaveGame = Cast<USG_SoulslikeFramework>(SGO_Character))
	{
		SaveData.SlotName = SlotName;
		SaveGame->SetSavedData(SaveData);
	}

	// Save the game
	if (SGO_Character)
	{
		UGameplayStatics::SaveGameToSlot(SGO_Character, SlotName, 0);
		UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] SaveGameToSlot SUCCEEDED: %s"), *SlotName);
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

	// Lazy re-cache: Pawn may not have existed at BeginPlay time
	if (!StatManager || !PawnInventoryManager || !PawnEquipmentManager)
	{
		CacheComponentReferences();
	}

	// Get spawn transform from pawn (not PC)
	if (AActor* Owner = GetOwner())
	{
		if (APlayerController* PC = Cast<APlayerController>(Owner))
		{
			if (APawn* Pawn = PC->GetPawn())
			{
				SaveData.SpawnTransform = Pawn->GetActorTransform();
				UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Saved spawn: %s"), *SaveData.SpawnTransform.GetLocation().ToString());
			}
		}
		else
		{
			SaveData.SpawnTransform = Owner->GetActorTransform();
		}
	}

	// Serialize stats - directly read from StatManager's ActiveStats
	if (StatManager)
	{
		SaveData.Level = StatManager->Level;
		SaveData.StatsData.Empty();

		for (const auto& StatEntry : StatManager->ActiveStats)
		{
			USLFStatBase* Stat = Cast<USLFStatBase>(StatEntry.Value);
			if (Stat)
			{
				FStatInfo SaveInfo = Stat->StatInfo;
				SaveInfo.Tag = StatEntry.Key;
				SaveData.StatsData.Add(FInstancedStruct::Make<FStatInfo>(SaveInfo));
			}
		}
		UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Serialized %d stats, Level=%d"), SaveData.StatsData.Num(), SaveData.Level);
	}

	// Serialize progress - directly read from ProgressManager
	if (ProgressManager)
	{
		SaveData.PlayTime = ProgressManager->PlayTime;
		SaveData.ProgressData.Empty();

		for (const auto& Pair : ProgressManager->CurrentProgress)
		{
			FSLFProgressSaveInfo SaveInfo;
			SaveInfo.ProgressTag = Pair.Key;
			SaveInfo.State = Pair.Value;
			SaveData.ProgressData.Add(FInstancedStruct::Make<FSLFProgressSaveInfo>(SaveInfo));
		}
		UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Serialized %d progress entries, PlayTime=%s"),
			SaveData.ProgressData.Num(), *SaveData.PlayTime.ToString());
	}

	// ═══════════════════════════════════════════════════════════════════════
	// INVENTORY SERIALIZATION
	// Priority: Pawn's UAC_InventoryManager (the live gameplay component)
	// Fallback: PC's UInventoryManagerComponent (has starting items)
	// ═══════════════════════════════════════════════════════════════════════
	SaveData.InventoryData.Empty();
	int32 CurrencyToSave = 0;

	if (PawnInventoryManager)
	{
		// Pawn's UAC_InventoryManager is the LIVE component - items picked up go here
		// Items is TMap<FGameplayTag, UPrimaryDataAsset*>
		for (const auto& ItemEntry : PawnInventoryManager->Items)
		{
			if (ItemEntry.Value)
			{
				FSLFInventoryItemsSaveInfo SaveInfo;
				SaveInfo.Item = ItemEntry.Value;
				SaveInfo.Amount = 1; // Tag-based map doesn't track quantity, default to 1
				SaveData.InventoryData.Add(FInstancedStruct::Make<FSLFInventoryItemsSaveInfo>(SaveInfo));
			}
		}
		CurrencyToSave = PawnInventoryManager->Currency;
		UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Serialized %d items from PAWN InventoryManager, Currency=%d"),
			SaveData.InventoryData.Num(), CurrencyToSave);
	}
	else if (UInventoryManagerComponent* InvMgr = Cast<UInventoryManagerComponent>(InventoryManager))
	{
		// Fallback: PC's UInventoryManagerComponent
		for (const auto& ItemEntry : InvMgr->Items)
		{
			const FSLFInventoryItem& InvItem = ItemEntry.Value;
			if (InvItem.ItemAsset)
			{
				FSLFInventoryItemsSaveInfo SaveInfo;
				SaveInfo.Item = InvItem.ItemAsset;
				SaveInfo.Amount = InvItem.Amount;
				SaveData.InventoryData.Add(FInstancedStruct::Make<FSLFInventoryItemsSaveInfo>(SaveInfo));
			}
		}
		CurrencyToSave = InvMgr->Currency;
		UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Serialized %d items from PC InventoryManager (fallback), Currency=%d"),
			SaveData.InventoryData.Num(), CurrencyToSave);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SaveLoadManager] No inventory manager found - no items will be saved!"));
	}

	// Store currency as a special entry (null item = currency marker)
	FSLFInventoryItemsSaveInfo CurrencyInfo;
	CurrencyInfo.Item = nullptr;
	CurrencyInfo.Amount = CurrencyToSave;
	SaveData.InventoryData.Add(FInstancedStruct::Make<FSLFInventoryItemsSaveInfo>(CurrencyInfo));

	// ═══════════════════════════════════════════════════════════════════════
	// EQUIPMENT SERIALIZATION
	// Priority: Pawn's UAC_EquipmentManager (the live gameplay component)
	// Fallback: PC's UEquipmentManagerComponent
	// ═══════════════════════════════════════════════════════════════════════
	SaveData.EquipmentData.Empty();

	if (PawnEquipmentManager)
	{
		// Pawn's UAC_EquipmentManager is the LIVE component
		// AllEquippedItems is TMap<FGameplayTag, TObjectPtr<UPrimaryDataAsset>>
		for (const auto& EquipEntry : PawnEquipmentManager->AllEquippedItems)
		{
			if (EquipEntry.Value)
			{
				FSLFEquipmentItemsSaveInfo SaveInfo;
				SaveInfo.SlotTag = EquipEntry.Key;
				SaveInfo.AssignedItem = EquipEntry.Value;
				SaveData.EquipmentData.Add(FInstancedStruct::Make<FSLFEquipmentItemsSaveInfo>(SaveInfo));
			}
		}
		UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Serialized %d equipment slots from PAWN EquipmentManager"),
			SaveData.EquipmentData.Num());

		// Save stance/overlay state
		SaveData.bRightHandTwoHandStance = PawnEquipmentManager->bRightHandTwoHandStance;
		SaveData.bLeftHandTwoHandStance = PawnEquipmentManager->bLeftHandTwoHandStance;
		SaveData.ActiveOverlayState = PawnEquipmentManager->ActiveOverlayState;
		UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Saved stance: R=%d L=%d Overlay=%d"),
			SaveData.bRightHandTwoHandStance ? 1 : 0, SaveData.bLeftHandTwoHandStance ? 1 : 0, (int32)SaveData.ActiveOverlayState);
	}
	else if (UEquipmentManagerComponent* EquipMgr = Cast<UEquipmentManagerComponent>(EquipmentManager))
	{
		// Fallback: PC's UEquipmentManagerComponent
		for (const auto& EquipEntry : EquipMgr->AllEquippedItems)
		{
			const FSLFCurrentEquipment& Equip = EquipEntry.Value;
			if (Equip.ItemAsset)
			{
				FSLFEquipmentItemsSaveInfo SaveInfo;
				SaveInfo.SlotTag = EquipEntry.Key;
				SaveInfo.AssignedItem = Equip.ItemAsset;
				SaveData.EquipmentData.Add(FInstancedStruct::Make<FSLFEquipmentItemsSaveInfo>(SaveInfo));
			}
		}
		UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Serialized %d equipment slots from PC EquipmentManager (fallback)"),
			SaveData.EquipmentData.Num());
	}
	else if (UAC_EquipmentManager* AcEquipMgr = Cast<UAC_EquipmentManager>(EquipmentManager))
	{
		// Fallback 2: EquipmentManager points to AC_EquipmentManager_C (Blueprint-derived from UAC_EquipmentManager)
		// This happens when FindComponentByClass<UEquipmentManagerComponent> fails but name-based search found it
		for (const auto& EquipEntry : AcEquipMgr->AllEquippedItems)
		{
			if (EquipEntry.Value)
			{
				FSLFEquipmentItemsSaveInfo SaveInfo;
				SaveInfo.SlotTag = EquipEntry.Key;
				SaveInfo.AssignedItem = EquipEntry.Value;
				SaveData.EquipmentData.Add(FInstancedStruct::Make<FSLFEquipmentItemsSaveInfo>(SaveInfo));
			}
		}
		UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Serialized %d equipment slots from UAC_EquipmentManager (fallback 2)"),
			SaveData.EquipmentData.Num());

		// Save stance/overlay state (fallback 2)
		SaveData.bRightHandTwoHandStance = AcEquipMgr->bRightHandTwoHandStance;
		SaveData.bLeftHandTwoHandStance = AcEquipMgr->bLeftHandTwoHandStance;
		SaveData.ActiveOverlayState = AcEquipMgr->ActiveOverlayState;
		UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Saved stance (fallback 2): R=%d L=%d Overlay=%d"),
			SaveData.bRightHandTwoHandStance ? 1 : 0, SaveData.bLeftHandTwoHandStance ? 1 : 0, (int32)SaveData.ActiveOverlayState);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SaveLoadManager] No equipment manager found - no equipment will be saved! EquipmentManager class: %s"),
			EquipmentManager ? *EquipmentManager->GetClass()->GetName() : TEXT("NULL"));
	}
}

void USaveLoadManagerComponent::AddToSaveData_Implementation(FGameplayTag DataTag, const FInstancedStruct& InData)
{
	UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] AddToSaveData: %s"), *DataTag.ToString());

	DataCache.Add(InData);

	// Mark autosave as needed so next timer tick will save
	bAutoSaveNeeded = true;
}

void USaveLoadManagerComponent::UpdateSaveData_Implementation(FGameplayTag DataTag)
{
	UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] UpdateSaveData: %s"), *DataTag.ToString());

	// Mark autosave as needed so next timer tick will save
	bAutoSaveNeeded = true;

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
	// Lazy re-cache: Pawn may not have existed at BeginPlay time
	if (!StatManager || !PawnInventoryManager || !PawnEquipmentManager)
	{
		CacheComponentReferences();
	}

	UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] ApplyLoadedData - Level=%d, PlayTime=%s, Stats=%d, Inventory=%d, Equipment=%d, Progress=%d"),
		SaveData.Level, *SaveData.PlayTime.ToString(),
		SaveData.StatsData.Num(), SaveData.InventoryData.Num(),
		SaveData.EquipmentData.Num(), SaveData.ProgressData.Num());

	// Apply stats - restore stat values from save data
	if (StatManager)
	{
		StatManager->Level = SaveData.Level;

		for (const FInstancedStruct& Entry : SaveData.StatsData)
		{
			if (const FStatInfo* LoadedStat = Entry.GetPtr<FStatInfo>())
			{
				if (UObject** FoundStat = StatManager->ActiveStats.Find(LoadedStat->Tag))
				{
					if (USLFStatBase* Stat = Cast<USLFStatBase>(*FoundStat))
					{
						Stat->StatInfo.CurrentValue = LoadedStat->CurrentValue;
						Stat->StatInfo.MaxValue = LoadedStat->MaxValue;
						UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Restored stat %s: %.0f/%.0f"),
							*LoadedStat->Tag.ToString(), LoadedStat->CurrentValue, LoadedStat->MaxValue);
					}
				}
			}
		}
		UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Applied %d stats, Level=%d"), SaveData.StatsData.Num(), SaveData.Level);
	}

	// Apply progress
	if (ProgressManager)
	{
		ProgressManager->InitializeLoadedPlayTime(SaveData.PlayTime);

		for (const FInstancedStruct& Entry : SaveData.ProgressData)
		{
			if (const FSLFProgressSaveInfo* LoadedProgress = Entry.GetPtr<FSLFProgressSaveInfo>())
			{
				ProgressManager->CurrentProgress.Add(LoadedProgress->ProgressTag, LoadedProgress->State);
			}
		}
		UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Applied %d progress entries"), SaveData.ProgressData.Num());
	}

	// ═══════════════════════════════════════════════════════════════════════
	// RESTORE INVENTORY
	// Priority: Pawn's UAC_InventoryManager (the live gameplay component)
	// Fallback: PC's UInventoryManagerComponent
	// ═══════════════════════════════════════════════════════════════════════
	if (PawnInventoryManager)
	{
		// Clear existing and restore items to Pawn's inventory
		PawnInventoryManager->Items.Empty();

		for (const FInstancedStruct& Entry : SaveData.InventoryData)
		{
			if (const FSLFInventoryItemsSaveInfo* LoadedItem = Entry.GetPtr<FSLFInventoryItemsSaveInfo>())
			{
				if (LoadedItem->Item)
				{
					UPrimaryDataAsset* PDA = Cast<UPrimaryDataAsset>(LoadedItem->Item);
					if (PDA)
					{
						// AddItem handles internal tag map and UI updates
						PawnInventoryManager->AddItem(PDA, LoadedItem->Amount, false);
						UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Restored item to PAWN: %s x%d"),
							*PDA->GetName(), LoadedItem->Amount);
					}
				}
				else
				{
					// null item = currency marker
					PawnInventoryManager->Currency = LoadedItem->Amount;
					UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Restored currency to PAWN: %d"), LoadedItem->Amount);
				}
			}
		}
		UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Applied %d items to PAWN InventoryManager"), SaveData.InventoryData.Num());
	}
	else if (UInventoryManagerComponent* InvMgr = Cast<UInventoryManagerComponent>(InventoryManager))
	{
		// Fallback: PC's UInventoryManagerComponent
		InvMgr->Items.Empty();

		for (const FInstancedStruct& Entry : SaveData.InventoryData)
		{
			if (const FSLFInventoryItemsSaveInfo* LoadedItem = Entry.GetPtr<FSLFInventoryItemsSaveInfo>())
			{
				if (LoadedItem->Item)
				{
					InvMgr->AddItem(Cast<UDataAsset>(LoadedItem->Item), LoadedItem->Amount, false);
					UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Restored item to PC: %s x%d"),
						*LoadedItem->Item->GetName(), LoadedItem->Amount);
				}
				else
				{
					InvMgr->Currency = LoadedItem->Amount;
					UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Restored currency to PC: %d"), LoadedItem->Amount);
				}
			}
		}
	}

	// ═══════════════════════════════════════════════════════════════════════
	// RESTORE EQUIPMENT
	// Priority: Pawn's UAC_EquipmentManager (the live gameplay component)
	// Fallback: PC's UEquipmentManagerComponent
	// ═══════════════════════════════════════════════════════════════════════
	if (PawnEquipmentManager)
	{
		for (const FInstancedStruct& Entry : SaveData.EquipmentData)
		{
			if (const FSLFEquipmentItemsSaveInfo* LoadedEquip = Entry.GetPtr<FSLFEquipmentItemsSaveInfo>())
			{
				if (LoadedEquip->AssignedItem)
				{
					UPrimaryDataAsset* PDA = Cast<UPrimaryDataAsset>(LoadedEquip->AssignedItem);
					if (PDA)
					{
						// Call EquipItemToSlot to visually apply equipment (spawns weapons, applies armor meshes, etc.)
						bool bS1, bS2, bS3, bS4, bS5, bS6, bS7;
						PawnEquipmentManager->EquipItemToSlot(PDA, LoadedEquip->SlotTag, false, bS1, bS2, bS3, bS4, bS5, bS6, bS7);
						UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Equipped to PAWN: %s -> %s (success: %d)"),
							*PDA->GetName(), *LoadedEquip->SlotTag.ToString(), bS1 ? 1 : 0);
					}
				}
			}
		}
		UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Applied %d equipment slots to PAWN EquipmentManager"), SaveData.EquipmentData.Num());
	}
	else if (UEquipmentManagerComponent* EquipMgr = Cast<UEquipmentManagerComponent>(EquipmentManager))
	{
		for (const FInstancedStruct& Entry : SaveData.EquipmentData)
		{
			if (const FSLFEquipmentItemsSaveInfo* LoadedEquip = Entry.GetPtr<FSLFEquipmentItemsSaveInfo>())
			{
				if (LoadedEquip->AssignedItem)
				{
					EquipMgr->EquipItemToSlot(
						LoadedEquip->SlotTag,
						Cast<UDataAsset>(LoadedEquip->AssignedItem),
						false
					);
					UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Restored equipment to PC: %s -> %s"),
						*LoadedEquip->AssignedItem->GetName(), *LoadedEquip->SlotTag.ToString());
				}
			}
		}
	}
	else if (UAC_EquipmentManager* AcEquipMgr = Cast<UAC_EquipmentManager>(EquipmentManager))
	{
		// Fallback 2: AC_EquipmentManager_C (Blueprint-derived from UAC_EquipmentManager)
		for (const FInstancedStruct& Entry : SaveData.EquipmentData)
		{
			if (const FSLFEquipmentItemsSaveInfo* LoadedEquip = Entry.GetPtr<FSLFEquipmentItemsSaveInfo>())
			{
				if (LoadedEquip->AssignedItem)
				{
					UPrimaryDataAsset* PDA = Cast<UPrimaryDataAsset>(LoadedEquip->AssignedItem);
					if (PDA)
					{
						bool bS1, bS2, bS3, bS4, bS5, bS6, bS7;
						AcEquipMgr->EquipItemToSlot(PDA, LoadedEquip->SlotTag, false, bS1, bS2, bS3, bS4, bS5, bS6, bS7);
						UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Restored equipment via UAC_EquipmentManager: %s -> %s (success: %d)"),
							*PDA->GetName(), *LoadedEquip->SlotTag.ToString(), bS1 ? 1 : 0);
					}
				}
			}
		}
		UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Applied %d equipment slots via UAC_EquipmentManager (fallback 2)"), SaveData.EquipmentData.Num());
	}

	// ═══════════════════════════════════════════════════════════════════════
	// RESTORE STANCE / OVERLAY STATE
	// Must happen AFTER equipment is restored (equipment sets base overlay)
	// ═══════════════════════════════════════════════════════════════════════
	{
		// Find the live equipment manager (any of the 3 paths)
		UAC_EquipmentManager* LiveEquipMgr = PawnEquipmentManager;
		if (!LiveEquipMgr)
		{
			LiveEquipMgr = Cast<UAC_EquipmentManager>(EquipmentManager);
		}

		if (LiveEquipMgr)
		{
			// AdjustForTwoHandStance toggles: if current is false (default after equip), calling it sets to true
			// Only call for hands that were saved as two-hand stance
			if (SaveData.bRightHandTwoHandStance)
			{
				LiveEquipMgr->AdjustForTwoHandStance(true); // true = right hand
				UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Restored right-hand two-hand stance"));
			}
			if (SaveData.bLeftHandTwoHandStance)
			{
				LiveEquipMgr->AdjustForTwoHandStance(false); // false = left hand
				UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Restored left-hand two-hand stance"));
			}
			UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Restored stance: R=%d L=%d SavedOverlay=%d"),
				SaveData.bRightHandTwoHandStance ? 1 : 0, SaveData.bLeftHandTwoHandStance ? 1 : 0, (int32)SaveData.ActiveOverlayState);
		}
	}

	// Apply spawn transform to the pawn
	if (SaveData.SpawnTransform.GetLocation() != FVector::ZeroVector)
	{
		if (AActor* Owner = GetOwner())
		{
			if (APlayerController* PC = Cast<APlayerController>(Owner))
			{
				if (APawn* Pawn = PC->GetPawn())
				{
					Pawn->SetActorTransform(SaveData.SpawnTransform);
					UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Restored spawn: %s"), *SaveData.SpawnTransform.GetLocation().ToString());
				}
			}
		}
	}

	// Destroy pickup items that were already collected in a previous session
	DestroyCollectedPickups();
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
	// Always mark autosave as needed on each timer tick
	// Without this, bAutoSaveNeeded stays false after the first save and no further saves happen
	bAutoSaveNeeded = true;
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

	// bp_only flow: Get ActiveSlot from GameInstance via BPI_GameInstance interface
	UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
	if (GI && GI->GetClass()->ImplementsInterface(UBPI_GameInstance::StaticClass()))
	{
		FString ActiveSlotName;
		IBPI_GameInstance::Execute_GetActiveSlotName(GI, ActiveSlotName);
		CurrentSaveSlot = ActiveSlotName;
		UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Got ActiveSlot from GameInstance: '%s'"), *CurrentSaveSlot);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SaveLoadManager] GameInstance does not implement BPI_GameInstance!"));
	}

	// Fallback: if no ActiveSlot but we have game components (PIE direct-to-game-level),
	// create a default slot so autosave works
	if (CurrentSaveSlot.IsEmpty() && StatManager)
	{
		CurrentSaveSlot = TEXT("SLF_Default_0");
		UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] No ActiveSlot set - using fallback: '%s'"), *CurrentSaveSlot);

		// Also set it on the GameInstance so the menu can find it
		if (GI && GI->GetClass()->ImplementsInterface(UBPI_GameInstance::StaticClass()))
		{
			IBPI_GameInstance::Execute_SetActiveSlot(GI, CurrentSaveSlot);
		}
	}

	// bp_only flow: TryPreloadData - check if save exists for this slot
	if (!CurrentSaveSlot.IsEmpty())
	{
		if (UGameplayStatics::DoesSaveGameExist(CurrentSaveSlot, 0))
		{
			// Existing save - load it asynchronously
			UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Save exists for '%s' - loading async"), *CurrentSaveSlot);
			LoadDataAsync(CurrentSaveSlot);
		}
		else
		{
			// New game - create SGO_Character and save immediately to disk
			UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] No save for '%s' - creating new save and writing to disk"), *CurrentSaveSlot);
			SGO_Character = UGameplayStatics::CreateSaveGameObject(USG_SoulslikeFramework::StaticClass());
			SaveData.SlotName = CurrentSaveSlot;
			SaveData.Level = 1;

			// Write save file immediately so Continue/LoadGame work even if user quits early
			SaveToSlot(CurrentSaveSlot);
		}
	}

	// Only enable autosave if we have critical component references (StatManager, InventoryManager).
	// This prevents the menu level's SaveLoadManager from overwriting game saves with empty data.
	if (StatManager || InventoryManager)
	{
		StartAutosaveTimer();
		bAutoSaveNeeded = true;
		UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Autosave ENABLED (has game components)"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Autosave DISABLED (no game components - likely menu level)"));
	}
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

	AActor* Owner = GetOwner();
	if (!Owner) return;

	// Strategy 1: Use PC's typed member variables directly (most reliable)
	if (APC_SoulslikeFramework* SlfPC = Cast<APC_SoulslikeFramework>(Owner))
	{
		if (SlfPC->AC_InventoryManager)
			InventoryManager = SlfPC->AC_InventoryManager;
		if (SlfPC->AC_EquipmentManager)
			EquipmentManager = SlfPC->AC_EquipmentManager;
		if (SlfPC->AC_ProgressManager)
			ProgressManager = SlfPC->AC_ProgressManager;
	}

	// Strategy 2: FindComponentByClass on PC
	if (!StatManager)
		StatManager = Owner->FindComponentByClass<UStatManagerComponent>();
	if (!ProgressManager)
		ProgressManager = Owner->FindComponentByClass<UProgressManagerComponent>();
	if (!InventoryManager)
		InventoryManager = Owner->FindComponentByClass<UInventoryManagerComponent>();
	if (!EquipmentManager)
		EquipmentManager = Owner->FindComponentByClass<UEquipmentManagerComponent>();

	// Strategy 3: Search the Pawn (StatManager lives there, and Pawn has its own inventory/equipment components)
	if (APlayerController* PC = Cast<APlayerController>(Owner))
	{
		if (APawn* Pawn = PC->GetPawn())
		{
			if (!StatManager)
			{
				StatManager = Pawn->FindComponentByClass<UStatManagerComponent>();
				UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Found StatManager on Pawn: %s"), StatManager ? TEXT("YES") : TEXT("NO"));
			}
			if (!ProgressManager)
				ProgressManager = Pawn->FindComponentByClass<UProgressManagerComponent>();
			if (!InventoryManager)
				InventoryManager = Pawn->FindComponentByClass<UInventoryManagerComponent>();
			if (!EquipmentManager)
				EquipmentManager = Pawn->FindComponentByClass<UEquipmentManagerComponent>();

			// Find Pawn's Blueprint-derived inventory/equipment managers (different class hierarchy)
			// These are the "live" components that gameplay systems actually use
			if (!PawnInventoryManager)
				PawnInventoryManager = Pawn->FindComponentByClass<UAC_InventoryManager>();
			if (!PawnEquipmentManager)
				PawnEquipmentManager = Pawn->FindComponentByClass<UAC_EquipmentManager>();

			// Strategy 4: Generic name fallback for PC components that aren't derived from our C++ classes
			if (!EquipmentManager || !InventoryManager)
			{
				// Search both PC and Pawn
				for (AActor* SearchActor : TArray<AActor*>({ Owner, Pawn }))
				{
					TArray<UActorComponent*> AllComps;
					SearchActor->GetComponents(AllComps);
					for (UActorComponent* Comp : AllComps)
					{
						FString ClassName = Comp->GetClass()->GetName();
						if (!EquipmentManager && ClassName.Contains(TEXT("Equipment")))
						{
							EquipmentManager = Comp;
							UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Found EquipmentManager via name: %s (class: %s) on %s"),
								*Comp->GetName(), *ClassName, *SearchActor->GetName());
						}
						if (!InventoryManager && ClassName.Contains(TEXT("Inventory")))
						{
							InventoryManager = Comp;
							UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Found InventoryManager via name: %s (class: %s) on %s"),
								*Comp->GetName(), *ClassName, *SearchActor->GetName());
						}
					}
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[SaveLoadManager] No Pawn found on PC during CacheComponentReferences!"));
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Components - Stat:%s Prog:%s Inv:%s(%s) Equip:%s(%s) PawnInv:%s PawnEquip:%s"),
		StatManager ? TEXT("OK") : TEXT("NULL"),
		ProgressManager ? TEXT("OK") : TEXT("NULL"),
		InventoryManager ? TEXT("OK") : TEXT("NULL"),
		InventoryManager ? *InventoryManager->GetClass()->GetName() : TEXT(""),
		EquipmentManager ? TEXT("OK") : TEXT("NULL"),
		EquipmentManager ? *EquipmentManager->GetClass()->GetName() : TEXT(""),
		PawnInventoryManager ? TEXT("OK") : TEXT("NULL"),
		PawnEquipmentManager ? TEXT("OK") : TEXT("NULL"));
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
		// Extract SaveData from the loaded save game and apply it
		LoadDataFromSave();
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

	MarkPickupCollected(Actor);
}

void USaveLoadManagerComponent::MarkPickupCollected(AActor* PickupActor)
{
	if (!PickupActor) return;

	// Use the actor's full path name for deterministic matching
	// Level-placed actors have consistent names like "B_PickupItem_Katana_C_0"
	FString ActorName = PickupActor->GetPathName();
	if (!SaveData.CollectedPickups.Contains(ActorName))
	{
		SaveData.CollectedPickups.Add(ActorName);
		bAutoSaveNeeded = true;
		UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Marked pickup as collected: %s (total: %d)"),
			*ActorName, SaveData.CollectedPickups.Num());
	}
}

void USaveLoadManagerComponent::DestroyCollectedPickups()
{
	if (SaveData.CollectedPickups.Num() == 0) return;

	UWorld* World = GetWorld();
	if (!World) return;

	int32 DestroyedCount = 0;
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (Actor && !Actor->IsPendingKillPending())
		{
			FString ActorPath = Actor->GetPathName();
			if (SaveData.CollectedPickups.Contains(ActorPath))
			{
				UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Destroying collected pickup: %s"), *ActorPath);
				Actor->Destroy();
				DestroyedCount++;
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[SaveLoadManager] Destroyed %d collected pickups out of %d tracked"),
		DestroyedCount, SaveData.CollectedPickups.Num());
}
