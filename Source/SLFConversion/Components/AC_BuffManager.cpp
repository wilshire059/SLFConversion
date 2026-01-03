// AC_BuffManager.cpp
// C++ component implementation for AC_BuffManager
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Component/AC_BuffManager.json
//
// PASS 8: Full logic implementation from Blueprint graphs
// PASS 10: Debug logging added

#include "Components/AC_BuffManager.h"
#include "Blueprints/B_Buff.h"
#include "TimerManager.h"
#include "Engine/World.h"

UAC_BuffManager::UAC_BuffManager()
{
	PrimaryComponentTick.bCanEverTick = true;
	Cache_Buff = nullptr;
}

void UAC_BuffManager::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("UAC_BuffManager::BeginPlay"));
}

void UAC_BuffManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


// ═══════════════════════════════════════════════════════════════════════
// FUNCTION IMPLEMENTATIONS (From Blueprint Graph Analysis)
// ═══════════════════════════════════════════════════════════════════════

/**
 * TryAddBuff - Async loads and constructs a buff, grants it to the owner
 *
 * Blueprint Logic (EventGraph - Event TryAddBuff):
 * 1. Get Class from Buff (PDA_Buff->Class soft reference)
 * 2. Async Load Class Asset
 * 3. Cast to B_Buff class
 * 4. Construct Object from Class (with Rank, BuffData, OwnerActor)
 * 5. If IsLoading: Call Event OnRemoved (for save/load stat duping prevention)
 * 6. Else: Call Event OnGranted
 * 7. Add to ActiveBuffs array
 * 8. Broadcast OnBuffDetected(Buff, true)
 *
 * Note: For C++ we use synchronous loading via TSubclassOf or LoadClass
 */
void UAC_BuffManager::TryAddBuff_Implementation(UPrimaryDataAsset* Buff, int32 Rank, bool IsLoading)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_BuffManager::TryAddBuff - Buff: %s, Rank: %d, IsLoading: %s"),
		Buff ? *Buff->GetName() : TEXT("None"), Rank, IsLoading ? TEXT("true") : TEXT("false"));

	if (!IsValid(Buff))
	{
		UE_LOG(LogTemp, Warning, TEXT("  Invalid Buff passed"));
		return;
	}

	// In Blueprint, this is done via async loading of the soft class reference
	// For C++, we construct the buff directly using the default UB_Buff class
	// Child buff classes would be loaded via the soft class reference in PDA_Buff
	UB_Buff* NewBuff = NewObject<UB_Buff>(this);
	if (!IsValid(NewBuff))
	{
		UE_LOG(LogTemp, Error, TEXT("  Failed to construct buff object"));
		return;
	}

	// Initialize buff properties (matches Construct Object from Class pins)
	NewBuff->Rank = Rank;
	NewBuff->BuffData = Buff;
	NewBuff->OwnerActor = GetOwner();

	// Branch on IsLoading flag
	// IsLoading = true when data is being loaded from save
	// We need to remove the buff and re-add it to prevent stat duping
	if (IsLoading)
	{
		// TRUE branch: Call OnRemoved first (clears old effects), then OnGranted
		NewBuff->EventOnRemoved();
		NewBuff->EventOnGranted();
	}
	else
	{
		// FALSE branch: Just call OnGranted
		NewBuff->EventOnGranted();
	}

	// Add to ActiveBuffs array
	ActiveBuffs.Add(NewBuff);

	// Broadcast OnBuffDetected(Buff, Added=true)
	OnBuffDetected.Broadcast(Buff, true);

	UE_LOG(LogTemp, Log, TEXT("  Buff added successfully. Total active buffs: %d"), ActiveBuffs.Num());
}

/**
 * DelayedRemoveBuff - Removes a buff after a delay
 *
 * Blueprint Logic (EventGraph - Event DelayedRemoveBuff):
 * 1. Delay for DelayDuration
 * 2. ForEachLoopWithBreak over ActiveBuffs
 * 3. For each: Get BuffData, compare with Buff parameter
 * 4. If equal: Set Cache_Buff, Break
 * 5. After loop: Call Event OnRemoved on Cache_Buff
 * 6. Remove Cache_Buff from ActiveBuffs
 * 7. Broadcast OnBuffDetected(Buff, false)
 * 8. Clear Cache_Buff
 */
void UAC_BuffManager::DelayedRemoveBuff_Implementation(float DelayDuration, UPrimaryDataAsset* Buff)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_BuffManager::DelayedRemoveBuff - Buff: %s, Delay: %f"),
		Buff ? *Buff->GetName() : TEXT("None"), DelayDuration);

	if (!IsValid(Buff))
	{
		UE_LOG(LogTemp, Warning, TEXT("  Invalid Buff passed"));
		return;
	}

	// Use timer for delayed execution
	if (UWorld* World = GetWorld())
	{
		FTimerHandle TimerHandle;
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUObject(this, &UAC_BuffManager::ExecuteDelayedRemove, Buff);
		World->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, DelayDuration, false);
	}
}

