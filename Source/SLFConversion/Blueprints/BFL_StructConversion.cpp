// BFL_StructConversion.cpp
// C++ Blueprint Function Library Implementation for FInstancedStruct conversions
//
// 20-PASS VALIDATION: 2026-01-06 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/BML_StructConversion.json

#include "Blueprints/BFL_StructConversion.h"

// ═══════════════════════════════════════════════════════════════════════════════
// SAVE DATA EXTRACTION
// ═══════════════════════════════════════════════════════════════════════════════

bool UBFL_StructConversion::ConvertToWorldSaveInfoStruct(const FSLFSaveData& Data, FSLFWorldSaveInfo& OutValue)
{
	if (Data.Data.Num() == 0)
	{
		return false;
	}

	return ExtractFromInstancedStruct(Data.Data[0], OutValue);
}

bool UBFL_StructConversion::ConvertToClassSaveInfoStruct(const FSLFSaveData& Data, FSLFClassSaveInfo& OutValue)
{
	if (Data.Data.Num() == 0)
	{
		return false;
	}

	return ExtractFromInstancedStruct(Data.Data[0], OutValue);
}

bool UBFL_StructConversion::ConvertToStatInfoStructs(const FSLFSaveData& Data, TArray<FStatInfo>& OutValues)
{
	return ExtractArrayFromSaveData(Data, OutValues);
}

bool UBFL_StructConversion::ConvertToIntStruct(const FSLFSaveData& Data, int32& OutValue)
{
	if (Data.Data.Num() == 0)
	{
		return false;
	}

	// Use FSLFInt wrapper type from SLFGameTypes.h
	const FSLFInt* WrapperPtr = Data.Data[0].GetPtr<FSLFInt>();
	if (WrapperPtr)
	{
		OutValue = WrapperPtr->Value;
		return true;
	}

	return false;
}

bool UBFL_StructConversion::ConvertToInventoryItemsSaveInfoStructs(const FSLFSaveData& Data, TArray<FSLFInventoryItemsSaveInfo>& OutValues)
{
	return ExtractArrayFromSaveData(Data, OutValues);
}

bool UBFL_StructConversion::ConvertToItemWheelSaveInfoStructs(const FSLFSaveData& Data, TArray<FSLFItemWheelSaveInfo>& OutValues)
{
	return ExtractArrayFromSaveData(Data, OutValues);
}

bool UBFL_StructConversion::ConvertToEquipmentItemsSaveInfoStructs(const FSLFSaveData& Data, TArray<FSLFEquipmentItemsSaveInfo>& OutValues)
{
	return ExtractArrayFromSaveData(Data, OutValues);
}

bool UBFL_StructConversion::ConvertToProgressSaveInfoStructs(const FSLFSaveData& Data, TArray<FSLFProgressSaveInfo>& OutValues)
{
	return ExtractArrayFromSaveData(Data, OutValues);
}

bool UBFL_StructConversion::ConvertToTimespanStruct(const FSLFSaveData& Data, FTimespan& OutValue)
{
	if (Data.Data.Num() == 0)
	{
		return false;
	}

	// Use FSLFTimespan wrapper type from SLFGameTypes.h
	const FSLFTimespan* WrapperPtr = Data.Data[0].GetPtr<FSLFTimespan>();
	if (WrapperPtr)
	{
		OutValue = WrapperPtr->Value;
		return true;
	}

	return false;
}

bool UBFL_StructConversion::ConvertToGuidStructs(const FSLFSaveData& Data, TArray<FGuid>& OutValues)
{
	OutValues.Empty();

	for (const FInstancedStruct& InstancedStruct : Data.Data)
	{
		if (!InstancedStruct.IsValid())
		{
			continue;
		}

		// Use FSLFGuid wrapper type from SLFGameTypes.h
		const FSLFGuid* Ptr = InstancedStruct.GetPtr<FSLFGuid>();
		if (Ptr)
		{
			OutValues.Add(Ptr->Value);
		}
	}

	return OutValues.Num() > 0;
}

bool UBFL_StructConversion::ConvertToInteractableStateSaveInfoStructs(const FSLFSaveData& Data, TArray<FSLFInteractableStateSaveInfo>& OutValues)
{
	return ExtractArrayFromSaveData(Data, OutValues);
}

bool UBFL_StructConversion::ConvertToSpawnedActorSaveInfoStructs(const FSLFSaveData& Data, TArray<FSLFSpawnedActorSaveInfo>& OutValues)
{
	return ExtractArrayFromSaveData(Data, OutValues);
}

bool UBFL_StructConversion::ConvertToNpcSaveInfoStructs(const FSLFSaveData& Data, TArray<FSLFNpcSaveInfo>& OutValues)
{
	return ExtractArrayFromSaveData(Data, OutValues);
}

bool UBFL_StructConversion::ConvertToVendorSaveInfoStructs(const FSLFSaveData& Data, TArray<FSLFNpcVendorSaveInfo>& OutValues)
{
	return ExtractArrayFromSaveData(Data, OutValues);
}

// ═══════════════════════════════════════════════════════════════════════════════
// STATUS EFFECT STRUCTS
// ═══════════════════════════════════════════════════════════════════════════════

