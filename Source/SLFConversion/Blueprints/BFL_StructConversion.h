// BFL_StructConversion.h
// C++ Blueprint Function Library for FInstancedStruct conversions
//
// 20-PASS VALIDATION: 2026-01-06 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/BML_StructConversion.json
// Replaces: BML_StructConversion Blueprint Macro Library
// Functions: 27 ConvertTo* functions for extracting typed structs from FInstancedStruct

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InstancedStruct.h"
#include "SLFGameTypes.h"
#include "SLFStatTypes.h"
#include "BFL_StructConversion.generated.h"

/**
 * Blueprint Function Library for converting FInstancedStruct containers to typed structs
 * These replace the BML_StructConversion Blueprint Macro Library
 *
 * Each function takes an FInstancedStruct or FSLFSaveData and extracts the typed struct
 */
UCLASS()
class SLFCONVERSION_API UBFL_StructConversion : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// ═══════════════════════════════════════════════════════════════════════
	// SAVE DATA EXTRACTION
	// ═══════════════════════════════════════════════════════════════════════

	/** Extract FSLFWorldSaveInfo from FSLFSaveData's Data array (index 0) */
	UFUNCTION(BlueprintCallable, Category = "StructConversion|SaveData")
	static bool ConvertToWorldSaveInfoStruct(const FSLFSaveData& Data, FSLFWorldSaveInfo& OutValue);

	/** Extract FSLFClassSaveInfo from FSLFSaveData's Data array (index 0) */
	UFUNCTION(BlueprintCallable, Category = "StructConversion|SaveData")
	static bool ConvertToClassSaveInfoStruct(const FSLFSaveData& Data, FSLFClassSaveInfo& OutValue);

	/** Extract array of FStatInfo from FSLFSaveData's Data array */
	UFUNCTION(BlueprintCallable, Category = "StructConversion|SaveData")
	static bool ConvertToStatInfoStructs(const FSLFSaveData& Data, TArray<FStatInfo>& OutValues);

	/** Extract int32 from FSLFSaveData's Data array (wrapped in FInstancedStruct) */
	UFUNCTION(BlueprintCallable, Category = "StructConversion|SaveData")
	static bool ConvertToIntStruct(const FSLFSaveData& Data, int32& OutValue);

	/** Extract array of FSLFInventoryItemsSaveInfo from FSLFSaveData's Data array */
	UFUNCTION(BlueprintCallable, Category = "StructConversion|SaveData")
	static bool ConvertToInventoryItemsSaveInfoStructs(const FSLFSaveData& Data, TArray<FSLFInventoryItemsSaveInfo>& OutValues);

	/** Extract array of FSLFItemWheelSaveInfo from FSLFSaveData's Data array */
	UFUNCTION(BlueprintCallable, Category = "StructConversion|SaveData")
	static bool ConvertToItemWheelSaveInfoStructs(const FSLFSaveData& Data, TArray<FSLFItemWheelSaveInfo>& OutValues);

	/** Extract array of FSLFEquipmentItemsSaveInfo from FSLFSaveData's Data array */
	UFUNCTION(BlueprintCallable, Category = "StructConversion|SaveData")
	static bool ConvertToEquipmentItemsSaveInfoStructs(const FSLFSaveData& Data, TArray<FSLFEquipmentItemsSaveInfo>& OutValues);

	/** Extract array of FSLFProgressSaveInfo from FSLFSaveData's Data array */
	UFUNCTION(BlueprintCallable, Category = "StructConversion|SaveData")
	static bool ConvertToProgressSaveInfoStructs(const FSLFSaveData& Data, TArray<FSLFProgressSaveInfo>& OutValues);

	/** Extract FTimespan from FSLFSaveData's Data array */
	UFUNCTION(BlueprintCallable, Category = "StructConversion|SaveData")
	static bool ConvertToTimespanStruct(const FSLFSaveData& Data, FTimespan& OutValue);

	/** Extract array of FGuid from FSLFSaveData's Data array */
	UFUNCTION(BlueprintCallable, Category = "StructConversion|SaveData")
	static bool ConvertToGuidStructs(const FSLFSaveData& Data, TArray<FGuid>& OutValues);

	/** Extract array of FSLFInteractableStateSaveInfo from FSLFSaveData's Data array */
	UFUNCTION(BlueprintCallable, Category = "StructConversion|SaveData")
	static bool ConvertToInteractableStateSaveInfoStructs(const FSLFSaveData& Data, TArray<FSLFInteractableStateSaveInfo>& OutValues);

	/** Extract array of FSLFSpawnedActorSaveInfo from FSLFSaveData's Data array */
	UFUNCTION(BlueprintCallable, Category = "StructConversion|SaveData")
	static bool ConvertToSpawnedActorSaveInfoStructs(const FSLFSaveData& Data, TArray<FSLFSpawnedActorSaveInfo>& OutValues);

	/** Extract array of FSLFNpcSaveInfo from FSLFSaveData's Data array */
	UFUNCTION(BlueprintCallable, Category = "StructConversion|SaveData")
	static bool ConvertToNpcSaveInfoStructs(const FSLFSaveData& Data, TArray<FSLFNpcSaveInfo>& OutValues);

	/** Extract array of FSLFNpcVendorSaveInfo from FSLFSaveData's Data array */
	UFUNCTION(BlueprintCallable, Category = "StructConversion|SaveData")
	static bool ConvertToVendorSaveInfoStructs(const FSLFSaveData& Data, TArray<FSLFNpcVendorSaveInfo>& OutValues);

	// ═══════════════════════════════════════════════════════════════════════
	// STATUS EFFECT STRUCTS
	// ═══════════════════════════════════════════════════════════════════════

	/** Extract FStatusEffectTick from FInstancedStruct */
	UFUNCTION(BlueprintCallable, Category = "StructConversion|StatusEffect")
	static bool ConvertToStatusEffectTickStructure(const FInstancedStruct& Data, FStatusEffectTick& OutValue);

	/** Extract FStatusEffectStatChanges from FInstancedStruct */
	UFUNCTION(BlueprintCallable, Category = "StructConversion|StatusEffect")
	static bool ConvertToStatusEffectStatChangesStructure(const FInstancedStruct& Data, FStatusEffectStatChanges& OutValue);

	/** Extract FStatusEffectOneShotAndTick from FInstancedStruct */
	UFUNCTION(BlueprintCallable, Category = "StructConversion|StatusEffect")
	static bool ConvertToStatusEffectOneShotAndTickStructure(const FInstancedStruct& Data, FStatusEffectOneShotAndTick& OutValue);

	// ═══════════════════════════════════════════════════════════════════════
	// PRIMITIVE TYPE EXTRACTION
	// ═══════════════════════════════════════════════════════════════════════

	/** Extract bool from FInstancedStruct */
	UFUNCTION(BlueprintCallable, Category = "StructConversion|Primitives")
	static bool ConvertToFBool(const FInstancedStruct& Data, bool& OutValue);

	/** Extract UClass* from FInstancedStruct (as soft class path) */
	UFUNCTION(BlueprintCallable, Category = "StructConversion|Primitives")
	static bool ConvertToFClass(const FInstancedStruct& Data, UClass*& OutValue);

	/** Extract byte/enum from FInstancedStruct */
	UFUNCTION(BlueprintCallable, Category = "StructConversion|Primitives")
	static bool ConvertToFEnumByte(const FInstancedStruct& Data, uint8& OutValue);

	/** Extract float/double from FInstancedStruct */
	UFUNCTION(BlueprintCallable, Category = "StructConversion|Primitives")
	static bool ConvertToFFloat(const FInstancedStruct& Data, double& OutValue);

	/** Extract int32 from FInstancedStruct */
	UFUNCTION(BlueprintCallable, Category = "StructConversion|Primitives")
	static bool ConvertToFInt(const FInstancedStruct& Data, int32& OutValue);

	/** Extract FName from FInstancedStruct */
	UFUNCTION(BlueprintCallable, Category = "StructConversion|Primitives")
	static bool ConvertToFName(const FInstancedStruct& Data, FName& OutValue);

	/** Extract FRotator from FInstancedStruct */
	UFUNCTION(BlueprintCallable, Category = "StructConversion|Primitives")
	static bool ConvertToFRotator(const FInstancedStruct& Data, FRotator& OutValue);

	/** Extract FString from FInstancedStruct */
	UFUNCTION(BlueprintCallable, Category = "StructConversion|Primitives")
	static bool ConvertToFString(const FInstancedStruct& Data, FString& OutValue);

	/** Extract FVector from FInstancedStruct */
	UFUNCTION(BlueprintCallable, Category = "StructConversion|Primitives")
	static bool ConvertToFVector(const FInstancedStruct& Data, FVector& OutValue);

	/** Extract FSLFProgressWrapper from FInstancedStruct */
	UFUNCTION(BlueprintCallable, Category = "StructConversion|Primitives")
	static bool ConvertToFProgress(const FInstancedStruct& Data, FSLFProgressWrapper& OutValue);

	// ═══════════════════════════════════════════════════════════════════════
	// GENERIC HELPERS
	// ═══════════════════════════════════════════════════════════════════════

	/** Generic template helper to extract a typed struct from FInstancedStruct */
	template<typename T>
	static bool ExtractFromInstancedStruct(const FInstancedStruct& InstancedStruct, T& OutValue)
	{
		if (!InstancedStruct.IsValid())
		{
			return false;
		}

		const T* Ptr = InstancedStruct.GetPtr<T>();
		if (Ptr)
		{
			OutValue = *Ptr;
			return true;
		}

		return false;
	}

	/** Generic template helper to extract an array of typed structs from FSLFSaveData */
	template<typename T>
	static bool ExtractArrayFromSaveData(const FSLFSaveData& Data, TArray<T>& OutValues)
	{
		OutValues.Empty();

		for (const FInstancedStruct& InstancedStruct : Data.Data)
		{
			if (!InstancedStruct.IsValid())
			{
				continue;
			}

			const T* Ptr = InstancedStruct.GetPtr<T>();
			if (Ptr)
			{
				OutValues.Add(*Ptr);
			}
		}

		return OutValues.Num() > 0;
	}
};
