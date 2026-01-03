// SLFSaveSlots.cpp
#include "SLFSaveSlots.h"

USLFSaveSlots::USLFSaveSlots()
{
	MaxSlots = 10;
	UE_LOG(LogTemp, Log, TEXT("[SLFSaveSlots] Created with max %d slots"), MaxSlots);
}
