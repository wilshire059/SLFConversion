// SLFItemBase.cpp
// C++ implementation for B_Item
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Item
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         1/1 (initialized in constructor)
// Functions:         1/1 implemented
// Components:        0/0 (DefaultSceneRoot comes from Blueprint SCS)
// Event Dispatchers: 0/0
// ═══════════════════════════════════════════════════════════════════════════════

#include "SLFItemBase.h"
#include "Components/SceneComponent.h"

ASLFItemBase::ASLFItemBase()
{
	PrimaryActorTick.bCanEverTick = false;
	// NOTE: DefaultSceneRoot is provided by B_Item's Blueprint SCS
	// Not creating it here to avoid name collision
}

void ASLFItemBase::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[Item] BeginPlay: %s"), *GetName());
}

void ASLFItemBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	SetupItem();
}

// ═══════════════════════════════════════════════════════════════════════════════
// FUNCTIONS [1/1]
// ═══════════════════════════════════════════════════════════════════════════════

void ASLFItemBase::SetupItem_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[Item] SetupItem: %s - %s"),
		*ItemInfo.ItemTag.ToString(), *ItemInfo.DisplayName.ToString());
}
