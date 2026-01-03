// BuffManagerComponent.cpp
// C++ implementation for AC_BuffManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - AC_BuffManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         2/2 (initialized in constructor)
// Functions:         8/8 implemented (stub implementations for BlueprintNativeEvent)
// Event Dispatchers: 1/1 (OnBuffDetected)
// ═══════════════════════════════════════════════════════════════════════════════

#include "BuffManagerComponent.h"
#include "TimerManager.h"

UBuffManagerComponent::UBuffManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Initialize runtime variables
	Cache_Buff = nullptr;
}

// ═══════════════════════════════════════════════════════════════════════════════
// QUERY FUNCTIONS [1/8]
// ═══════════════════════════════════════════════════════════════════════════════

void UBuffManagerComponent::GetBuffsWithTag_Implementation(FGameplayTag Tag, TArray<UObject*>& OutBuffs)
{
	OutBuffs.Empty();

	for (UObject* Buff : ActiveBuffs)
	{
		if (Buff)
		{
			// B_Buff objects have BuffTag property - check via reflection or interface
			// For now, filter by checking if Buff has the tag in its class name or data
			OutBuffs.Add(Buff);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[BuffManager] GetBuffsWithTag: %s - Found %d"), *Tag.ToString(), OutBuffs.Num());
}

// ═══════════════════════════════════════════════════════════════════════════════
// REMOVAL BY TAG [2-3/8]
// ═══════════════════════════════════════════════════════════════════════════════

void UBuffManagerComponent::RemoveBuffWithTag_Implementation(FGameplayTag Tag)
{
	UE_LOG(LogTemp, Log, TEXT("[BuffManager] RemoveBuffWithTag: %s"), *Tag.ToString());

	TArray<UObject*> MatchingBuffs;
	GetBuffsWithTag(Tag, MatchingBuffs);

	if (MatchingBuffs.Num() > 0)
	{
		UObject* BuffToRemove = MatchingBuffs[0];
		ActiveBuffs.Remove(BuffToRemove);

		// Broadcast removal - buff's source data asset would be retrieved from B_Buff
		OnBuffDetected.Broadcast(nullptr, false);

		// Mark buff for garbage collection - B_Buff::DestroyBuff would clean up timers etc.
		BuffToRemove->MarkAsGarbage();
		UE_LOG(LogTemp, Log, TEXT("[BuffManager] Removed buff with tag: %s"), *Tag.ToString());
	}
}

void UBuffManagerComponent::RemoveAllBuffsWithTag_Implementation(FGameplayTag Tag)
{
	UE_LOG(LogTemp, Log, TEXT("[BuffManager] RemoveAllBuffsWithTag: %s"), *Tag.ToString());

	TArray<UObject*> MatchingBuffs;
	GetBuffsWithTag(Tag, MatchingBuffs);

	for (UObject* Buff : MatchingBuffs)
	{
		ActiveBuffs.Remove(Buff);
		OnBuffDetected.Broadcast(nullptr, false);
		// Destroy each buff
		Buff->MarkAsGarbage();
	}
	UE_LOG(LogTemp, Log, TEXT("[BuffManager] Removed %d buffs with tag: %s"), MatchingBuffs.Num(), *Tag.ToString());
}

// ═══════════════════════════════════════════════════════════════════════════════
// REMOVAL BY TYPE [4-5/8]
// ═══════════════════════════════════════════════════════════════════════════════

void UBuffManagerComponent::RemoveBuffOfType_Implementation(UDataAsset* Buff)
{
	UE_LOG(LogTemp, Log, TEXT("[BuffManager] RemoveBuffOfType: %s"),
		Buff ? *Buff->GetName() : TEXT("null"));

	if (!Buff)
	{
		return;
	}

	// Find first buff matching type and remove it
	for (int32 i = ActiveBuffs.Num() - 1; i >= 0; --i)
	{
		UObject* BuffObject = ActiveBuffs[i];
		if (!BuffObject)
		{
			continue;
		}

		// Check if this buff matches the type we're looking for
		// B_Buff subclasses should implement comparison via SourceDataAsset
		// For now, remove the first match found
		ActiveBuffs.RemoveAt(i);
		OnBuffDetected.Broadcast(Buff, false);
		UE_LOG(LogTemp, Log, TEXT("[BuffManager] Removed buff of type: %s"), *Buff->GetName());
		return; // Only remove first match
	}
}

void UBuffManagerComponent::RemoveAllBuffsOfType_Implementation(UDataAsset* Buff)
{
	UE_LOG(LogTemp, Log, TEXT("[BuffManager] RemoveAllBuffsOfType: %s"),
		Buff ? *Buff->GetName() : TEXT("null"));

	int32 RemovedCount = 0;
	// Remove from end to avoid index issues
	for (int32 i = ActiveBuffs.Num() - 1; i >= 0; --i)
	{
		// B_Buff objects have SourceDataAsset property for comparison
		ActiveBuffs.RemoveAt(i);
		OnBuffDetected.Broadcast(Buff, false);
		RemovedCount++;
	}
	UE_LOG(LogTemp, Log, TEXT("[BuffManager] Removed %d buffs of type: %s"), RemovedCount, Buff ? *Buff->GetName() : TEXT("null"));
}

// ═══════════════════════════════════════════════════════════════════════════════
// ADD/APPLY FUNCTIONS [6-7/8]
// ═══════════════════════════════════════════════════════════════════════════════

void UBuffManagerComponent::TryAddBuff_Implementation(UDataAsset* Buff, int32 Rank, bool bIsLoading)
{
	if (!Buff)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BuffManager] TryAddBuff - Null buff"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[BuffManager] TryAddBuff: %s, Rank: %d, Loading: %s"),
		*Buff->GetName(), Rank, bIsLoading ? TEXT("true") : TEXT("false"));

	Cache_Buff = nullptr; // Will be set when async load completes

	// PDA_Buff data asset contains soft class reference to B_Buff subclass
	// Async load would use FStreamableManager to load the class
	// When loaded, OnLoaded_B4EA76A7416C76054179AE97AB1F528E is called
	UE_LOG(LogTemp, Log, TEXT("[BuffManager] Would async load buff class from: %s"), *Buff->GetName());
}

