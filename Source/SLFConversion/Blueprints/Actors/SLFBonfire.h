// SLFBonfire.h
// C++ base for B_Bonfire - Rest point / checkpoint (Dark Souls style bonfire)
#pragma once

#include "CoreMinimal.h"
#include "SLFLocationActor.h"
#include "SLFBonfire.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBonfireLit, AActor*, Player);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBonfireRested, AActor*, Player);

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFBonfire : public ASLFLocationActor
{
	GENERATED_BODY()

public:
	ASLFBonfire();

	// ============================================================
	// MIGRATION SUMMARY: B_Bonfire
	// Variables: 6 | Functions: 5 | Dispatchers: 2
	// ============================================================

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BonfireMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UPointLightComponent* FireLight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UNiagaraComponent* FireEffect;

	// State
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bonfire")
	bool bIsLit = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bonfire")
	bool bRequiresKindling = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bonfire")
	int32 KindlingLevel = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bonfire")
	int32 MaxKindlingLevel = 3;

	// Effects
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bonfire|Effects")
	USoundBase* LightSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bonfire|Effects")
	USoundBase* RestSound;

	// Event Dispatchers
	UPROPERTY(BlueprintAssignable, Category = "Bonfire|Events")
	FOnBonfireLit OnBonfireLit;

	UPROPERTY(BlueprintAssignable, Category = "Bonfire|Events")
	FOnBonfireRested OnBonfireRested;

	// Functions
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Bonfire")
	void LightBonfire(AActor* Player);
	virtual void LightBonfire_Implementation(AActor* Player);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Bonfire")
	void Rest(AActor* Player);
	virtual void Rest_Implementation(AActor* Player);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Bonfire")
	void Kindle(AActor* Player);
	virtual void Kindle_Implementation(AActor* Player);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Bonfire")
	void OpenBonfireMenu(AActor* Player);
	virtual void OpenBonfireMenu_Implementation(AActor* Player);

	UFUNCTION(BlueprintPure, Category = "Bonfire")
	bool CanKindle() const { return KindlingLevel < MaxKindlingLevel; }

protected:
	virtual void BeginPlay() override;
	void UpdateFireVisuals();
};
