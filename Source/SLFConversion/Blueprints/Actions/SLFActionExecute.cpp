// SLFActionExecute.cpp
#include "SLFActionExecute.h"

USLFActionExecute::USLFActionExecute()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionExecute] Initialized"));
}

void USLFActionExecute::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionExecute] ExecuteAction - Riposte/execution attack"));
	// TODO: Play paired execution animation on staggered enemy
}
