# Crafting System Peer Review - Request for Gemini

## Context
This is a Blueprint-to-C++ migration project for a Soulslike Framework in Unreal Engine 5.7. The crafting system widgets were migrated but had incomplete implementations that were just fixed.

## Review Request
Please review the following code changes for:
1. Correctness - Does the logic properly consume materials and query inventory?
2. Memory safety - Any potential null pointer issues or memory leaks?
3. Edge cases - What happens with 0 amounts, invalid tags, empty inventory?
4. Performance - Any concerns with the iteration patterns?
5. API design - Is RemoveItemWithTag the right abstraction?

---

## Change 1: New RemoveItemWithTag Function (InventoryManagerComponent)

**File: Source/SLFConversion/Components/InventoryManagerComponent.h**
```cpp
/** Remove item from inventory by gameplay tag (for crafting material consumption) */
UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory|Items")
bool RemoveItemWithTag(const FGameplayTag& Tag, int32 Count);
virtual bool RemoveItemWithTag_Implementation(const FGameplayTag& Tag, int32 Count);
```

**File: Source/SLFConversion/Components/InventoryManagerComponent.cpp**
```cpp
bool UInventoryManagerComponent::RemoveItemWithTag_Implementation(const FGameplayTag& Tag, int32 Count)
{
    if (!Tag.IsValid() || Count <= 0)
    {
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("[InventoryManager] RemoveItemWithTag: %s x%d"), *Tag.ToString(), Count);

    int32 RemainingToRemove = Count;

    // Iterate through items and remove by tag match
    TArray<int32> KeysToRemove;
    for (auto& Pair : Items)
    {
        if (RemainingToRemove <= 0)
        {
            break;
        }

        UPDA_Item* ItemData = Cast<UPDA_Item>(Pair.Value.ItemAsset);
        if (ItemData && ItemData->ItemInformation.ItemTag.MatchesTag(Tag))
        {
            int32 OldAmount = Pair.Value.Amount;
            int32 ToRemoveFromThis = FMath::Min(RemainingToRemove, Pair.Value.Amount);
            UDataAsset* ItemAsDataAsset = Cast<UDataAsset>(Pair.Value.ItemAsset);

            if (Pair.Value.Amount <= ToRemoveFromThis)
            {
                // Remove entire stack
                KeysToRemove.Add(Pair.Key);
                OnItemAmountUpdated.Broadcast(ItemAsDataAsset, OldAmount, 0);
            }
            else
            {
                // Reduce stack
                Pair.Value.Amount -= ToRemoveFromThis;
                OnItemAmountUpdated.Broadcast(ItemAsDataAsset, OldAmount, Pair.Value.Amount);
            }

            RemainingToRemove -= ToRemoveFromThis;
            UE_LOG(LogTemp, Log, TEXT("  Removed %d from %s, %d remaining to remove"),
                ToRemoveFromThis, *ItemData->GetName(), RemainingToRemove);
        }
    }

    // Remove empty stacks
    for (int32 Key : KeysToRemove)
    {
        Items.Remove(Key);
    }

    if (RemainingToRemove < Count)
    {
        // Something was removed
        OnInventoryUpdated.Broadcast();
    }

    return RemainingToRemove == 0;
}
```

**Data Structure Context:**
```cpp
// Items is: TMap<int32, FSLFInventoryItem> Items;
// Where FSLFInventoryItem contains:
//   UObject* ItemAsset;  // Actually UPDA_Item*
//   int32 Amount;
```

---

## Change 2: New RemoveItemWithTag Function (AC_InventoryManager)

**File: Source/SLFConversion/Components/AC_InventoryManager.cpp**
```cpp
bool UAC_InventoryManager::RemoveItemWithTag_Implementation(const FGameplayTag& Tag, int32 Count)
{
    UE_LOG(LogTemp, Log, TEXT("UAC_InventoryManager::RemoveItemWithTag - Tag: %s, Count: %d"),
        *Tag.ToString(), Count);

    if (!Tag.IsValid() || Count <= 0)
    {
        return false;
    }

    // In this implementation, Items is TMap<FGameplayTag, UPrimaryDataAsset*>
    // We need to find items whose ItemTag matches the requested tag
    // Then remove them from Items map

    TArray<FGameplayTag> TagsToRemove;
    int32 RemainingToRemove = Count;

    for (const auto& ItemEntry : Items)
    {
        if (RemainingToRemove <= 0)
        {
            break;
        }

        // Check if this item's ItemTag matches
        UPDA_Item* ItemData = Cast<UPDA_Item>(ItemEntry.Value);
        if (ItemData && ItemData->ItemInformation.ItemTag.MatchesTag(Tag))
        {
            TagsToRemove.Add(ItemEntry.Key);
            RemainingToRemove--;
            UE_LOG(LogTemp, Log, TEXT("  Found item to remove: %s"), *ItemData->GetName());
        }
    }

    // Remove the found items
    for (const FGameplayTag& TagToRemove : TagsToRemove)
    {
        if (UPrimaryDataAsset** ItemPtr = Items.Find(TagToRemove))
        {
            OnItemAmountUpdated.Broadcast(*ItemPtr, 0);
        }
        Items.Remove(TagToRemove);
    }

    if (TagsToRemove.Num() > 0)
    {
        OnInventoryUpdated.Broadcast();
    }

    return RemainingToRemove == 0;
}
```

