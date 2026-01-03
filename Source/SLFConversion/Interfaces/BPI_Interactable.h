// BPI_Interactable.h
// C++ Interface for BPI_Interactable
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA_v2/Interface/BPI_Interactable.json
// Functions: 4

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InstancedStruct.h"
#include "SLFGameTypes.h"
#include "BPI_Interactable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UBPI_Interactable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interactable Interface
 * Provides interaction functionality for world objects
 */
class SLFCONVERSION_API IBPI_Interactable
{
	GENERATED_BODY()

public:
	// Try to get item info for this interactable
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable Interface")
	void TryGetItemInfo(FSLFItemInfo& ItemInfo);

	// Called when spawned from save data
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable Interface")
	void OnSpawnedFromSave(FGuid Id, FInstancedStruct CustomData);

	// Called when interacted with
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable Interface")
	void OnInteract(AActor* InteractingActor);

	// Called when traced by interaction system
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable Interface")
	void OnTraced(AActor* TracedBy);
};
