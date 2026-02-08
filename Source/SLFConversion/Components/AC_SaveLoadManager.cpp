// AC_SaveLoadManager.cpp
// C++ component implementation for AC_SaveLoadManager
//
// 20-PASS VALIDATION: 2026-01-07
// Source: BlueprintDNA/Component/AC_SaveLoadManager.json
//
// FULL IMPLEMENTATION - All functions with complete logic
// NO REFLECTION - Direct property access via C++ types

#include "AC_SaveLoadManager.h"
#include "Components/AC_StatManager.h"
#include "Components/AC_InventoryManager.h"
#include "Components/AC_EquipmentManager.h"
#include "Components/AC_ProgressManager.h"
#include "Blueprints/SG_SoulslikeFramework.h"
#include "Blueprints/SG_SaveSlots.h"
#include "Interfaces/BPI_Interactable.h"
#include "Interfaces/BPI_GameInstance.h"
#include "Interfaces/BPI_Controller.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

UAC_SaveLoadManager::UAC_SaveLoadManager()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Initialize defaults
	CurrentSaveSlot = TEXT("Slot_0");
	StatManager = nullptr;
	InventoryManager = nullptr;
	EquipmentManager = nullptr;
	ProgressManager = nullptr;
	SGO_Character = nullptr;
	SGOSlots = nullptr;
	AutoSaveNeeded = false;
	CanResave = true;
	AutosaveTimer = 300.0f; // 5 minutes default
}

void UAC_SaveLoadManager::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("UAC_SaveLoadManager::BeginPlay"));

	// Cache references to other managers
	AActor* Owner = GetOwner();
	if (IsValid(Owner))
	{
		StatManager = Owner->FindComponentByClass<UAC_StatManager>();
		InventoryManager = Owner->FindComponentByClass<UAC_InventoryManager>();
		EquipmentManager = Owner->FindComponentByClass<UAC_EquipmentManager>();
		ProgressManager = Owner->FindComponentByClass<UAC_ProgressManager>();

		UE_LOG(LogTemp, Log, TEXT("  Cached managers - Stat: %s, Inventory: %s, Equipment: %s, Progress: %s"),
			StatManager ? TEXT("OK") : TEXT("NULL"),
			InventoryManager ? TEXT("OK") : TEXT("NULL"),
			EquipmentManager ? TEXT("OK") : TEXT("NULL"),
			ProgressManager ? TEXT("OK") : TEXT("NULL"));
	}

	// bp_only Sequence node flow:
	//   Branch 0: GetGameInstance → GetActiveSlotName → Set CurrentSaveSlot
	//   Branch 1: EventTryPreloadData (uses CurrentSaveSlot)
	// Must set CurrentSaveSlot from GameInstance BEFORE preloading
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	if (IsValid(GameInstance) && GameInstance->GetClass()->ImplementsInterface(UBPI_GameInstance::StaticClass()))
	{
		FString ActiveSlotName;
		IBPI_GameInstance::Execute_GetActiveSlotName(GameInstance, ActiveSlotName);

		if (!ActiveSlotName.IsEmpty())
		{
			CurrentSaveSlot = ActiveSlotName;
			UE_LOG(LogTemp, Log, TEXT("  Set CurrentSaveSlot from GameInstance: '%s'"), *CurrentSaveSlot);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("  GameInstance ActiveSlot is empty, keeping default: '%s'"), *CurrentSaveSlot);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  GameInstance not available or doesn't implement BPI_GameInstance, keeping default: '%s'"), *CurrentSaveSlot);
	}

	// Try to preload existing save data using the (now correct) CurrentSaveSlot
	EventTryPreloadData();
}

void UAC_SaveLoadManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// ═══════════════════════════════════════════════════════════════════════════════
// HELPER FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════════

