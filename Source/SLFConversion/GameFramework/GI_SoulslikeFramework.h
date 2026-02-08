// GI_SoulslikeFramework.h
// C++ Game Framework class for GI_SoulslikeFramework
//
// Parent: USLFGameInstance (provides all IBPI_GameInstance implementations)
// All variables, functions, and dispatchers are inherited from USLFGameInstance.
// This class exists as the reparent target for the Blueprint GI_SoulslikeFramework.

#pragma once

#include "CoreMinimal.h"
#include "Framework/SLFGameInstance.h"
#include "GI_SoulslikeFramework.generated.h"

UCLASS()
class SLFCONVERSION_API UGI_SoulslikeFramework : public USLFGameInstance
{
	GENERATED_BODY()

public:
	UGI_SoulslikeFramework();
};
