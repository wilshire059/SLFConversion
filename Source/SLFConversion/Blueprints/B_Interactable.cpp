// B_Interactable.cpp
// C++ implementation for Blueprint B_Interactable
//
// 20-PASS VALIDATION: 2026-01-07
// Source: BlueprintDNA/Blueprint/B_Interactable.json

#include "Blueprints/B_Interactable.h"
#include "Components/AC_SaveLoadManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "InstancedStruct.h"
#include "GameplayTagContainer.h"

AB_Interactable::AB_Interactable()
{
	// Default values
	CanBeTraced = true;
	IsActivated = false;
	// ID is auto-generated as FGuid::NewGuid() if needed, or can be set in editor
}

void AB_Interactable::AddInteractableStateToSaveData_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("AB_Interactable::AddInteractableStateToSaveData - %s"), *GetName());

	// 1. Get player controller
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!IsValid(PC))
	{
		UE_LOG(LogTemp, Warning, TEXT("  No player controller found"));
		return;
	}

	// 2. Get SaveLoadManager component from player's pawn
	APawn* Pawn = PC->GetPawn();
	if (!IsValid(Pawn))
	{
		UE_LOG(LogTemp, Warning, TEXT("  No pawn found"));
		return;
	}

	UAC_SaveLoadManager* SaveLoadManager = Pawn->FindComponentByClass<UAC_SaveLoadManager>();
	if (!IsValid(SaveLoadManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("  No SaveLoadManager component found on pawn"));
		return;
	}

	// 3. Create FSLFInteractableStateSaveInfo struct
	FSLFInteractableStateSaveInfo StateInfo;
	StateInfo.Id = ID;
	StateInfo.bCanBeTraced = CanBeTraced;
	StateInfo.bIsActivated = IsActivated;

	// 4. Create FInstancedStruct from the state info
	FInstancedStruct InstancedData;
	InstancedData.InitializeAs<FSLFInteractableStateSaveInfo>(StateInfo);

	// 5. Call EventAddToSaveData with the tag "SoulslikeFramework.Saving.InteractableStates"
	FGameplayTag InteractableTag = FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Saving.InteractableStates")));
	SaveLoadManager->EventAddToSaveData(InteractableTag, InstancedData);

	UE_LOG(LogTemp, Log, TEXT("  Added interactable state - ID: %s, CanBeTraced: %s, IsActivated: %s"),
		*ID.ToString(), CanBeTraced ? TEXT("true") : TEXT("false"), IsActivated ? TEXT("true") : TEXT("false"));
}

void AB_Interactable::AddSpawnedInteractableToSaveData_Implementation(const FInstancedStruct& AdditionalDataToSave)
{
	UE_LOG(LogTemp, Log, TEXT("AB_Interactable::AddSpawnedInteractableToSaveData - %s"), *GetName());

	// 1. Get player controller
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!IsValid(PC))
	{
		UE_LOG(LogTemp, Warning, TEXT("  No player controller found"));
		return;
	}

	// 2. Get SaveLoadManager component from player's pawn
	APawn* Pawn = PC->GetPawn();
	if (!IsValid(Pawn))
	{
		UE_LOG(LogTemp, Warning, TEXT("  No pawn found"));
		return;
	}

	UAC_SaveLoadManager* SaveLoadManager = Pawn->FindComponentByClass<UAC_SaveLoadManager>();
	if (!IsValid(SaveLoadManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("  No SaveLoadManager component found on pawn"));
		return;
	}

	// 3. For spawned interactables, we save additional data including the transform
	// The Blueprint creates an FSLFInteractableStateSaveInfo with extra transform data
	// For now, we pass through the additional data

	// 4. Call EventAddToSaveData with the tag for spawned actors
	FGameplayTag SpawnedTag = FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Saving.SpawnedActors")));
	SaveLoadManager->EventAddToSaveData(SpawnedTag, AdditionalDataToSave);

	UE_LOG(LogTemp, Log, TEXT("  Added spawned interactable data for ID: %s"), *ID.ToString());
}