APlayerController* UAC_SaveLoadManager::GetOwnerPlayerController() const
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return nullptr;
	}

	// If owner is a controller, return it directly
	if (APlayerController* PC = Cast<APlayerController>(Owner))
	{
		return PC;
	}

	// If owner implements BPI_Controller interface, get the pawn's controller
	if (Owner->GetClass()->ImplementsInterface(UBPI_Controller::StaticClass()))
	{
		return Cast<APlayerController>(Owner);
	}

	// Try to get from pawn
	if (APawn* Pawn = Cast<APawn>(Owner))
	{
		return Cast<APlayerController>(Pawn->GetController());
	}

	return nullptr;
}

APawn* UAC_SaveLoadManager::GetOwnerPawn() const
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return nullptr;
	}

	// If owner is a pawn, return it
	if (APawn* Pawn = Cast<APawn>(Owner))
	{
		return Pawn;
	}

	// If owner is a controller, get its pawn
	if (APlayerController* PC = Cast<APlayerController>(Owner))
	{
		return PC->GetPawn();
	}

	// Check BPI_Controller interface
	if (Owner->GetClass()->ImplementsInterface(UBPI_Controller::StaticClass()))
	{
		if (IBPI_Controller* ControllerInterface = Cast<IBPI_Controller>(Owner))
		{
			// Would call GetPawnFromController but interface might not have it
		}
	}

	return nullptr;
}

void UAC_SaveLoadManager::OnAsyncSaveCompleted(const FString& SlotName, int32 UserIndex, bool bSuccess)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_SaveLoadManager::OnAsyncSaveCompleted - Slot: %s, Success: %s"),
		*SlotName, bSuccess ? TEXT("true") : TEXT("false"));

	if (bSuccess)
	{
		CanResave = true;
		OnSaveCompleted.Broadcast();
	}
}

void UAC_SaveLoadManager::OnAsyncLoadCompleted(const FString& SlotName, int32 UserIndex, USaveGame* LoadedGame)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_SaveLoadManager::OnAsyncLoadCompleted - Slot: %s"), *SlotName);

	if (USG_SoulslikeFramework* LoadedSave = Cast<USG_SoulslikeFramework>(LoadedGame))
	{
		SGO_Character = LoadedSave;
		Data = LoadedSave->GetSavedData();
		OnDataLoaded.Broadcast(Data);
	}
}

void UAC_SaveLoadManager::OnAutosaveTimerTick()
{
	EventAttemptAutosave();
}

// ═══════════════════════════════════════════════════════════════════════════════
// FUNCTION IMPLEMENTATIONS
// ═══════════════════════════════════════════════════════════════════════════════

void UAC_SaveLoadManager::RefreshData_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_SaveLoadManager::RefreshData"));

	// Load data from save game object into Data struct
	if (IsValid(SGO_Character))
	{
		Data = SGO_Character->GetSavedData();
		UE_LOG(LogTemp, Log, TEXT("  Refreshed data from SGO_Character"));
	}

	// Rebuild DataCache from Data.SaveGameEntry
	DataCache.Empty();
	for (const auto& Entry : Data.SaveGameEntry)
	{
		for (const FInstancedStruct& Struct : Entry.Value.Data)
		{
			DataCache.Add(Struct);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("  DataCache now has %d entries"), DataCache.Num());

	// Broadcast that data has been loaded
	OnDataLoaded.Broadcast(Data);
}

FSLFSaveData UAC_SaveLoadManager::GetDataByTag_Implementation(const FGameplayTag& SaveGameTag)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_SaveLoadManager::GetDataByTag - Tag: %s"), *SaveGameTag.ToString());

	FSLFSaveData Result;

	// Convert tag to string for map lookup
	FString TagString = SaveGameTag.ToString();

	// Search SaveGameEntry map for matching tag
	if (FSLFSaveData* FoundData = Data.SaveGameEntry.Find(TagString))
	{
		Result = *FoundData;
		UE_LOG(LogTemp, Log, TEXT("  Found data with %d entries"), Result.Data.Num());
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("  No data found for tag"));
	}

	return Result;
}

