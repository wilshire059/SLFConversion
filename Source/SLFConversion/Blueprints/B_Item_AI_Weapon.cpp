// B_Item_AI_Weapon.cpp
// C++ implementation for Blueprint B_Item_AI_Weapon
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Item_AI_Weapon.json

#include "Blueprints/B_Item_AI_Weapon.h"

AB_Item_AI_Weapon::AB_Item_AI_Weapon()
{
	// Default rotation offset is zero - child classes can override
	DefaultAttachmentRotationOffset = FRotator::ZeroRotator;
}

void AB_Item_AI_Weapon::BeginPlay()
{
	Super::BeginPlay();

	// Apply rotation offset if specified (handles different skeleton socket orientations)
	// AI weapons are spawned as ChildActors - their relative rotation needs adjustment
	if (!DefaultAttachmentRotationOffset.IsZero())
	{
		SetActorRelativeRotation(DefaultAttachmentRotationOffset);
		UE_LOG(LogTemp, Log, TEXT("[AI Weapon] %s: Applied rotation offset: %s"),
			*GetName(), *DefaultAttachmentRotationOffset.ToString());
	}
}

