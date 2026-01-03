// PC_SoulslikeFramework.cpp
// C++ implementation for Game Framework class PC_SoulslikeFramework
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session

#include "GameFramework/PC_SoulslikeFramework.h"
#include "Widgets/W_HUD.h"

APC_SoulslikeFramework::APC_SoulslikeFramework()
{
	W_HUD = nullptr;
}

void APC_SoulslikeFramework::HandleMainMenuRequest_Implementation()
{
	// TODO: Implement from Blueprint
}

void APC_SoulslikeFramework::GetNearestRestingPoint_Implementation(bool& OutSuccess, AActor*& OutPoint, bool& OutSuccess2, AActor*& OutPoint3)
{
	// TODO: Implement from Blueprint
	OutSuccess = false;
	OutPoint = nullptr;
	OutSuccess2 = false;
	OutPoint3 = nullptr;
}

void APC_SoulslikeFramework::GetPlayerHUD_Implementation(UUserWidget*& HUD)
{
	HUD = W_HUD;
}
