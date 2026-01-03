// AIInteractionManagerComponent.h
// C++ base class for AC_AI_InteractionManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - AC_AI_InteractionManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         7/7 migrated (excluding dispatcher)
// Functions:         6/6 migrated (excluding ExecuteUbergraph)
// Event Dispatchers: 1/1 migrated
// Graphs:            3 (EventGraph, GetCurrentDialogEntry, OnDialogFinished)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/Components/AC_AI_InteractionManager
//
// PURPOSE: NPC dialog system - manages dialog progression and vendor interactions

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "AIInteractionManagerComponent.generated.h"

// Forward declarations
class UDataAsset;
class UProgressManagerComponent;
class UUserWidget;

// ═══════════════════════════════════════════════════════════════════════════════
// EVENT DISPATCHERS: 1/1 migrated
// ═══════════════════════════════════════════════════════════════════════════════

/** [1/1] Called when dialog sequence finishes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogFinished);

UCLASS(ClassGroup = (Soulslike), meta = (BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class SLFCONVERSION_API UAIInteractionManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAIInteractionManagerComponent();

	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 7/7 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Config Variables (3) ---

	/** [1/7] NPC display name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FText Name;

	/** [2/7] Dialog data asset (PDA_Dialog) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UDataAsset* DialogAsset;

	/** [3/7] Vendor data asset (PDA_Vendor) - for shop NPCs */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UDataAsset* VendorAsset;

	// --- Runtime Variables (4) ---

	/** [4/7] Cached progress manager reference */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	UProgressManagerComponent* ProgressManager;

	/** [5/7] Current dialog entry index */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	int32 CurrentIndex;

	/** [6/7] Maximum dialog entry index */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	int32 MaxIndex;

	/** [7/7] Active dialog data table (soft reference) */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	TSoftObjectPtr<UDataTable> ActiveTable;

	// ═══════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS: 1/1 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/1] Broadcast when dialog completes */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDialogFinished OnDialogFinished;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 6/6 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Dialog Access (1) ---

	/** [1/6] Get current dialog entry from active table
	 * @param DataTable - The dialog data table
	 * @param OutRowName - Row name at current index
	 * @param OutRow - Dialog entry struct
	 * @param bSuccess - Whether entry was found
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Interaction|Dialog")
	void GetCurrentDialogEntry(UDataTable* DataTable, FName& OutRowName, FTableRowBase& OutRow, bool& bSuccess);
	virtual void GetCurrentDialogEntry_Implementation(UDataTable* DataTable, FName& OutRowName, FTableRowBase& OutRow, bool& bSuccess);

	// --- Dialog Control (3) ---

	/** [2/6] Begin dialog sequence with player
	 * @param InProgressManager - Player's progress manager
	 * @param HUD - Player's HUD widget
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Interaction|Dialog")
	void BeginDialog(UProgressManagerComponent* InProgressManager, UUserWidget* HUD);
	virtual void BeginDialog_Implementation(UProgressManagerComponent* InProgressManager, UUserWidget* HUD);

	/** [3/6] Adjust index when exiting dialog (increments for "goodbye" responses) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Interaction|Dialog")
	void AdjustIndexForExit();
	virtual void AdjustIndexForExit_Implementation();

	/** [4/6] Reset dialog to beginning */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI Interaction|Dialog")
	void ResetDialogIndex();
	virtual void ResetDialogIndex_Implementation();

	// --- Internal Callbacks (2) ---

	/** [5/6] Callback when dialog table async loads */
	UFUNCTION()
	void OnLoaded_06386C0B4155173667FF4F931DF1ACA5(UObject* Loaded);

	/** [6/6] Callback when vendor table async loads */
	UFUNCTION()
	void OnLoaded_0D66BEC94ACD1661A92D91A4A9C8D9E4(UObject* Loaded);
};
