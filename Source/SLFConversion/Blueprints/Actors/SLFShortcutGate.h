// SLFShortcutGate.h
// One-way shortcut gate — interactable from one side only (like Elden Ring elevator shortcuts)

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Blueprints/Actors/SLFInteractableBase.h"
#include "SLFShortcutGate.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFShortcutGate : public ASLFInteractableBase
{
	GENERATED_BODY()

public:
	ASLFShortcutGate();

	/** Whether the gate has been opened (persists) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shortcut Gate")
	bool bIsOpened = false;

	/** Direction the gate can be opened from (forward vector of actor) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shortcut Gate")
	bool bCanOpenFromFront = true;

	/** The gate mesh (door/lever/chain) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* GateMesh;

	/** Blocking collision — disabled when opened */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* BlockingVolume;

	/** Open the gate (called from interaction) */
	UFUNCTION(BlueprintCallable, Category = "Shortcut Gate")
	void OpenGate();

	/** Check if player is on the correct side to open */
	UFUNCTION(BlueprintCallable, Category = "Shortcut Gate")
	bool IsOnOpenSide(AActor* Actor) const;

	virtual void OnInteract_Implementation(AActor* InteractingActor) override;

protected:
	virtual void BeginPlay() override;
};