**Data Structure Context:**
```cpp
// Items is: TMap<FGameplayTag, UPrimaryDataAsset*> Items;
// This is a different structure - each entry is a single item, not a stack
```

---

## Change 3: Fixed GetOwnedAmount (W_CraftingEntry)

**File: Source/SLFConversion/Widgets/W_CraftingEntry.cpp**

**Before (stub):**
```cpp
int32 UW_CraftingEntry::GetOwnedAmount_Implementation()
{
    return 0;  // Always returned 0!
}
```

**After:**
```cpp
int32 UW_CraftingEntry::GetOwnedAmount_Implementation()
{
    // Return the amount of this item currently owned by the player
    if (!IsValid(RequiredItem))
    {
        return 0;
    }

    // Get inventory manager from player controller's pawn
    APlayerController* PC = GetOwningPlayer();
    if (!PC)
    {
        return 0;
    }

    APawn* Pawn = PC->GetPawn();
    if (!Pawn)
    {
        return 0;
    }

    // Try AC_InventoryManager first (on Pawn)
    UAC_InventoryManager* InventoryManager = Pawn->FindComponentByClass<UAC_InventoryManager>();
    if (InventoryManager)
    {
        // Get item tag from the required item
        UPDA_Item* ItemData = Cast<UPDA_Item>(RequiredItem);
        if (ItemData && ItemData->ItemInformation.ItemTag.IsValid())
        {
            int32 Amount = 0;
            bool bSuccess = false;
            InventoryManager->GetAmountOfItemWithTag(ItemData->ItemInformation.ItemTag, Amount, bSuccess);
            if (bSuccess)
            {
                OwnedAmount = Amount;
                return Amount;
            }
        }

        // Fallback: query by asset directly
        int32 Amount = 0;
        bool bSuccess = false;
        InventoryManager->GetAmountOfItem(RequiredItem, Amount, bSuccess);
        OwnedAmount = Amount;
        return Amount;
    }

    return 0;
}
```

---

## Change 4: Updated CraftItem to Consume Materials

**File: Source/SLFConversion/Widgets/W_CraftingAction.cpp**

**Before (no material consumption):**
```cpp
for (const auto& Requirement : RequiredItems)
{
    // ... logging only, no actual removal
    // TODO comment about needing implementation
}
```

**After:**
```cpp
for (const auto& Requirement : RequiredItems)
{
    FGameplayTag ItemTag = Requirement.Key;
    int32 RequiredAmount = Requirement.Value;

    if (!ItemTag.IsValid())
    {
        continue;
    }

    int32 TotalToConsume = RequiredAmount * CurrentAmount;

    UE_LOG(LogTemp, Log, TEXT("  Consuming %s x%d"), *ItemTag.ToString(), TotalToConsume);

    // Remove materials by tag
    bool bSuccess = InventoryManager->RemoveItemWithTag(ItemTag, TotalToConsume);
    if (!bSuccess)
    {
        UE_LOG(LogTemp, Warning, TEXT("  Failed to consume %s x%d - not enough materials!"),
            *ItemTag.ToString(), TotalToConsume);
    }
}
```

---

## Crafting Data Structure Reference

```cpp
// How crafting requirements are stored on items:
struct FSLFCraftingInfo
{
    bool bCanBeCrafted = false;
    TMap<FGameplayTag, int32> RequiredItems;  // Tag -> Amount needed
};

struct FSLFItemInfo
{
    FGameplayTag ItemTag;  // This item's identifier tag
    // ... other fields ...
    FSLFCraftingInfo CraftingDetails;
};

// On UPDA_Item:
FSLFItemInfo ItemInformation;
```

---

## Questions for Review

1. **Two Different Inventory Systems**: Note that `InventoryManagerComponent` uses `TMap<int32, FSLFInventoryItem>` (slot-based with stacks) while `AC_InventoryManager` uses `TMap<FGameplayTag, UPrimaryDataAsset*>` (tag-keyed, single items). Is this intentional duplication or technical debt?

2. **Tag Matching**: Using `MatchesTag()` allows parent/child tag matching (e.g., `Item.Material.Iron` would match `Item.Material`). Is this the desired behavior for crafting, or should it use `MatchesTagExact()`?

