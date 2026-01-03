// AC_AI_InteractionManager.cpp
// C++ component implementation for AC_AI_InteractionManager
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Component/AC_AI_InteractionManager.json
//
// PASS 8: Full logic implementation from Blueprint graphs
// PASS 10: Debug logging added

#include "AC_AI_InteractionManager.h"
#include "Engine/DataTable.h"

UAC_AI_InteractionManager::UAC_AI_InteractionManager()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Initialize defaults
	DialogAsset = nullptr;
	ProgressManager = nullptr;
	CurrentIndex = 0;
	MaxIndex = 0;
	VendorAsset = nullptr;
}

void UAC_AI_InteractionManager::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("UAC_AI_InteractionManager::BeginPlay"));
}

void UAC_AI_InteractionManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


// ═══════════════════════════════════════════════════════════════════════
// FUNCTION IMPLEMENTATIONS (From Blueprint Graph Analysis)
// ═══════════════════════════════════════════════════════════════════════

/**
 * GetCurrentDialogEntry - Get dialog entry at current index
 *
 * Blueprint Logic:
 * 1. Check if CurrentIndex is valid within Rows array
 * 2. Look up row in DataTable using Rows[CurrentIndex]
 * 3. Return the dialog entry
 */
FSLFDialogEntry UAC_AI_InteractionManager::GetCurrentDialogEntry_Implementation(UDataTable* DataTable, const TArray<FName>& Rows)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_AI_InteractionManager::GetCurrentDialogEntry - Index: %d, MaxIndex: %d"),
		CurrentIndex, Rows.Num());

	FSLFDialogEntry Result;

	if (!IsValid(DataTable))
	{
		UE_LOG(LogTemp, Warning, TEXT("  Invalid DataTable"));
		return Result;
	}

	// Update max index
	MaxIndex = Rows.Num();

	// Validate current index
	if (CurrentIndex < 0 || CurrentIndex >= Rows.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("  Invalid CurrentIndex: %d (Max: %d)"), CurrentIndex, Rows.Num());
		return Result;
	}

	// Get row name at current index
	FName RowName = Rows[CurrentIndex];

	// Look up row in data table
	FSLFDialogEntry* FoundEntry = DataTable->FindRow<FSLFDialogEntry>(RowName, TEXT("GetCurrentDialogEntry"));
	if (FoundEntry)
	{
		Result = *FoundEntry;
		UE_LOG(LogTemp, Log, TEXT("  Found entry for row: %s"), *RowName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  Row not found: %s"), *RowName.ToString());
	}

	return Result;
}
