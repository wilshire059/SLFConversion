// GI_SoulslikeFramework.h
// C++ Game Framework class for GI_SoulslikeFramework
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/GameFramework/GI_SoulslikeFramework.json
// Parent: UGameInstance
// Variables: 6 | Functions: 0 | Dispatchers: 1

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameplayTagContainer.h"
#include "Interfaces/SLFGameInstanceInterface.h"
#include "GI_SoulslikeFramework.generated.h"

// Forward declarations
class USG_SaveSlots;

// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGI_SoulslikeFramework_OnSelectedClassChanged);

UCLASS()
class SLFCONVERSION_API UGI_SoulslikeFramework : public UGameInstance, public ISLFGameInstanceInterface
{
	GENERATED_BODY()

public:
	UGI_SoulslikeFramework();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (6)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UPrimaryDataAsset* CustomGameSettings;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime")
	UPrimaryDataAsset* SelectedBaseClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime")
	USG_SaveSlots* SGO_Slots;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime")
	FString SlotsSaveName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime")
	FString ActiveSlot;
	// Note: DisplayName must match the original Blueprint variable name for Level BP compatibility
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demo Level", meta = (DisplayName = "First Time on Demo Level?"))
	bool FirstTimeOnDemoLevel;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FGI_SoulslikeFramework_OnSelectedClassChanged OnSelectedClassChanged;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (0)
	// ═══════════════════════════════════════════════════════════════════════

};
