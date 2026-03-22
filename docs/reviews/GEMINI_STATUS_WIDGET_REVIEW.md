# W_Status Widget System - Peer Review

**Reviewer:** Claude Opus 4.5
**Date:** 2026-01-14
**Issue:** Status menu shows placeholder stats (99, 999) instead of actual character stat values

---

## Executive Summary

The W_Status widget system is **fundamentally broken** because the C++ implementation **does not call the stat population logic**. The Blueprint has a sophisticated data flow that:

1. Gets the StatManager component via the `BPI_GenericCharacter` interface
2. Waits for `OnStatsInitialized` event
3. Calls `GetStatManager()` to get stat data
4. Calls `SetupCurrentStats()` on each W_Status_StatBlock
5. `SetupCurrentStats()` creates W_StatEntry_Status widgets for each stat
6. Each W_StatEntry_Status initializes itself with the stat's current/max values

The C++ implementation **skips almost all of this**. The StatBlock's `SetupCurrentStats_Implementation()` is an empty stub that does nothing.

---

## Data Flow Analysis

### Blueprint Data Flow (How It Should Work)

```
W_Status (Parent Widget)
    |
    |-- NativeConstruct
    |       |
    |       +-- Get owning player pawn
    |       +-- Cast to BPI_GenericCharacter interface
    |       +-- Call GetStatManager() interface function
    |       +-- Store StatManagerComponent reference
    |       +-- Bind to OnStatsInitialized delegate
    |
    |-- EventStatInitializationComplete (triggered by OnStatsInitialized)
    |       |
    |       +-- For each W_Status_StatBlock child widget:
    |               |
    |               +-- GetStatManager() -> returns StatManager reference
    |               +-- GetAllStats() -> returns TArray<B_Stat*> and TMap<Class, Tag>
    |               +-- SetupCurrentStats(StatObjects, StatClassesAndCategories)
    |
    +-- W_Status_StatBlock::SetupCurrentStats()
            |
            +-- Clear existing children (StatsVerticalBox->ClearChildren())
            +-- For each stat in StatObjects:
            |       |
            |       +-- Check if stat's category matches this block's Category tag
            |       +-- If match: Create W_StatEntry_Status widget
            |       +-- Set widget.Stat = current stat object
            |       +-- AddChildToVerticalBox(StatsVerticalBox, newWidget)
            |       +-- Add widget to StatEntries array
            |
            +-- (Widget's NativeConstruct calls InitStatEntry automatically)

W_StatEntry_Status::NativeConstruct()
    |
    +-- InitStatEntry()
            |
            +-- Get Stat.StatInfo (CurrentValue, MaxValue, DisplayName, etc.)
            +-- Format value text based on bDisplayAsPercent, bShowMaxValue
            +-- Set StatName text widget
            +-- Set StatValue text widget
            +-- Bind to Stat.OnStatUpdated delegate
```

### Current C++ Implementation (What Actually Happens)

```
W_Status::NativeConstruct()
    |
    +-- Find StatManagerComponent on PC or Pawn (OK)
    +-- Bind to OnLevelUpdated (OK)
    +-- InitializeLevelCurrencyBlock() (OK - for level/currency only)
    |
    +-- MISSING: No call to populate stat blocks!
    +-- MISSING: No waiting for OnStatsInitialized!
    +-- MISSING: No iteration over W_Status_StatBlock widgets!

W_Status_StatBlock::SetupCurrentStats_Implementation()
    |
    +-- UE_LOG(...) // Just logging, no actual implementation!
    +-- return; // Does nothing!

W_StatEntry_Status::InitStatEntry_Implementation()
    |
    +-- UE_LOG(...) // Just logging, no actual implementation!
    +-- return; // Does nothing!
```

---

## Root Cause Analysis

### Problem 1: SetupCurrentStats_Implementation is Empty

**File:** `C:/scripts/SLFConversion/Source/SLFConversion/Widgets/W_Status_StatBlock.cpp`

