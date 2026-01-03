// SLFActionThrowProjectile.cpp
#include "SLFActionThrowProjectile.h"

USLFActionThrowProjectile::USLFActionThrowProjectile()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionThrowProjectile] Initialized"));
}

void USLFActionThrowProjectile::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionThrowProjectile] ExecuteAction - Throw consumable projectile"));
	// TODO: Get throwing item from inventory, spawn projectile actor
}
