// PC_SoulslikeFramework.h
// C++ Game Framework class for PC_SoulslikeFramework
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/GameFramework/PC_SoulslikeFramework.json
// Parent: APlayerController
// Variables: 4 | Functions: 3 | Dispatchers: 2

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "InputMappingContext.h"
#include "Interfaces/BPI_Controller.h"
#include "PC_SoulslikeFramework.generated.h"

// Forward declarations
class UW_HUD;
class ULevelSequencePlayer;

// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPC_SoulslikeFramework_OnInputDetected, bool, IsGamepad);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPC_SoulslikeFramework_OnHudInitialized);

UCLASS()
class SLFCONVERSION_API APC_SoulslikeFramework : public APlayerController, public IBPI_Controller
{
	GENERATED_BODY()

public:
	APC_SoulslikeFramework();

	// VARIABLES (4)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime")
	UW_HUD* W_HUD;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UInputMappingContext* GameplayMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime")
	ULevelSequencePlayer* ActiveSequencePlayer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime|Navigation")
	FGameplayTag ActiveWidgetTag;

	// EVENT DISPATCHERS (2)

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FPC_SoulslikeFramework_OnInputDetected OnInputDetected;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FPC_SoulslikeFramework_OnHudInitialized OnHudInitialized;

	// FUNCTIONS (3)

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PC_SoulslikeFramework")
	void HandleMainMenuRequest();
	virtual void HandleMainMenuRequest_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PC_SoulslikeFramework")
	void GetNearestRestingPoint(bool& OutSuccess, AActor*& OutPoint, bool& OutSuccess2, AActor*& OutPoint3);
	virtual void GetNearestRestingPoint_Implementation(bool& OutSuccess, AActor*& OutPoint, bool& OutSuccess2, AActor*& OutPoint3);

	// INTERFACE IMPLEMENTATIONS (IBPI_Controller)

	virtual void GetPlayerHUD_Implementation(UUserWidget*& HUD) override;
};
