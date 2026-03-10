// SLFCraftingManager.h
// Crafting system component — manages recipes, discovery, and crafting at resting points

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SLFGameTypes.h"
#include "SLFCraftingManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRecipeDiscovered, const FSLFCraftingRecipe&, Recipe);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemCrafted, TSubclassOf<UObject>, ItemClass);

UCLASS(Blueprintable, BlueprintType, meta = (BlueprintSpawnableComponent))
class SLFCONVERSION_API USLFCraftingManager : public UActorComponent
{
	GENERATED_BODY()

public:
	USLFCraftingManager();

	/** All known recipes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	TArray<FSLFCraftingRecipe> Recipes;

	/** Discover a recipe by index */
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	void DiscoverRecipe(int32 RecipeIndex);

	/** Get all discovered recipes */
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	TArray<FSLFCraftingRecipe> GetDiscoveredRecipes() const;

	/** Check if player has materials for a recipe */
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	bool CanCraft(int32 RecipeIndex) const;

	/** Craft an item from a recipe (consumes materials, produces output) */
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	bool CraftItem(int32 RecipeIndex);

	UPROPERTY(BlueprintAssignable, Category = "Crafting|Events")
	FOnRecipeDiscovered OnRecipeDiscovered;

	UPROPERTY(BlueprintAssignable, Category = "Crafting|Events")
	FOnItemCrafted OnItemCrafted;

protected:
	virtual void BeginPlay() override;
};
