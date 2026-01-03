// SLFDoorBase.h
// C++ base for B_Door - Interactive door actor
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "GameplayTagContainer.h"
#include "SLFDoorBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDoorStateChanged, bool, bIsOpen);

UENUM(BlueprintType)
enum class ESLFDoorState : uint8
{
	Closed,
	Opening,
	Open,
	Closing
};

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFDoorBase : public AActor
{
	GENERATED_BODY()

public:
	/** Default scene root for Blueprint components */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> DefaultSceneRoot;

public:
	ASLFDoorBase();

	// ============================================================
	// MIGRATION SUMMARY: B_Door
	// Variables: 8 | Functions: 6 | Dispatchers: 1
	// ============================================================

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* DoorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* InteractionTrigger;

	// State
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	ESLFDoorState DoorState = ESLFDoorState::Closed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	bool bIsLocked = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	FGameplayTag RequiredKeyTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float OpenAngle = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float OpenSpeed = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	bool bAutoClose = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float AutoCloseDelay = 3.0f;

	// Event Dispatcher
	UPROPERTY(BlueprintAssignable, Category = "Door|Events")
	FOnDoorStateChanged OnDoorStateChanged;

	// Functions
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Door")
	void Interact(AActor* Interactor);
	virtual void Interact_Implementation(AActor* Interactor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Door")
	void OpenDoor();
	virtual void OpenDoor_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Door")
	void CloseDoor();
	virtual void CloseDoor_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Door")
	void ToggleDoor();
	virtual void ToggleDoor_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Door")
	bool TryUnlock(AActor* Interactor);
	virtual bool TryUnlock_Implementation(AActor* Interactor);

	UFUNCTION(BlueprintPure, Category = "Door")
	bool IsOpen() const { return DoorState == ESLFDoorState::Open; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	FRotator InitialRotation;
	FRotator TargetRotation;
	FTimerHandle AutoCloseTimerHandle;

	void HandleAutoClose();
};
