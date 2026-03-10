// SLFCraftingManager.cpp
#include "SLFCraftingManager.h"

USLFCraftingManager::USLFCraftingManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USLFCraftingManager::BeginPlay()
{
	Super::BeginPlay();
}

void USLFCraftingManager::DiscoverRecipe(int32 RecipeIndex)
{
	if (!Recipes.IsValidIndex(RecipeIndex)) return;

	if (!Recipes[RecipeIndex].bDiscovered)
	{
		Recipes[RecipeIndex].bDiscovered = true;
		UE_LOG(LogTemp, Log, TEXT("[Crafting] Recipe discovered: %s"),
			*Recipes[RecipeIndex].RecipeName.ToString());
		OnRecipeDiscovered.Broadcast(Recipes[RecipeIndex]);
	}
}

TArray<FSLFCraftingRecipe> USLFCraftingManager::GetDiscoveredRecipes() const
{
	TArray<FSLFCraftingRecipe> Result;
	for (const FSLFCraftingRecipe& Recipe : Recipes)
	{
		if (Recipe.bDiscovered)
		{
			Result.Add(Recipe);
		}
	}
	return Result;
}

bool USLFCraftingManager::CanCraft(int32 RecipeIndex) const
{
	if (!Recipes.IsValidIndex(RecipeIndex)) return false;

	const FSLFCraftingRecipe& Recipe = Recipes[RecipeIndex];
	if (!Recipe.bDiscovered) return false;

	// TODO: Check player inventory for required materials
	// This needs integration with InventoryManagerComponent
	// For now, return true if recipe is discovered
	UE_LOG(LogTemp, Log, TEXT("[Crafting] CanCraft check for: %s — inventory check pending"),
		*Recipe.RecipeName.ToString());
	return true;
}

bool USLFCraftingManager::CraftItem(int32 RecipeIndex)
{
	if (!CanCraft(RecipeIndex)) return false;

	const FSLFCraftingRecipe& Recipe = Recipes[RecipeIndex];

	// TODO: Remove input materials from inventory
	// TODO: Add output item to inventory

	UE_LOG(LogTemp, Warning, TEXT("[Crafting] Crafted: %s x%d"),
		*Recipe.RecipeName.ToString(), Recipe.OutputCount);

	OnItemCrafted.Broadcast(Recipe.OutputItem);
	return true;
}
