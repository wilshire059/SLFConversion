// SLFDiscovery.h
// C++ base for B_Discovery - Area discovery trigger (bonfires, new area notifications)
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "SLFDiscovery.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAreaDiscovered, FText, AreaName, AActor*, Discoverer);

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFDiscovery : public AActor
{
	GENERATED_BODY()

public:
	ASLFDiscovery();

	// ============================================================
	// MIGRATION SUMMARY: B_Discovery
	// Variables: 6 | Functions: 3 | Dispatchers: 1
	// ============================================================

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* DiscoveryTrigger;

	// Discovery Info
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Discovery")
	FText AreaName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Discovery")
	FText AreaDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Discovery")
	FGameplayTag DiscoveryTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Discovery")
	bool bAlreadyDiscovered = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Discovery")
	bool bShowNotification = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Discovery")
	USoundBase* DiscoverySound;

	// Event Dispatcher
	UPROPERTY(BlueprintAssignable, Category = "Discovery|Events")
	FOnAreaDiscovered OnAreaDiscovered;

	// Functions
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Discovery")
	void OnPlayerEnter(AActor* Player);
	virtual void OnPlayerEnter_Implementation(AActor* Player);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Discovery")
	void TriggerDiscovery(AActor* Discoverer);
	virtual void TriggerDiscovery_Implementation(AActor* Discoverer);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Discovery")
	void ShowDiscoveryUI();
	virtual void ShowDiscoveryUI_Implementation();

protected:
	virtual void BeginPlay() override;
};