```cpp
void UW_Status_StatBlock::SetupCurrentStats_Implementation(const TArray<UB_Stat*>& StatObjects, const TMap<FGameplayTag, TSubclassOf<UB_Stat>>& StatClassesAndCategories)
{
    // Setup status stat entries for each stat object in the provided array
    UE_LOG(LogTemp, Log, TEXT("UW_Status_StatBlock::SetupCurrentStats - %d stats"), StatObjects.Num());
    // NOTHING ACTUALLY HAPPENS HERE!
}
```

The Blueprint's SetupCurrentStats function does the following:
1. Calls `Map_Values()` on StatClassesAndCategories to get category tags
2. Calls `ClearChildren()` on StatsVerticalBox
3. Clears the StatEntries array
4. For each stat in StatObjects:
   - Checks if stat category matches this block's Category tag
   - Creates a W_StatEntry_Status widget
   - Sets the Stat property on the new widget
   - Adds widget to VerticalBox
   - Adds widget to StatEntries array

### Problem 2: W_Status Doesn't Trigger Stat Population

**File:** `C:/scripts/SLFConversion/Source/SLFConversion/Widgets/W_Status.cpp`

The W_Status widget finds the StatManagerComponent but never:
- Waits for OnStatsInitialized
- Calls GetAllStats() to get the stat data
- Iterates over W_Status_StatBlock children
- Calls SetupCurrentStats() on each stat block

### Problem 3: InitStatEntry_Implementation is Empty (W_StatEntry_Status)

**File:** `C:/scripts/SLFConversion/Source/SLFConversion/Widgets/W_Status_StatBlock.cpp`

```cpp
void UW_StatEntry_Status::InitStatEntry_Implementation()
{
    // Initialize the stat entry widget with stat display data
    UE_LOG(LogTemp, Log, TEXT("UW_StatEntry_Status::InitStatEntry"));
    // NOTHING ACTUALLY HAPPENS HERE!
}
```

Note: The base class `W_StatEntry` does have a real implementation, but W_StatEntry_Status doesn't call it and doesn't use its functionality.

### Problem 4: Missing BPI_GenericCharacter Interface Usage

The Blueprint uses the `BPI_GenericCharacter` interface to get the StatManager:

```
GetOwningPlayerPawn() -> Cast to BPI_GenericCharacter -> GetStatManager() -> StatManager
```

The C++ code uses direct component lookup instead:
```cpp
StatManagerComponent = Pawn->FindComponentByClass<UStatManagerComponent>();
```

While this might work, it bypasses the interface pattern that the rest of the system relies on.

---

## Missing Components

### 1. W_Status_StatBlock Child Widget References

W_Status has multiple W_Status_StatBlock children in the Blueprint hierarchy:
- `W_StatBlock_Status` (for base stats)
- `W_StatBlock_Status_1`
- `W_StatBlock_Status_3`
- `W_StatBlock_Status_5`
- `W_StatBlock_Status_7`
- `W_StatBlock_Status_8`

The C++ doesn't bind to these or iterate over them.

### 2. StatsVerticalBox Widget in W_Status_StatBlock

Each W_Status_StatBlock has a `StatsVerticalBox` (or similar) where it adds W_StatEntry_Status children. This needs to be:
- Bound via BindWidgetOptional or found by name
- Cleared before populating
- Used as the target for AddChildToVerticalBox

### 3. Category Filtering Logic

Each W_Status_StatBlock has a `Category` FGameplayTag property that filters which stats it displays. The SetupCurrentStats function must:
- Check each stat's category against the block's Category
- Only create widgets for matching stats

### 4. OnStatsInitialized Event Binding

The Blueprint waits for the StatManager's `OnStatsInitialized` event before populating stats. This is critical because stats are created asynchronously during BeginPlay.

---

## Recommended Fixes

### Fix 1: Implement SetupCurrentStats in W_Status_StatBlock