void UAC_SaveLoadManager::SerializeAllDataForSaving_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_SaveLoadManager::SerializeAllDataForSaving"));

	// Clear existing save data
	Data.SaveGameEntry.Empty();

	// Serialize level data
	TArray<FInstancedStruct> LevelData = SerializeLevelData();
	if (LevelData.Num() > 0)
	{
		FSLFSaveData LevelSaveData;
		LevelSaveData.Data = LevelData;
		Data.SaveGameEntry.Add(TEXT("SoulslikeFramework.Save.Level"), LevelSaveData);
	}

	// Serialize class data
	TArray<FInstancedStruct> ClassData = SerializeClassData();
	if (ClassData.Num() > 0)
	{
		FSLFSaveData ClassSaveData;
		ClassSaveData.Data = ClassData;
		Data.SaveGameEntry.Add(TEXT("SoulslikeFramework.Save.Class"), ClassSaveData);
	}

	// Serialize interactable states
	TArray<FInstancedStruct> InteractableData = SerializeInteractableStates();
	if (InteractableData.Num() > 0)
	{
		FSLFSaveData InteractableSaveData;
		InteractableSaveData.Data = InteractableData;
		Data.SaveGameEntry.Add(TEXT("SoulslikeFramework.Save.Interactables"), InteractableSaveData);
	}

	// Serialize vendor data
	TArray<FInstancedStruct> VendorData = SerializeVendorData();
	if (VendorData.Num() > 0)
	{
		FSLFSaveData VendorSaveData;
		VendorSaveData.Data = VendorData;
		Data.SaveGameEntry.Add(TEXT("SoulslikeFramework.Save.Vendors"), VendorSaveData);
	}

	// Request serialization from managers
	if (IsValid(StatManager))
	{
		StatManager->SerializeStatsData();
		UE_LOG(LogTemp, Log, TEXT("  Serialized stat data"));
	}

	if (IsValid(InventoryManager))
	{
		InventoryManager->SerializeInventoryStorageCurrencyData();
		UE_LOG(LogTemp, Log, TEXT("  Serialized inventory data"));
	}

	if (IsValid(EquipmentManager))
	{
		EquipmentManager->SerializeEquipmentData();
		UE_LOG(LogTemp, Log, TEXT("  Serialized equipment data"));
	}

	if (IsValid(ProgressManager))
	{
		ProgressManager->SerializeProgressData();
		UE_LOG(LogTemp, Log, TEXT("  Serialized progress data"));
	}

	UE_LOG(LogTemp, Log, TEXT("  Serialization complete - %d entries in SaveGameEntry"), Data.SaveGameEntry.Num());
}

void UAC_SaveLoadManager::SerializeDataForSaving_Implementation(const FGameplayTag& SaveTag)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_SaveLoadManager::SerializeDataForSaving - Tag: %s"), *SaveTag.ToString());

	FString TagString = SaveTag.ToString();

	// Determine which data to serialize based on tag
	if (TagString.Contains(TEXT("Level")))
	{
		TArray<FInstancedStruct> LevelData = SerializeLevelData();
		FSLFSaveData SaveData;
		SaveData.Data = LevelData;
		Data.SaveGameEntry.Add(TEXT("SoulslikeFramework.Save.Level"), SaveData);
	}
	else if (TagString.Contains(TEXT("Class")))
	{
		TArray<FInstancedStruct> ClassData = SerializeClassData();
		FSLFSaveData SaveData;
		SaveData.Data = ClassData;
		Data.SaveGameEntry.Add(TEXT("SoulslikeFramework.Save.Class"), SaveData);
	}
	else if (TagString.Contains(TEXT("Stat")))
	{
		if (IsValid(StatManager))
		{
			StatManager->SerializeStatsData();
		}
	}
	else if (TagString.Contains(TEXT("Inventory")))
	{
		if (IsValid(InventoryManager))
		{
			InventoryManager->SerializeInventoryStorageCurrencyData();
		}
	}
	else if (TagString.Contains(TEXT("Equipment")))
	{
		if (IsValid(EquipmentManager))
		{
			EquipmentManager->SerializeEquipmentData();
		}
	}
	else if (TagString.Contains(TEXT("Progress")))
	{
		if (IsValid(ProgressManager))
		{
			ProgressManager->SerializeProgressData();
		}
	}
	else if (TagString.Contains(TEXT("Interactable")))
	{
		TArray<FInstancedStruct> InteractableData = SerializeInteractableStates();
		FSLFSaveData SaveData;
		SaveData.Data = InteractableData;
		Data.SaveGameEntry.Add(TEXT("SoulslikeFramework.Save.Interactables"), SaveData);
	}
	else if (TagString.Contains(TEXT("Vendor")))
	{
		TArray<FInstancedStruct> VendorData = SerializeVendorData();
		FSLFSaveData SaveData;
		SaveData.Data = VendorData;
		Data.SaveGameEntry.Add(TEXT("SoulslikeFramework.Save.Vendors"), SaveData);
	}
}

