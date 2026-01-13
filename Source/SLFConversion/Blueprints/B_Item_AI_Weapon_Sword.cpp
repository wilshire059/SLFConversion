// B_Item_AI_Weapon_Sword.cpp
// C++ implementation for Blueprint B_Item_AI_Weapon_Sword
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Item_AI_Weapon_Sword.json

#include "Blueprints/B_Item_AI_Weapon_Sword.h"

AB_Item_AI_Weapon_Sword::AB_Item_AI_Weapon_Sword()
{
	// Set default rotation offset to fix upside-down weapons on certain skeletons
	// The Demo enemy uses a skeleton with different socket orientations than Guard
	// 180 degree pitch rotation flips the weapon right-side-up
	DefaultAttachmentRotationOffset = FRotator(180.0f, 0.0f, 0.0f);
}

