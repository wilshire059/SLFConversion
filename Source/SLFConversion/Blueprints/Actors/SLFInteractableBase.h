// SLFInteractableBase.h
// C++ base for B_InteractableBase - Generic interactable object
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLFInteractableBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteracted, AActor*, Interactor);

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFInteractableBase : public AActor
{
	GENERATED_BODY()

public:
	ASLFInteractableBase();

	// ============================================================
	// MIGRATION SUMMARY: B_InteractableBase
	// Variables: 6 | Functions: 4 | Dispatchers: 1
	// ============================================================

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* InteractableMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* InteractionTrigger;

	// Interaction Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText InteractionPrompt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText InteractionText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText InteractableDisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bCanInteract = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bRequiresLookAt = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionDistance = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bOneTimeUse = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bHasBeenUsed = false;

	// Event Dispatcher
	UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
	FOnInteracted OnInteracted;

	// Functions
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact(AActor* Interactor);
	virtual void Interact_Implementation(AActor* Interactor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	bool CanBeInteractedWith(AActor* Interactor);
	virtual bool CanBeInteractedWith_Implementation(AActor* Interactor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void EnableInteraction();
	virtual void EnableInteraction_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void DisableInteraction();
	virtual void DisableInteraction_Implementation();

	/** Called when interaction happens - override in child classes */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnInteract(AActor* Interactor);
	virtual void OnInteract_Implementation(AActor* Interactor);

	/** Setup interactable - called from construction or BeginPlay */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void SetupInteractable();
	virtual void SetupInteractable_Implementation();

protected:
	virtual void BeginPlay() override;
};