```cpp
void UW_Status_StatBlock::SetupCurrentStats_Implementation(const TArray<UB_Stat*>& StatObjects, const TMap<FGameplayTag, TSubclassOf<UB_Stat>>& StatClassesAndCategories)
{
    UE_LOG(LogTemp, Log, TEXT("[W_Status_StatBlock] SetupCurrentStats - %d stats, Category: %s"),
        StatObjects.Num(), *Category.ToString());

    // Get VerticalBox to add children to
    UVerticalBox* StatsBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("StatsVerticalBox")));
    if (!StatsBox)
    {
        StatsBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("StatBox")));
    }

    if (!StatsBox)
    {
        UE_LOG(LogTemp, Error, TEXT("[W_Status_StatBlock] Could not find StatsVerticalBox!"));
        return;
    }

    // Clear existing children
    StatsBox->ClearChildren();
    StatEntries.Empty();

    // Get the W_StatEntry_Status class to spawn
    UClass* StatEntryClass = LoadClass<UUserWidget>(nullptr,
        TEXT("/Game/SoulslikeFramework/Widgets/Stats/W_StatEntry_Status.W_StatEntry_Status_C"));

    if (!StatEntryClass)
    {
        UE_LOG(LogTemp, Error, TEXT("[W_Status_StatBlock] Could not load W_StatEntry_Status class!"));
        return;
    }

    // For each stat, check category and create widget
    for (UB_Stat* StatObj : StatObjects)
    {
        if (!StatObj) continue;

        // Get stat's category tag from the map
        FGameplayTag StatCategory;
        for (const auto& Pair : StatClassesAndCategories)
        {
            if (StatObj->IsA(Pair.Key))
            {
                StatCategory = Pair.Value;
                break;
            }
        }

        // Check if this stat belongs in this block (by category)
        if (!Category.IsValid() || Category.MatchesTag(StatCategory))
        {
            // Create W_StatEntry_Status widget
            UW_StatEntry_Status* NewEntry = CreateWidget<UW_StatEntry_Status>(GetOwningPlayer(), StatEntryClass);
            if (NewEntry)
            {
                // Set the Stat property BEFORE adding to parent (NativeConstruct reads it)
                NewEntry->Stat = StatObj;

                // Add to vertical box
                StatsBox->AddChildToVerticalBox(NewEntry);

                // Track in array
                StatEntries.Add(NewEntry);

                UE_LOG(LogTemp, Log, TEXT("[W_Status_StatBlock] Created entry for stat: %s"),
                    *StatObj->StatInfo.DisplayName.ToString());
            }
        }
    }

    UE_LOG(LogTemp, Log, TEXT("[W_Status_StatBlock] Created %d stat entries"), StatEntries.Num());
}
```

### Fix 2: Add Stat Population to W_Status

```cpp
// In W_Status.h - add references to stat blocks
UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
UW_Status_StatBlock* W_StatBlock_Status;

UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
UW_Status_StatBlock* W_StatBlock_Status_1;

// ... etc for other stat blocks

// Add event handler
UFUNCTION()
void OnStatsInitializedHandler();

// In W_Status.cpp NativeConstruct:
void UW_Status::NativeConstruct()
{
    Super::NativeConstruct();

    // ... existing component finding code ...

    if (StatManagerComponent)
    {
        // Bind to OnStatsInitialized to know when stats are ready
        StatManagerComponent->OnStatsInitialized.AddDynamic(this, &UW_Status::OnStatsInitializedHandler);

        // If stats are already initialized, populate now
        // Check if ActiveStats map is populated
        if (StatManagerComponent->ActiveStats.Num() > 0)
        {
            OnStatsInitializedHandler();
        }
    }
}

void UW_Status::OnStatsInitializedHandler()
{
    if (!StatManagerComponent) return;

    // Get all stats from StatManager
    TArray<UObject*> StatObjects;
    TMap<TSubclassOf<UObject>, FGameplayTag> StatClassesAndCategories;
    StatManagerComponent->GetAllStats(StatObjects, StatClassesAndCategories);

    UE_LOG(LogTemp, Log, TEXT("[W_Status] Populating stat blocks with %d stats"), StatObjects.Num());

    // Convert to UB_Stat array
    TArray<UB_Stat*> Stats;
    for (UObject* Obj : StatObjects)
    {
        if (UB_Stat* Stat = Cast<UB_Stat>(Obj))
        {
            Stats.Add(Stat);
        }
    }

    // Populate each stat block
    TArray<UW_Status_StatBlock*> StatBlocks = {
        W_StatBlock_Status, W_StatBlock_Status_1,
        // ... add all stat blocks
    };

    for (UW_Status_StatBlock* Block : StatBlocks)
    {
        if (Block)
        {
            Block->SetupCurrentStats(Stats, StatClassesAndCategories);
        }
    }
}
```