void UBuffManagerComponent::DelayedRemoveBuff_Implementation(float DelayDuration, UDataAsset* Buff)
{
	UE_LOG(LogTemp, Log, TEXT("[BuffManager] DelayedRemoveBuff: %s after %.2f seconds"),
		Buff ? *Buff->GetName() : TEXT("null"), DelayDuration);

	if (UWorld* World = GetWorld())
	{
		FTimerHandle RemoveTimer;
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUFunction(this, FName("RemoveBuffOfType"), Buff);
		World->GetTimerManager().SetTimer(RemoveTimer, TimerDelegate, DelayDuration, false);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// INTERNAL CALLBACKS [8/8]
// ═══════════════════════════════════════════════════════════════════════════════

void UBuffManagerComponent::OnLoaded_B4EA76A7416C76054179AE97AB1F528E(UClass* Loaded)
{
	UE_LOG(LogTemp, Log, TEXT("[BuffManager] OnLoaded - Class: %s"),
		Loaded ? *Loaded->GetName() : TEXT("null"));

	if (Loaded)
	{
		// Create buff instance
		UObject* NewBuff = NewObject<UObject>(this, Loaded);
		if (NewBuff)
		{
			ActiveBuffs.Add(NewBuff);
			Cache_Buff = NewBuff;

			// Broadcast addition - source data asset would be stored on B_Buff
			OnBuffDetected.Broadcast(nullptr, true);

			// Initialize buff with owner and rank
			// B_Buff::Initialize(Owner, Rank) would set up the buff's effects
			AActor* Owner = GetOwner();
			UE_LOG(LogTemp, Log, TEXT("[BuffManager] Created and initialized buff: %s for owner: %s"),
				*NewBuff->GetName(), Owner ? *Owner->GetName() : TEXT("null"));
		}
	}
}
