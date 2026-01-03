// SLFContainer.h
// C++ base for B_Container - Lootable container (chests, corpses, etc.)
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "SLFContainer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnContainerOpened, AActor*, Opener);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnContainerLooted);

USTRUCT(BlueprintType)
struct FSLFLootEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UObject> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Quantity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DropChance = 1.0f;
};

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFContainer : public AActor
{
	GENERATED_BODY()

public:
	ASLFContainer();

	// ============================================================
	// MIGRATION SUMMARY: B_Container
	// Variables: 8 | Functions: 5 | Dispatchers: 2
	// ============================================================

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ContainerMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* InteractionTrigger;

	// State
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container")
	bool bIsOpen = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container")
	bool bIsLooted = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container")
	bool bRequiresKey = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container")
	FGameplayTag RequiredKeyTag;

	// Loot
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container|Loot")
	TArray<FSLFLootEntry> LootTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container|Loot")
	int32 DeathCurrencyAmount = 0;

	// Animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Container|Animation")
	UAnimMontage* OpenAnimation;

	// Event Dispatchers
	UPROPERTY(BlueprintAssignable, Category = "Container|Events")
	FOnContainerOpened OnContainerOpened;

	UPROPERTY(BlueprintAssignable, Category = "Container|Events")
	FOnContainerLooted OnContainerLooted;

	// Functions
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Container")
	void Interact(AActor* Interactor);
	virtual void Interact_Implementation(AActor* Interactor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Container")
	void OpenContainer(AActor* Opener);
	virtual void OpenContainer_Implementation(AActor* Opener);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Container")
	void LootContainer(AActor* Looter);
	virtual void LootContainer_Implementation(AActor* Looter);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Container")
	TArray<FSLFLootEntry> GenerateLoot();
	virtual TArray<FSLFLootEntry> GenerateLoot_Implementation();

	UFUNCTION(BlueprintPure, Category = "Container")
	bool CanOpen(AActor* Interactor) const;

protected:
	virtual void BeginPlay() override;
};
