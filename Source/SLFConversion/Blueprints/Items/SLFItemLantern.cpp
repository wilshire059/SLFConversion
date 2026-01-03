// SLFItemLantern.cpp
#include "SLFItemLantern.h"
#include "Components/PointLightComponent.h"

ASLFItemLantern::ASLFItemLantern()
{
	// Component created in Blueprint - just initialize state
	bIsLit = false;
}

void ASLFItemLantern::ToggleLight()
{
	bIsLit = !bIsLit;
	if (LanternLight)
	{
		LanternLight->SetVisibility(bIsLit);
		UE_LOG(LogTemp, Log, TEXT("[ItemLantern] Light toggled: %s"), bIsLit ? TEXT("ON") : TEXT("OFF"));
	}
}