### Fix 3: Implement InitStatEntry in W_StatEntry_Status

The W_StatEntry base class has a working implementation. Either:
1. Have W_StatEntry_Status inherit from W_StatEntry and call Super
2. Or implement the full logic in W_StatEntry_Status directly

```cpp
void UW_StatEntry_Status::InitStatEntry_Implementation()
{
    if (!Stat)
    {
        UE_LOG(LogTemp, Warning, TEXT("[W_StatEntry_Status] InitStatEntry - No Stat assigned!"));
        return;
    }

    // Get stat info
    const FStatInfo& Info = Stat->StatInfo;

    // Find text widgets
    UTextBlock* StatNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("StatName")));
    UTextBlock* StatValueText = Cast<UTextBlock>(GetWidgetFromName(TEXT("StatValue")));

    // Set name
    if (StatNameText)
    {
        StatNameText->SetText(Info.DisplayName);
    }

    // Format and set value
    FString ValueStr;
    if (Info.bDisplayAsPercent)
    {
        ValueStr = FString::Printf(TEXT("%.0f%%"), Info.CurrentValue);
    }
    else if (Info.bShowMaxValue)
    {
        ValueStr = FString::Printf(TEXT("%.0f / %.0f"), Info.CurrentValue, Info.MaxValue);
    }
    else
    {
        ValueStr = FString::Printf(TEXT("%.0f"), Info.CurrentValue);
    }

    if (StatValueText)
    {
        StatValueText->SetText(FText::FromString(ValueStr));
    }

    // Bind to stat updates
    Stat->OnStatUpdated.AddDynamic(this, &UW_StatEntry_Status::EventOnStatUpdated);

    UE_LOG(LogTemp, Log, TEXT("[W_StatEntry_Status] Initialized: %s = %s"),
        *Info.DisplayName.ToString(), *ValueStr);
}
```

---

## Why Placeholder Values (99, 999) Appear

The placeholder values likely come from:
1. **Design-time values** set in the UMG designer for preview
2. **Default widget template** that has hardcoded values
3. **Missing stat data** causing widgets to show uninitialized state

Since `SetupCurrentStats` never runs, the W_StatEntry_Status widgets are never created at runtime. If you see any stat widgets at all, they're either:
- Pre-existing in the UMG hierarchy (design-time only)
- Created with null Stat references (showing default/garbage values)

---

## Testing Checklist

After implementing fixes:

1. [ ] Open Status menu in-game
2. [ ] Verify stat names appear correctly (Health, Stamina, Vigor, etc.)
3. [ ] Verify stat values match actual character stats
4. [ ] Change a stat value (take damage, level up) and verify display updates
5. [ ] Close and reopen Status menu - values should persist correctly
6. [ ] Check log output for stat initialization messages

---

## Summary

| Component | Blueprint Does | C++ Does | Fix Needed |
|-----------|---------------|----------|------------|
| W_Status | Binds to OnStatsInitialized, calls SetupCurrentStats on each block | Only gets level/currency | Add stat population logic |
| W_Status_StatBlock.SetupCurrentStats | Creates W_StatEntry_Status for each stat, filters by category | Empty stub | Full implementation |
| W_StatEntry_Status.InitStatEntry | Reads Stat.StatInfo, sets text widgets | Empty stub | Full implementation or inherit from W_StatEntry |
| Category filtering | Checks stat category against block's Category tag | Not implemented | Add to SetupCurrentStats |
| OnStatsInitialized binding | Waits for stats to be ready | Not bound | Add binding and handler |

**Priority:** HIGH - This is a core UI feature that is completely non-functional.