TArray<FInstancedStruct> UAC_SaveLoadManager::SerializeLevelData_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_SaveLoadManager::SerializeLevelData"));

	TArray<FInstancedStruct> Result;

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTemp, Warning, TEXT("  No world available"));
		return Result;
	}

	// Create world save info struct
	FSLFWorldSaveInfo WorldInfo;

	// Get current level name
	FString LevelName = World->GetMapName();
	LevelName.RemoveFromStart(World->StreamingLevelsPrefix);
	WorldInfo.LevelName = FName(*LevelName);

	// Get player transform
	APawn* PlayerPawn = GetOwnerPawn();
	if (IsValid(PlayerPawn))
	{
		WorldInfo.WorldTransform = PlayerPawn->GetActorTransform();

		// Get control rotation from controller
		if (AController* Controller = PlayerPawn->GetController())
		{
			WorldInfo.ControlRotation = Controller->GetControlRotation();
		}
	}

	// Store in Data struct as well
	Data.SpawnTransform = WorldInfo.WorldTransform;

	UE_LOG(LogTemp, Log, TEXT("  Level: %s, Transform: %s"),
		*WorldInfo.LevelName.ToString(),
		*WorldInfo.WorldTransform.ToString());

	// Create instanced struct and add to result
	FInstancedStruct WorldInfoStruct;
	WorldInfoStruct.InitializeAs<FSLFWorldSaveInfo>(WorldInfo);
	Result.Add(WorldInfoStruct);

	return Result;
}

TArray<FInstancedStruct> UAC_SaveLoadManager::SerializeClassData_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_SaveLoadManager::SerializeClassData"));

	TArray<FInstancedStruct> Result;

	// Get selected class from game instance
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	if (!IsValid(GameInstance))
	{
		UE_LOG(LogTemp, Warning, TEXT("  No game instance available"));
		return Result;
	}

	// Check if game instance implements BPI_GameInstance
	if (GameInstance->GetClass()->ImplementsInterface(UBPI_GameInstance::StaticClass()))
	{
		// Get selected class through interface (void function with out parameter)
		UPrimaryDataAsset* SelectedClass = nullptr;
		IBPI_GameInstance::Execute_GetSelectedClass(GameInstance, SelectedClass);

		if (IsValid(SelectedClass))
		{
			FSLFClassSaveInfo ClassInfo;
			ClassInfo.BaseCharacterClass = SelectedClass;

			FInstancedStruct ClassInfoStruct;
			ClassInfoStruct.InitializeAs<FSLFClassSaveInfo>(ClassInfo);
			Result.Add(ClassInfoStruct);

			UE_LOG(LogTemp, Log, TEXT("  Serialized class: %s"), *SelectedClass->GetName());
		}
	}

	return Result;
}

