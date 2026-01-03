// SLFActionDrinkFlaskHP.cpp
#include "SLFActionDrinkFlaskHP.h"

USLFActionDrinkFlaskHP::USLFActionDrinkFlaskHP()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionDrinkFlaskHP] Initialized"));
}

void USLFActionDrinkFlaskHP::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionDrinkFlaskHP] ExecuteAction - Use healing flask"));
	// TODO: Check flask charges, play drink animation, heal HP
}
