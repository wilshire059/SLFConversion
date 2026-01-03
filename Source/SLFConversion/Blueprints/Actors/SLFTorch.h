// SLFTorch.h
// C++ base for B_Torch - Interactive light source
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLFTorch.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTorchStateChanged, bool, bIsLit);

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFTorch : public AActor
{
	GENERATED_BODY()

public:
	ASLFTorch();

	// ============================================================
	// MIGRATION SUMMARY: B_Torch
	// Variables: 7 | Functions: 4 | Dispatchers: 1
	// ============================================================

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* TorchMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UPointLightComponent* TorchLight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UNiagaraComponent* FlameEffect;

	// State
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Torch")
	bool bIsLit = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Torch")
	bool bCanBeToggled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Torch")
	bool bStartLit = true;

	// Light Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Torch|Light")
	float LightIntensity = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Torch|Light")
	float LightRadius = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Torch|Light")
	FLinearColor LightColor = FLinearColor(1.0f, 0.7f, 0.3f);

	// Event Dispatcher
	UPROPERTY(BlueprintAssignable, Category = "Torch|Events")
	FOnTorchStateChanged OnTorchStateChanged;

	// Functions
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Torch")
	void LightTorch();
	virtual void LightTorch_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Torch")
	void ExtinguishTorch();
	virtual void ExtinguishTorch_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Torch")
	void ToggleTorch();
	virtual void ToggleTorch_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Torch")
	void Interact(AActor* Interactor);
	virtual void Interact_Implementation(AActor* Interactor);

protected:
	virtual void BeginPlay() override;
	void UpdateTorchVisuals();
};
