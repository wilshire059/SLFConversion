// SLFGameInstance.cpp
#include "SLFGameInstance.h"
#include "Kismet/GameplayStatics.h"

USLFGameInstance::USLFGameInstance()
{
	CustomGameSettings = nullptr;
	SelectedBaseClass = nullptr;
	SGO_Slots = nullptr;
}

void USLFGameInstance::Init()
{
	Super::Init();

	UE_LOG(LogTemp, Log, TEXT("[SLFGameInstance] Init"));

	// Load save slots
	if (UGameplayStatics::DoesSaveGameExist(SlotsSaveName, 0))
	{
		SGO_Slots = UGameplayStatics::LoadGameFromSlot(SlotsSaveName, 0);
		UE_LOG(LogTemp, Log, TEXT("[SLFGameInstance] Loaded save slots"));
	}
}