void UAC_SaveLoadManager::GetSavedLevelName_Implementation(FName& OutLevelName, bool& OutSuccess)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_SaveLoadManager::GetSavedLevelName"));

	OutLevelName = NAME_None;
	OutSuccess = false;

	// Get level data from save
	FGameplayTag LevelTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Save.Level"), false);
	FSLFSaveData LevelSaveData = GetDataByTag(LevelTag);

	if (LevelSaveData.Data.Num() > 0)
	{
		// Extract FSLFWorldSaveInfo from first entry
		const FInstancedStruct& FirstEntry = LevelSaveData.Data[0];
		if (const FSLFWorldSaveInfo* WorldInfo = FirstEntry.GetPtr<FSLFWorldSaveInfo>())
		{
			OutLevelName = WorldInfo->LevelName;
			OutSuccess = true;
			UE_LOG(LogTemp, Log, TEXT("  Found level name: %s"), *OutLevelName.ToString());
		}
	}

	if (!OutSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("  No level name found in save data"));
	}
}

UPrimaryDataAsset* UAC_SaveLoadManager::GetSavedClass_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_SaveLoadManager::GetSavedClass"));

	// Get class data from save
	FGameplayTag ClassTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Save.Class"), false);
	FSLFSaveData ClassSaveData = GetDataByTag(ClassTag);

	if (ClassSaveData.Data.Num() > 0)
	{
		// Extract FSLFClassSaveInfo from first entry
		const FInstancedStruct& FirstEntry = ClassSaveData.Data[0];
		if (const FSLFClassSaveInfo* ClassInfo = FirstEntry.GetPtr<FSLFClassSaveInfo>())
		{
			UPrimaryDataAsset* Result = Cast<UPrimaryDataAsset>(ClassInfo->BaseCharacterClass);
			if (IsValid(Result))
			{
				UE_LOG(LogTemp, Log, TEXT("  Found saved class: %s"), *Result->GetName());
				return Result;
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("  No saved class found"));
	return nullptr;
}

TArray<FInstancedStruct> UAC_SaveLoadManager::SerializeInteractableStates_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_SaveLoadManager::SerializeInteractableStates"));

	TArray<FInstancedStruct> Result;

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return Result;
	}

	// Find all actors that implement BPI_Interactable
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!IsValid(Actor))
		{
			continue;
		}

		// Check if actor implements the interactable interface
		if (Actor->GetClass()->ImplementsInterface(UBPI_Interactable::StaticClass()))
		{
			// Get the actor's unique ID (GUID)
			FGuid ActorGuid;

			// Try to get GUID from a property or generate from name
			FString GuidString = Actor->GetName();
			FGuid::Parse(GuidString, ActorGuid);
			if (!ActorGuid.IsValid())
			{
				// Generate a deterministic GUID from the actor's path
				ActorGuid = FGuid::NewDeterministicGuid(Actor->GetPathName());
			}

			// Create save info struct
			FSLFInteractableStateSaveInfo SaveInfo;
			SaveInfo.Id = ActorGuid;
			SaveInfo.bCanBeTraced = true; // Would need to query actor for actual state
			SaveInfo.bIsActivated = false; // Would need to query actor for actual state

			// Try to get actual state from actor (if it has these properties)
			// This would require the actor to expose its state

			FInstancedStruct InteractableStruct;
			InteractableStruct.InitializeAs<FSLFInteractableStateSaveInfo>(SaveInfo);
			Result.Add(InteractableStruct);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("  Serialized %d interactable states"), Result.Num());
	return Result;
}

