// BPI_NPC.h
// C++ Interface for BPI_NPC
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA_v2/Interface/BPI_NPC.json
// Functions: 0 (Type identification interface)

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BPI_NPC.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UBPI_NPC : public UInterface
{
	GENERATED_BODY()
};

/**
 * NPC Interface
 * Empty interface used for NPC type identification
 */
class SLFCONVERSION_API IBPI_NPC
{
	GENERATED_BODY()

public:
	// No functions - used for type identification only
};
