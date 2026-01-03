// SLFEnemyShowcase.h
// C++ base class for B_Soulslike_Enemy_Showcase
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - B_Soulslike_Enemy_Showcase
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 migrated
// Functions:         0/0 migrated
// Event Dispatchers: 0/0
// Components:        1/1 (Weapon - values in Blueprint)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Showcase
// Parent: B_Soulslike_Enemy_C -> ASLFSoulslikeEnemy
//
// PURPOSE: Showcase enemy - simple enemy for tutorial/showcase areas

#pragma once

#include "CoreMinimal.h"
#include "SLFSoulslikeEnemy.h"
#include "SLFEnemyShowcase.generated.h"

// Forward declarations
class UChildActorComponent;

/**
 * Showcase Enemy - Simple enemy for showcase/tutorial areas
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFEnemyShowcase : public ASLFSoulslikeEnemy
{
	GENERATED_BODY()

public:
	ASLFEnemyShowcase();

protected:
	virtual void BeginPlay() override;

	// ═══════════════════════════════════════════════════════════════════
	// COMPONENTS: 1/1 (Weapon - stays in Blueprint to preserve hierarchy)
	// Access via FindComponentByClass<UChildActorComponent>() if needed
	// ═══════════════════════════════════════════════════════════════════
};