/**
 * ExecuteDelayedRemove - Internal helper for DelayedRemoveBuff
 */
void UAC_BuffManager::ExecuteDelayedRemove(UPrimaryDataAsset* Buff)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_BuffManager::ExecuteDelayedRemove - Buff: %s"),
		Buff ? *Buff->GetName() : TEXT("None"));

	// ForEachLoopWithBreak over ActiveBuffs
	for (UB_Buff* ActiveBuff : ActiveBuffs)
	{
		if (IsValid(ActiveBuff) && ActiveBuff->BuffData == Buff)
		{
			Cache_Buff = ActiveBuff;
			break;
		}
	}

	// After loop: process Cache_Buff
	if (IsValid(Cache_Buff))
	{
		// Call Event OnRemoved
		Cache_Buff->EventOnRemoved();

		// Remove from array
		ActiveBuffs.Remove(Cache_Buff);

		// Broadcast OnBuffDetected(Buff, false)
		OnBuffDetected.Broadcast(Buff, false);

		// Clear cache
		Cache_Buff = nullptr;

		UE_LOG(LogTemp, Log, TEXT("  Buff removed. Remaining active buffs: %d"), ActiveBuffs.Num());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  Buff not found in active buffs"));
	}
}

/**
 * RemoveBuffOfType - Remove the first buff matching the given type
 *
 * Blueprint Logic (Function Graph - RemoveBuffOfType):
 * 1. ForEachLoopWithBreak over ActiveBuffs
 * 2. For each: Get BuffData, compare with Buff parameter
 * 3. If equal: Set LOCAL_FoundBuff, Break
 * 4. After loop: Call Event OnRemoved on LOCAL_FoundBuff
 * 5. Remove LOCAL_FoundBuff from ActiveBuffs
 * 6. Broadcast OnBuffDetected(Buff, false)
 */
void UAC_BuffManager::RemoveBuffOfType_Implementation(UPrimaryDataAsset* Buff)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_BuffManager::RemoveBuffOfType - Buff: %s"),
		Buff ? *Buff->GetName() : TEXT("None"));

	if (!IsValid(Buff))
	{
		UE_LOG(LogTemp, Warning, TEXT("  Invalid Buff passed"));
		return;
	}

	UB_Buff* LOCAL_FoundBuff = nullptr;

	// ForEachLoopWithBreak - find first matching buff
	for (UB_Buff* ActiveBuff : ActiveBuffs)
	{
		if (IsValid(ActiveBuff) && ActiveBuff->BuffData == Buff)
		{
			LOCAL_FoundBuff = ActiveBuff;
			break;
		}
	}

	// Process found buff
	if (IsValid(LOCAL_FoundBuff))
	{
		// Call Event OnRemoved
		LOCAL_FoundBuff->EventOnRemoved();

		// Remove from array
		ActiveBuffs.Remove(LOCAL_FoundBuff);

		// Broadcast OnBuffDetected(Buff, false)
		OnBuffDetected.Broadcast(Buff, false);

		UE_LOG(LogTemp, Log, TEXT("  Buff removed. Remaining active buffs: %d"), ActiveBuffs.Num());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  Buff not found in active buffs"));
	}
}

/**
 * RemoveAllBuffsOfType - Remove ALL buffs matching the given type
 *
 * Blueprint Logic (Function Graph - RemoveAllBuffsOfType):
 * Similar to RemoveBuffOfType but uses ForEachLoop (no break)
 * Removes ALL matching buffs, not just the first one
 */
void UAC_BuffManager::RemoveAllBuffsOfType_Implementation(UPrimaryDataAsset* Buff)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_BuffManager::RemoveAllBuffsOfType - Buff: %s"),
		Buff ? *Buff->GetName() : TEXT("None"));

	if (!IsValid(Buff))
	{
		UE_LOG(LogTemp, Warning, TEXT("  Invalid Buff passed"));
		return;
	}

	// Collect all matching buffs first (can't modify array during iteration)
	TArray<UB_Buff*> BuffsToRemove;
	for (UB_Buff* ActiveBuff : ActiveBuffs)
	{
		if (IsValid(ActiveBuff) && ActiveBuff->BuffData == Buff)
		{
			BuffsToRemove.Add(ActiveBuff);
		}
	}

	// Remove all matching buffs
	for (UB_Buff* BuffToRemove : BuffsToRemove)
	{
		// Call Event OnRemoved
		BuffToRemove->EventOnRemoved();

		// Remove from array
		ActiveBuffs.Remove(BuffToRemove);

		// Broadcast OnBuffDetected(Buff, false)
		OnBuffDetected.Broadcast(Buff, false);
	}

	UE_LOG(LogTemp, Log, TEXT("  Removed %d buffs. Remaining active buffs: %d"),
		BuffsToRemove.Num(), ActiveBuffs.Num());
}