3. **Partial Consumption**: If crafting requires 5 iron but player only has 3, the current code will consume all 3 and continue. Should it fail atomically (all-or-nothing)?

4. **Event Broadcasting**: `OnItemAmountUpdated` is broadcast per-stack removed. For large removals across many stacks, this could be many events. Should there be a batch option?

5. **Thread Safety**: These functions modify `Items` map while iterating. The current approach collects keys to remove in a separate array first. Is this sufficient?

---

## Build Status
All changes compile successfully on UE 5.7.

---

## Fixes Applied Based on Gemini Review

### Issue 1: Partial Consumption Bug (FIXED)
**Gemini Identified:** RemoveItemWithTag modified inventory before verifying availability - could lose materials if not enough.

**Fix Applied:** Both `InventoryManagerComponent::RemoveItemWithTag` and `AC_InventoryManager::RemoveItemWithTag` now use a **two-pass atomic pattern**:

```cpp
// PASS 1: Dry run - Check if we have enough BEFORE modifying anything
int32 TotalAvailable = 0;
for (const auto& Pair : Items)
{
    UPDA_Item* ItemData = Cast<UPDA_Item>(Pair.Value.ItemAsset);
    if (ItemData && ItemData->ItemInformation.ItemTag.MatchesTag(Tag))
    {
        TotalAvailable += Pair.Value.Amount;
    }
}

if (TotalAvailable < Count)
{
    UE_LOG(LogTemp, Warning, TEXT("  ATOMIC CHECK FAILED: Need %d but only have %d - no items removed"),
        Count, TotalAvailable);
    return false; // Fail gracefully, touch nothing
}

// PASS 2: Commit - Actually remove (we verified we have enough)
// ... removal logic
```

### Issue 2: CraftItem Non-Atomic (FIXED)
**Gemini Identified:** CraftItem consumed materials one-by-one without first checking ALL are available.

**Fix Applied:** `W_CraftingAction::CraftItem_Implementation` now has a validation pass before any consumption:

```cpp
// PASS 1: VALIDATION - Check ALL materials are available BEFORE consuming any
for (const auto& Requirement : RequiredItems)
{
    int32 TotalNeeded = RequiredAmount * CurrentAmount;
    int32 OwnedAmount = 0;
    InventoryManager->GetAmountOfItemWithTag(ItemTag, OwnedAmount, bHasItem);

    if (OwnedAmount < TotalNeeded)
    {
        UE_LOG(LogTemp, Warning, TEXT("  CRAFT FAILED: Need %d of %s but only have %d - aborting craft (no materials consumed)"),
            TotalNeeded, *ItemTag.ToString(), OwnedAmount);
        return; // Fail entire craft BEFORE removing anything
    }
}

// PASS 2: EXECUTION - Only if all materials verified, now consume them
for (const auto& Requirement : RequiredItems)
{
    InventoryManager->RemoveItemWithTag(ItemTag, TotalToConsume);
}
```

### Issue 3: Component Disconnect (FIXED)
**Gemini Identified:** W_CraftingEntry only looked for AC_InventoryManager but we added code to InventoryManagerComponent too.

**Fix Applied:** Both `W_CraftingEntry` and `W_CraftingAction` now check for **both inventory components**:

```cpp
// Try AC_InventoryManager first
UAC_InventoryManager* InventoryManager = Pawn->FindComponentByClass<UAC_InventoryManager>();

// Fallback to InventoryManagerComponent (slot-based with stacks)
UInventoryManagerComponent* InvManagerComp = nullptr;
if (!InventoryManager)
{
    InvManagerComp = Pawn->FindComponentByClass<UInventoryManagerComponent>();
}

// Use whichever is available
if (InventoryManager)
{
    InventoryManager->GetAmountOfItemWithTag(ItemTag, OwnedAmount, bSuccess);
}
else if (InvManagerComp)
{
    InvManagerComp->GetAmountOfItemWithTag(ItemTag, OwnedAmount, bSuccess);
}
```

---

## Summary of All Changes

| File | Change |
|------|--------|
| `InventoryManagerComponent.h` | Added `RemoveItemWithTag` declaration |
| `InventoryManagerComponent.cpp` | Added atomic `RemoveItemWithTag` with two-pass pattern |
| `AC_InventoryManager.h` | Added `RemoveItemWithTag` declaration |
| `AC_InventoryManager.cpp` | Added atomic `RemoveItemWithTag` with two-pass pattern |
| `W_CraftingEntry.cpp` | Fixed `GetOwnedAmount` + added fallback for both inventory types |
| `W_CraftingAction.cpp` | Added validation pass + fallback for both inventory types |

**Build Status:** All changes compile successfully on UE 5.7.
