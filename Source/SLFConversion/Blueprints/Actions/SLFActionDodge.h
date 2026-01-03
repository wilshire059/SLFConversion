// SLFActionDodge.h
// C++ base for B_Action_Dodge - Dodge/roll action
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Action_Dodge
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         1/1 (DodgeMontages struct)
// Functions:         1/1 (ExecuteAction override)
// Event Dispatchers: 0/0 (inherits from ActionBase)
// CDO Defaults:      Dodge action with directional montages
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFActionBase.h"
#include "SLFGameTypes.h"
#include "SLFActionDodge.generated.h"

// Types used from SLFGameTypes.h:
// - FSLFDodgeMontages

/**
 * USLFActionDodge - Dodge/roll action
 * Plays directional dodge animation based on input
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFActionDodge : public USLFActionBase
{
	GENERATED_BODY()

public:
	USLFActionDodge();

	// ═══════════════════════════════════════════════════════════════════════════
	// VARIABLES [1/1]
	// ═══════════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action|Dodge")
	FSLFDodgeMontages DodgeMontages;

	// ═══════════════════════════════════════════════════════════════════════════
	// FUNCTIONS [1/1]
	// ═══════════════════════════════════════════════════════════════════════════

	virtual void ExecuteAction_Implementation() override;

protected:
	/** Get the appropriate dodge montage based on movement direction */
	UFUNCTION(BlueprintCallable, Category = "Action|Dodge")
	UAnimMontage* GetDirectionalDodgeMontage();
};
