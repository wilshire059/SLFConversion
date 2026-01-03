// SLFGameInstance.h
// C++ base for GI_SoulslikeFramework - Game Instance
//
// IMPLEMENTATION SUMMARY - GI_SoulslikeFramework
// Variables:         7/7 (settings, save slots, active slot)
// Functions:         0/0 (uses Init)
// Event Dispatchers: 1/1 (OnSelectedClassChanged)

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameFramework/SaveGame.h"
#include "SLFGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSelectedClassChanged);

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	USLFGameInstance();

	virtual void Init() override;

	// VARIABLES [7/7]

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UDataAsset* CustomGameSettings;

	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	UDataAsset* SelectedBaseClass;

	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	USaveGame* SGO_Slots;

	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	FString SlotsSaveName = TEXT("SaveSlots");

	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	FString ActiveSlot;

	// Note: Blueprint name was "FirstTimeOnDemoLevel?" - C++ cannot use ? in identifiers
	// Callers expecting the ? suffix will need to be migrated
	UPROPERTY(BlueprintReadWrite, Category = "Demo Level")
	bool FirstTimeOnDemoLevel = true;

	// EVENT DISPATCHERS [1/1]

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSelectedClassChanged OnSelectedClassChanged;
};
