// B_Soulslike_Boss.h
// C++ class for Blueprint B_Soulslike_Boss
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Soulslike_Boss.json
// Parent: B_Soulslike_Enemy_C -> AB_Soulslike_Enemy

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/B_Soulslike_Enemy.h"
#include "Components/SphereComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "B_Soulslike_Boss.generated.h"

// Forward declarations
class UAIBossComponent;

/**
 * Boss Character - Base class for boss enemies
 * Has trigger collision for fight activation and boss component for phase management
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API AB_Soulslike_Boss : public AB_Soulslike_Enemy
{
	GENERATED_BODY()

public:
	AB_Soulslike_Boss();

	// ═══════════════════════════════════════════════════════════════════════
	// COMPONENTS
	// ═══════════════════════════════════════════════════════════════════════

	// Boss AI component for phase management, boss doors, etc.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAIBossComponent* AC_AI_Boss;

	// Trigger collision for entering boss arena
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* TriggerCollision;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════

protected:
	virtual void BeginPlay() override;

	// Called when player enters trigger collision
	UFUNCTION()
	void OnTriggerCollisionBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	// Called when AI perception senses actors
	UFUNCTION()
	void OnControllerPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

private:
	// Try to start the boss fight with a target
	void TryStartFight(AActor* Target);
};
