// PC_Menu_SoulslikeFramework.h
// C++ Game Framework class for PC_Menu_SoulslikeFramework
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/GameFramework/PC_Menu_SoulslikeFramework.json
// Parent: APlayerController
// Variables: 3 | Functions: 1 | Dispatchers: 0

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "Interfaces/SLFControllerInterface.h"
#include "PC_Menu_SoulslikeFramework.generated.h"

// Forward declarations
class UW_MainMenu;

// Event Dispatchers


UCLASS()
class SLFCONVERSION_API APC_Menu_SoulslikeFramework : public APlayerController, public ISLFControllerInterface
{
	GENERATED_BODY()

public:
	APC_Menu_SoulslikeFramework();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (3)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	UW_MainMenu* W_MainMenu;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	bool SaveGameExists;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FGameplayTag NavigableWidgetTag;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

};
