// SLFEnemyGuard.h
// C++ base class for B_Soulslike_Enemy_Guard

#pragma once

#include "CoreMinimal.h"
#include "SLFSoulslikeEnemy.h"
#include "SLFEnemyGuard.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFEnemyGuard : public ASLFSoulslikeEnemy
{
	GENERATED_BODY()

public:
	ASLFEnemyGuard();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	void ApplyC3100Config();
	void ConfigureAbilities();
	void ApplyMontageLocomotionFallback();
	void FixMontageDurations();

	UPROPERTY()
	TSubclassOf<UAnimInstance> C3100AnimBPClass;

	// Montage-based locomotion (bypasses BlendSpacePlayer which collapses bones to meter-scale)
	UPROPERTY()
	UAnimMontage* IdleMontage = nullptr;

	UPROPERTY()
	UAnimMontage* WalkMontage = nullptr;

	UPROPERTY()
	UAnimMontage* RunMontage = nullptr;

	// Track which locomotion montage is currently playing
	UAnimMontage* ActiveLocomotionMontage = nullptr;

	bool bAnimBPApplied = false;
	bool bMontageLocomotionActive = false;
	bool bMontageDurationsFixed = false;
	bool bDiagDone = false;
	int32 TicksAfterBeginPlay = 0;
	int32 MoveDiagCount = 0;
};