bool UBFL_StructConversion::ConvertToStatusEffectTickStructure(const FInstancedStruct& Data, FStatusEffectTick& OutValue)
{
	return ExtractFromInstancedStruct(Data, OutValue);
}

bool UBFL_StructConversion::ConvertToStatusEffectStatChangesStructure(const FInstancedStruct& Data, FStatusEffectStatChanges& OutValue)
{
	return ExtractFromInstancedStruct(Data, OutValue);
}

bool UBFL_StructConversion::ConvertToStatusEffectOneShotAndTickStructure(const FInstancedStruct& Data, FStatusEffectOneShotAndTick& OutValue)
{
	return ExtractFromInstancedStruct(Data, OutValue);
}

// ═══════════════════════════════════════════════════════════════════════════════
// PRIMITIVE TYPE EXTRACTION
// ═══════════════════════════════════════════════════════════════════════════════

bool UBFL_StructConversion::ConvertToFBool(const FInstancedStruct& Data, bool& OutValue)
{
	if (!Data.IsValid())
	{
		return false;
	}

	// Use FSLFBool wrapper type from SLFGameTypes.h
	const FSLFBool* Ptr = Data.GetPtr<FSLFBool>();
	if (Ptr)
	{
		OutValue = Ptr->bValue;
		return true;
	}

	return false;
}

bool UBFL_StructConversion::ConvertToFClass(const FInstancedStruct& Data, UClass*& OutValue)
{
	if (!Data.IsValid())
	{
		OutValue = nullptr;
		return false;
	}

	// Use FSLFClass wrapper type from SLFGameTypes.h
	const FSLFClass* Ptr = Data.GetPtr<FSLFClass>();
	if (Ptr)
	{
		OutValue = Ptr->Value;
		return OutValue != nullptr;
	}

	OutValue = nullptr;
	return false;
}

bool UBFL_StructConversion::ConvertToFEnumByte(const FInstancedStruct& Data, uint8& OutValue)
{
	if (!Data.IsValid())
	{
		return false;
	}

	// Use FSLFEnumByte wrapper type from SLFGameTypes.h
	const FSLFEnumByte* Ptr = Data.GetPtr<FSLFEnumByte>();
	if (Ptr)
	{
		OutValue = Ptr->Value;
		return true;
	}

	return false;
}

bool UBFL_StructConversion::ConvertToFFloat(const FInstancedStruct& Data, double& OutValue)
{
	if (!Data.IsValid())
	{
		return false;
	}

	// Use FSLFFloat wrapper type from SLFGameTypes.h
	const FSLFFloat* Ptr = Data.GetPtr<FSLFFloat>();
	if (Ptr)
	{
		OutValue = Ptr->Value;
		return true;
	}

	return false;
}

bool UBFL_StructConversion::ConvertToFInt(const FInstancedStruct& Data, int32& OutValue)
{
	if (!Data.IsValid())
	{
		return false;
	}

	// Use FSLFInt wrapper type from SLFGameTypes.h
	const FSLFInt* Ptr = Data.GetPtr<FSLFInt>();
	if (Ptr)
	{
		OutValue = Ptr->Value;
		return true;
	}

	return false;
}

bool UBFL_StructConversion::ConvertToFName(const FInstancedStruct& Data, FName& OutValue)
{
	if (!Data.IsValid())
	{
		return false;
	}

	// Use FSLFName wrapper type from SLFGameTypes.h
	const FSLFName* Ptr = Data.GetPtr<FSLFName>();
	if (Ptr)
	{
		OutValue = Ptr->Value;
		return true;
	}

	return false;
}

bool UBFL_StructConversion::ConvertToFRotator(const FInstancedStruct& Data, FRotator& OutValue)
{
	if (!Data.IsValid())
	{
		return false;
	}

	// Use FSLFRotator wrapper type from SLFGameTypes.h
	const FSLFRotator* Ptr = Data.GetPtr<FSLFRotator>();
	if (Ptr)
	{
		OutValue = Ptr->Value;
		return true;
	}

	return false;
}

bool UBFL_StructConversion::ConvertToFString(const FInstancedStruct& Data, FString& OutValue)
{
	if (!Data.IsValid())
	{
		return false;
	}

	// Use FSLFString wrapper type from SLFGameTypes.h
	const FSLFString* Ptr = Data.GetPtr<FSLFString>();
	if (Ptr)
	{
		OutValue = Ptr->Value;
		return true;
	}

	return false;
}

bool UBFL_StructConversion::ConvertToFVector(const FInstancedStruct& Data, FVector& OutValue)
{
	if (!Data.IsValid())
	{
		return false;
	}

	// Use FSLFVector wrapper type from SLFGameTypes.h
	const FSLFVector* Ptr = Data.GetPtr<FSLFVector>();
	if (Ptr)
	{
		OutValue = Ptr->Value;
		return true;
	}

	return false;
}

bool UBFL_StructConversion::ConvertToFProgress(const FInstancedStruct& Data, FSLFProgressWrapper& OutValue)
{
	return ExtractFromInstancedStruct(Data, OutValue);
}
