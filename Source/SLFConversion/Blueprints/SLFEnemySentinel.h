// SLFEnemySentinel.h
// C++ base class for B_Soulslike_Enemy_Sentinel
// Forensically distinct custom enemy using Ironbound Warlord mesh and ARP-retargeted animations

#pragma once

#include "CoreMinimal.h"
#include "SLFSoulslikeEnemy.h"
#include "SLFEnemySentinel.generated.h"

class UNavigationInvokerComponent;

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFEnemySentinel : public ASLFSoulslikeEnemy
{
	GENERATED_BODY()

public:
	ASLFEnemySentinel();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	void ApplySentinelConfig();
	void ConfigureAbilities();
	void FixMontageDurations();

	UPROPERTY()
	TSubclassOf<UAnimInstance> SentinelAnimBPClass;

	// Navigation invoker - triggers dynamic NavMesh generation around this actor
	UPROPERTY()
	UNavigationInvokerComponent* NavInvoker = nullptr;

	// Montage-based locomotion (bypasses BlendSpacePlayer which collapses bones to meter-scale)
	UPROPERTY()
	UAnimMontage* IdleMontage = nullptr;

	UPROPERTY()
	UAnimMontage* WalkMontage = nullptr;

	UPROPERTY()
	UAnimMontage* RunMontage = nullptr;

	// Track active locomotion montage for seamless loop
	UAnimMontage* ActiveLocomotionMontage = nullptr;

	bool bAnimBPApplied = false;
	bool bMontageLocomotionActive = false;
	bool bMontageDurationsFixed = false;
	int32 TicksAfterBeginPlay = 0;
};