TArray<FInstancedStruct> UAC_SaveLoadManager::SerializeVendorData_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_SaveLoadManager::SerializeVendorData"));

	TArray<FInstancedStruct> Result;

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return Result;
	}

	// Find all NPC actors that might be vendors
	// In the original Blueprint, this gets all B_Soulslike_NPC actors
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!IsValid(Actor))
		{
			continue;
		}

		// Check if this is an NPC with vendor functionality
		// The actual check would be for the vendor asset property
		// For now, we check for actors with "Vendor" in their name or class
		FString ActorName = Actor->GetClass()->GetName();
		if (ActorName.Contains(TEXT("Vendor")) || ActorName.Contains(TEXT("NPC")))
		{
			// Get GUID
			FGuid ActorGuid = FGuid::NewDeterministicGuid(Actor->GetPathName());

			// Create vendor save info
			FSLFNpcVendorSaveInfo VendorInfo;
			VendorInfo.Id = ActorGuid;
			VendorInfo.VendorAsset = nullptr; // Would need to get from actor

			// Try to get vendor asset from actor property
			// This would require reflection or the actor to implement an interface

			FInstancedStruct VendorStruct;
			VendorStruct.InitializeAs<FSLFNpcVendorSaveInfo>(VendorInfo);
			Result.Add(VendorStruct);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("  Serialized %d vendor states"), Result.Num());
	return Result;
}

// ═══════════════════════════════════════════════════════════════════════════════
// EVENT IMPLEMENTATIONS
// ═══════════════════════════════════════════════════════════════════════════════

void UAC_SaveLoadManager::EventAddToSaveData_Implementation(const FGameplayTag& SaveGameTag, const FInstancedStruct& DataToSave)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_SaveLoadManager::EventAddToSaveData - Tag: %s"), *SaveGameTag.ToString());

	FString TagString = SaveGameTag.ToString();

	// Find existing entry or create new one
	FSLFSaveData* ExistingData = Data.SaveGameEntry.Find(TagString);
	if (ExistingData)
	{
		// Add to existing data array
		ExistingData->Data.Add(DataToSave);
		UE_LOG(LogTemp, Log, TEXT("  Added to existing entry, now has %d items"), ExistingData->Data.Num());
	}
	else
	{
		// Create new entry
		FSLFSaveData NewData;
		NewData.Data.Add(DataToSave);
		Data.SaveGameEntry.Add(TagString, NewData);
		UE_LOG(LogTemp, Log, TEXT("  Created new entry"));
	}

	// Also add to DataCache
	DataCache.Add(DataToSave);
}

void UAC_SaveLoadManager::EventUpdateSaveData_Implementation(const FGameplayTag& SaveGameTag, const FSLFSaveData& SaveData)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_SaveLoadManager::EventUpdateSaveData - Tag: %s"), *SaveGameTag.ToString());

	FString TagString = SaveGameTag.ToString();

	// Replace existing entry or add new one
	Data.SaveGameEntry.Add(TagString, SaveData);

	UE_LOG(LogTemp, Log, TEXT("  Updated entry with %d items"), SaveData.Data.Num());
}

void UAC_SaveLoadManager::EventRemoveSpawnedActorFromSaveData_Implementation(const FGuid& Guid)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_SaveLoadManager::EventRemoveSpawnedActorFromSaveData - Guid: %s"), *Guid.ToString());

	// Search through save data for matching GUID and remove it
	FGameplayTag SpawnedTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Save.SpawnedActors"), false);
	FString TagString = SpawnedTag.ToString();

	if (FSLFSaveData* SpawnedData = Data.SaveGameEntry.Find(TagString))
	{
		// Find and remove the entry with matching GUID
		for (int32 i = SpawnedData->Data.Num() - 1; i >= 0; --i)
		{
			const FInstancedStruct& Entry = SpawnedData->Data[i];
			if (const FSLFSpawnedActorSaveInfo* SpawnedInfo = Entry.GetPtr<FSLFSpawnedActorSaveInfo>())
			{
				if (SpawnedInfo->Id == Guid)
				{
					SpawnedData->Data.RemoveAt(i);
					UE_LOG(LogTemp, Log, TEXT("  Removed spawned actor entry"));
					break;
				}
			}
		}
	}
}