/**
 * RemoveBuffWithTag - Remove the first buff matching the given tag
 *
 * Blueprint Logic (Function Graph - RemoveBuffWithTag):
 * 1. ForEachLoopWithBreak over ActiveBuffs
 * 2. For each: Get buff's tag, compare with Tag parameter
 * 3. If matching: Set LOCAL_FoundBuff, Break
 * 4. After loop: Call Event OnRemoved, remove, broadcast
 */
void UAC_BuffManager::RemoveBuffWithTag_Implementation(const FGameplayTag& Tag)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_BuffManager::RemoveBuffWithTag - Tag: %s"), *Tag.ToString());

	UB_Buff* LOCAL_FoundBuff = nullptr;

	// ForEachLoopWithBreak - find first matching buff
	for (UB_Buff* ActiveBuff : ActiveBuffs)
	{
		if (IsValid(ActiveBuff) && ActiveBuff->GetBuffTag().MatchesTag(Tag))
		{
			LOCAL_FoundBuff = ActiveBuff;
			break;
		}
	}

	// Process found buff
	if (IsValid(LOCAL_FoundBuff))
	{
		// Call Event OnRemoved
		LOCAL_FoundBuff->EventOnRemoved();

		// Get BuffData for broadcast
		UPrimaryDataAsset* BuffData = LOCAL_FoundBuff->BuffData;

		// Remove from array
		ActiveBuffs.Remove(LOCAL_FoundBuff);

		// Broadcast OnBuffDetected(BuffData, false)
		OnBuffDetected.Broadcast(BuffData, false);

		UE_LOG(LogTemp, Log, TEXT("  Buff removed. Remaining active buffs: %d"), ActiveBuffs.Num());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  No buff found with matching tag"));
	}
}

/**
 * RemoveAllBuffsWithTag - Remove ALL buffs matching the given tag
 *
 * Blueprint Logic (Function Graph - RemoveAllBuffsWithTag):
 * Similar to RemoveBuffWithTag but uses ForEachLoop (no break)
 */
void UAC_BuffManager::RemoveAllBuffsWithTag_Implementation(const FGameplayTag& Tag)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_BuffManager::RemoveAllBuffsWithTag - Tag: %s"), *Tag.ToString());

	// Collect all matching buffs first
	TArray<UB_Buff*> BuffsToRemove;
	for (UB_Buff* ActiveBuff : ActiveBuffs)
	{
		if (IsValid(ActiveBuff) && ActiveBuff->GetBuffTag().MatchesTag(Tag))
		{
			BuffsToRemove.Add(ActiveBuff);
		}
	}

	// Remove all matching buffs
	for (UB_Buff* BuffToRemove : BuffsToRemove)
	{
		// Call Event OnRemoved
		BuffToRemove->EventOnRemoved();

		// Get BuffData for broadcast
		UPrimaryDataAsset* BuffData = BuffToRemove->BuffData;

		// Remove from array
		ActiveBuffs.Remove(BuffToRemove);

		// Broadcast OnBuffDetected(BuffData, false)
		OnBuffDetected.Broadcast(BuffData, false);
	}

	UE_LOG(LogTemp, Log, TEXT("  Removed %d buffs. Remaining active buffs: %d"),
		BuffsToRemove.Num(), ActiveBuffs.Num());
}

/**
 * GetBuffsWithTag - Get all buffs matching the given tag
 *
 * Blueprint Logic (Function Graph - GetBuffsWithTag):
 * 1. ForEachLoop over ActiveBuffs
 * 2. For each: Check if tag matches
 * 3. If matches: Add to LOCAL_Buffs array
 * 4. Return LOCAL_Buffs
 */
TArray<UB_Buff*> UAC_BuffManager::GetBuffsWithTag_Implementation(const FGameplayTag& Tag)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_BuffManager::GetBuffsWithTag - Tag: %s"), *Tag.ToString());

	TArray<UB_Buff*> LOCAL_Buffs;

	// ForEachLoop - collect all matching buffs
	for (UB_Buff* ActiveBuff : ActiveBuffs)
	{
		if (IsValid(ActiveBuff) && ActiveBuff->GetBuffTag().MatchesTag(Tag))
		{
			LOCAL_Buffs.Add(ActiveBuff);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("  Found %d buffs with matching tag"), LOCAL_Buffs.Num());
	return LOCAL_Buffs;
}
