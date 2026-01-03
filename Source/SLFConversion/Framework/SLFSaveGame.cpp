// SLFSaveGame.cpp
#include "SLFSaveGame.h"

USLFSaveGame::USLFSaveGame()
{
	SaveSlotName = TEXT("SaveSlot1");
	UserIndex = 0;
	CurrentLevelName = NAME_None;

	UE_LOG(LogTemp, Log, TEXT("[SLFSaveGame] Created"));
}