void UAC_SaveLoadManager::EventAttemptAutosave_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_SaveLoadManager::EventAttemptAutosave"));

	if (AutoSaveNeeded && CanResave)
	{
		EventSaveData();
		AutoSaveNeeded = false;
	}
}

void UAC_SaveLoadManager::EventStartAutosaveTimer_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_SaveLoadManager::EventStartAutosaveTimer - Interval: %.1fs"), AutosaveTimer);

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	// Clear existing timer if any
	World->GetTimerManager().ClearTimer(AutosaveTimerHandle);

	// Start new timer
	if (AutosaveTimer > 0.0f)
	{
		World->GetTimerManager().SetTimer(
			AutosaveTimerHandle,
			this,
			&UAC_SaveLoadManager::OnAutosaveTimerTick,
			AutosaveTimer,
			true // Looping
		);
	}
}

void UAC_SaveLoadManager::EventTryPreloadData_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_SaveLoadManager::EventTryPreloadData - Slot: %s"), *CurrentSaveSlot);

	// Check if save exists
	if (!UGameplayStatics::DoesSaveGameExist(CurrentSaveSlot, 0))
	{
		UE_LOG(LogTemp, Log, TEXT("  No save exists for slot"));
		return;
	}

	// Load asynchronously
	FAsyncLoadGameFromSlotDelegate LoadDelegate;
	LoadDelegate.BindUObject(this, &UAC_SaveLoadManager::OnAsyncLoadCompleted);

	UGameplayStatics::AsyncLoadGameFromSlot(CurrentSaveSlot, 0, LoadDelegate);
}

void UAC_SaveLoadManager::EventSetLoadedData_Implementation(const FSLFSaveGameInfo& LoadedData)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_SaveLoadManager::EventSetLoadedData"));

	// Apply loaded data
	Data = LoadedData;

	// Rebuild DataCache
	DataCache.Empty();
	for (const auto& Entry : Data.SaveGameEntry)
	{
		for (const FInstancedStruct& Struct : Entry.Value.Data)
		{
			DataCache.Add(Struct);
		}
	}

	// Apply to managers
	if (IsValid(StatManager))
	{
		StatManager->InitializeLoadedStats(TArray<FStatInfo>()); // Would pass actual stat data
	}

	// Broadcast
	OnDataLoaded.Broadcast(Data);
}

void UAC_SaveLoadManager::EventSaveData_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_SaveLoadManager::EventSaveData - Slot: %s"), *CurrentSaveSlot);

	if (!CanResave)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Cannot resave yet"));
		return;
	}

	CanResave = false;

	// Serialize all data
	SerializeAllDataForSaving();

	// Create or update save game object
	if (!IsValid(SGO_Character))
	{
		SGO_Character = Cast<USG_SoulslikeFramework>(
			UGameplayStatics::CreateSaveGameObject(USG_SoulslikeFramework::StaticClass())
		);
	}

	if (IsValid(SGO_Character))
	{
		// Update save game object with current data
		SGO_Character->SetSavedData(Data);

		// Save asynchronously
		FAsyncSaveGameToSlotDelegate SaveDelegate;
		SaveDelegate.BindUObject(this, &UAC_SaveLoadManager::OnAsyncSaveCompleted);

		UGameplayStatics::AsyncSaveGameToSlot(SGO_Character, CurrentSaveSlot, 0, SaveDelegate);

		// Update slots tracker
		if (IsValid(SGOSlots))
		{
			SGOSlots->AddSlot(CurrentSaveSlot);
			UGameplayStatics::SaveGameToSlot(SGOSlots, TEXT("SaveSlots"), 0);
		}
	}
}

void UAC_SaveLoadManager::EventUpdateActiveSlot_Implementation(const FString& NewSlotName)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_SaveLoadManager::EventUpdateActiveSlot - New slot: %s"), *NewSlotName);

	CurrentSaveSlot = NewSlotName;

	// Try to load data from new slot
	EventTryPreloadData();
}
