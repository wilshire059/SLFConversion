// SLFSoulslikeBoss.h
// C++ base class for B_Soulslike_Boss
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - B_Soulslike_Boss
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         0/0 migrated (inherits from Enemy)
// Functions:         0/0 migrated (inherits from Enemy)
// Event Dispatchers: 0/0 migrated (inherits from Enemy)
// Interfaces:        None (inherits BPI_Enemy, BPI_Executable from Enemy)
// Components:        2 (AC_AI_Boss, TriggerCollision)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Boss
//
// PURPOSE: Base boss character - inherits enemy functionality, works with AC_AI_Boss component
// PARENT: B_Soulslike_Enemy
//
// NOTE: This class has no additional variables or functions in the original Blueprint.
//       Boss-specific behavior is handled by the UAIBossComponent attached to this actor.

#pragma once

#include "CoreMinimal.h"
#include "SLFSoulslikeEnemy.h"
#include "Components/SphereComponent.h"
#include "SLFSoulslikeBoss.generated.h"

// Forward declarations
class UAIBossComponent;
class UAC_AI_CombatManager;

/**
 * Soulslike boss character - enemy with boss encounter functionality
 * Works in conjunction with UAIBossComponent for phase management
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFSoulslikeBoss : public ASLFSoulslikeEnemy
{
	GENERATED_BODY()

public:
	ASLFSoulslikeBoss();

protected:
	virtual void BeginPlay() override;

public:
	// ═══════════════════════════════════════════════════════════════════
	// COMPONENTS (from JSON)
	// ═══════════════════════════════════════════════════════════════════

	/** AI Boss component for phase management - named BossComponent to avoid collision with Blueprint SCS "AC_AI_Boss" */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UAIBossComponent> BossComponent;

	/** Trigger collision sphere for boss encounter activation - named TriggerCollisionSphere to avoid collision with Blueprint SCS "TriggerCollision" */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USphereComponent> TriggerCollisionSphere;

	/** AC_AI_CombatManager component - required for AnimBP property bindings that expect this type */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UAC_AI_CombatManager> AC_AI_CombatManagerComponent;

	// ═══════════════════════════════════════════════════════════════════
	// HELPER FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════

	/** Helper to get the boss component */
	UFUNCTION(BlueprintPure, Category = "Boss")
	UAIBossComponent* GetBossComponent() const { return BossComponent; }

protected:
	/** Called when something enters trigger collision */
	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
