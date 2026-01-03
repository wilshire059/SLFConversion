// SLFProjectileThrowingKnife.cpp
#include "SLFProjectileThrowingKnife.h"

ASLFProjectileThrowingKnife::ASLFProjectileThrowingKnife()
{
	ProjectileSpeed = 3000.0f;
	bIsHoming = false;
	UE_LOG(LogTemp, Log, TEXT("[ProjectileThrowingKnife] Initialized - Speed: %.0f"), ProjectileSpeed);
}
