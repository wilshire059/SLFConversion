// W_DebugWindow.h
// C++ Widget class for W_DebugWindow
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_DebugWindow.json
// Parent: UUserWidget
// Variables: 7 | Functions: 23 | Dispatchers: 0

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "InputMappingContext.h"
#include "GameFramework/InputSettings.h"
#include "GenericPlatform/GenericWindow.h"
#include "MediaPlayer.h"


#include "W_DebugWindow.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types
class UAC_DebugCentral;

// Forward declarations for SaveGame types


// Event Dispatchers


UCLASS()
class SLFCONVERSION_API UW_DebugWindow : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_DebugWindow(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (7)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FTimerHandle WindowLocationTimer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FVector2D MousePos;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FVector2D OffsetPos;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FTimerHandle WindowScaleTimer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	ESLFDebugWindowScaleType ScaleType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FGameplayTag ComponentTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UAC_DebugCentral* DebugCentral;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (23)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_DebugWindow")
	void SetMovePositionClickDiff();
	virtual void SetMovePositionClickDiff_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_DebugWindow")
	void SetScaleClickDiff();
	virtual void SetScaleClickDiff_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_DebugWindow")
	void ToggleVisibility();
	virtual void ToggleVisibility_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_DebugWindow")
	FString ParseActions(const TMap<FGameplayTag, FGameplayTag>& TargetMap);
	virtual FString ParseActions_Implementation(const TMap<FGameplayTag, FGameplayTag>& TargetMap);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_DebugWindow")
	void ParseBuffs(const TArray<UB_Buff*>& Buffs, FString& OutActions, FString& OutActions1);
	virtual void ParseBuffs_Implementation(const TArray<UB_Buff*>& Buffs, FString& OutActions, FString& OutActions1);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_DebugWindow")
	void ParseActors(const TArray<AActor*>& Actors, FString& OutActions, FString& OutActions1);
	virtual void ParseActors_Implementation(const TArray<AActor*>& Actors, FString& OutActions, FString& OutActions1);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_DebugWindow")
	void ParseTagContainer(const FGameplayTagContainer& Container, bool ShortTagName, FString& OutActions, FString& OutActions1);
	virtual void ParseTagContainer_Implementation(const FGameplayTagContainer& Container, bool ShortTagName, FString& OutActions, FString& OutActions1);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_DebugWindow")
	FString ParseEquippedItems(const TMap<FGameplayTag, FGameplayTag>& TargetMap);
	virtual FString ParseEquippedItems_Implementation(const TMap<FGameplayTag, FGameplayTag>& TargetMap);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_DebugWindow")
	FString ParseSpawnedEquipment(const TMap<FGameplayTag, FGameplayTag>& TargetMap);
	virtual FString ParseSpawnedEquipment_Implementation(const TMap<FGameplayTag, FGameplayTag>& TargetMap);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_DebugWindow")
	void ParseTraceChannels(const TArray<TEnumAsByte<EObjectTypeQuery>>& Array, FString& OutActions, FString& OutActions1);
	virtual void ParseTraceChannels_Implementation(const TArray<TEnumAsByte<EObjectTypeQuery>>& Array, FString& OutActions, FString& OutActions1);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_DebugWindow")
	void ParseNameArray(const TArray<FName>& Array, FString& OutActions, FString& OutActions1);
	virtual void ParseNameArray_Implementation(const TArray<FName>& Array, FString& OutActions, FString& OutActions1);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_DebugWindow")
	void ParseInventoryCategories(const TArray<FSLFInventoryCategory>& Categories, FString& OutActions, FString& OutActions1);
	virtual void ParseInventoryCategories_Implementation(const TArray<FSLFInventoryCategory>& Categories, FString& OutActions, FString& OutActions1);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_DebugWindow")
	FString ParseInventoryItems(const TMap<FGameplayTag, UPrimaryDataAsset*>& TargetMap);
	virtual FString ParseInventoryItems_Implementation(const TMap<FGameplayTag, UPrimaryDataAsset*>& TargetMap);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_DebugWindow")
	FString ParseStatusEffects(const TMap<FGameplayTag, FGameplayTag>& TargetMap);
	virtual FString ParseStatusEffects_Implementation(const TMap<FGameplayTag, FGameplayTag>& TargetMap);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_DebugWindow")
	FString ParseStats(const TMap<FGameplayTag, TSubclassOf<UB_Stat>>& TargetMap);
	virtual FString ParseStats_Implementation(const TMap<FGameplayTag, TSubclassOf<UB_Stat>>& TargetMap);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_DebugWindow")
	FString ParseStatOverrides(const TMap<FGameplayTag, FGameplayTag>& TargetMap);
	virtual FString ParseStatOverrides_Implementation(const TMap<FGameplayTag, FGameplayTag>& TargetMap);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_DebugWindow")
	FString ParseActiveStats(const TMap<FGameplayTag, FGameplayTag>& TargetMap);
	virtual FString ParseActiveStats_Implementation(const TMap<FGameplayTag, FGameplayTag>& TargetMap);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_DebugWindow")
	FString ParseStatsToAffect(const TMap<FGameplayTag, FGameplayTag>& TargetMap);
	virtual FString ParseStatsToAffect_Implementation(const TMap<FGameplayTag, FGameplayTag>& TargetMap);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_DebugWindow")
	void ParseStatSoftcaps(const TArray<FAffectedStat>& Array, FString& OutActions, FString& OutActions1);
	virtual void ParseStatSoftcaps_Implementation(const TArray<FAffectedStat>& Array, FString& OutActions, FString& OutActions1);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_DebugWindow")
	FString ParseCardinals(const TMap<FGameplayTag, FGameplayTag>& TargetMap);
	virtual FString ParseCardinals_Implementation(const TMap<FGameplayTag, FGameplayTag>& TargetMap);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_DebugWindow")
	FString ParseProgress(const TMap<FGameplayTag, FGameplayTag>& TargetMap);
	virtual FString ParseProgress_Implementation(const TMap<FGameplayTag, FGameplayTag>& TargetMap);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_DebugWindow")
	FString ParseLevelUpCosts(UCurveFloat* Curve);
	virtual FString ParseLevelUpCosts_Implementation(UCurveFloat* Curve);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_DebugWindow")
	FString ParseSaveDataEntries(const FSLFSaveGameInfo& FSaveGameInfo);
	virtual FString ParseSaveDataEntries_Implementation(const FSLFSaveGameInfo& FSaveGameInfo);


	// Event Handlers (5 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_DebugWindow")
	void EventStopUpdateWindowLocation();
	virtual void EventStopUpdateWindowLocation_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_DebugWindow")
	void EventStopWindowScale();
	virtual void EventStopWindowScale_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_DebugWindow")
	void EventUpdateComponentData();
	virtual void EventUpdateComponentData_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_DebugWindow")
	void EventUpdateWindowLocation();
	virtual void EventUpdateWindowLocation_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_DebugWindow")
	void EventUpdateWindowScale();
	virtual void EventUpdateWindowScale_Implementation();

protected:
	// Cache references
	void CacheWidgetReferences();
};
