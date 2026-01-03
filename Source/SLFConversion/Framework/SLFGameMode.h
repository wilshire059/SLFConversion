// SLFGameMode.h
// C++ base for GM_SoulslikeFramework - Game Mode
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - GM_SoulslikeFramework
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0
// Functions:         0/0 (uses BeginPlay)
// Event Dispatchers: 0/0
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SLFGameMode.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASLFGameMode();

	virtual void BeginPlay() override;
};
