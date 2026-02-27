// W_HUD.cpp
// C++ Widget implementation for W_HUD
//
// 20-PASS VALIDATION: 2026-01-07
// Base implementation with child widget visibility management

#include "Widgets/W_HUD.h"
#include "Blueprints/B_Stat.h"
#include "Blueprints/SLFStatBase.h"
#include "Blueprints/B_StatusEffect.h"
#include "Components/StatManagerComponent.h"
#include "Blueprints/B_Interactable.h"
#include "Blueprints/B_RestingPoint.h"
#include "Blueprints/SLFRestingPointBase.h"
#include "Blueprints/Actors/SLFInteractableBase.h"
#include "Blueprints/Actors/SLFBossDoor.h"
#include "Blueprints/SLFPickupItemBase.h"
#include "SLFPrimaryDataAssets.h"
#include "Interfaces/SLFInteractableInterface.h"
#include "Interfaces/BPI_Interactable.h"
#include "Interfaces/BPI_Controller.h"
#include "Components/AIInteractionManagerComponent.h"
#include "Widgets/W_LoadingScreen.h"
#include "Widgets/W_Inventory.h"
#include "Widgets/W_Equipment.h"
#include "Widgets/W_Crafting.h"
#include "Widgets/W_GameMenu.h"
#include "Widgets/W_Status.h"
#include "Widgets/W_Settings.h"
#include "Widgets/W_RestMenu.h"
#include "Widgets/W_NPC_Window.h"
#include "Widgets/W_Dialog.h"
#include "Widgets/W_Interaction.h"
#include "Widgets/W_LootNotification.h"
#include "Widgets/W_FirstLootNotification.h"
#include "Widgets/W_Resources.h"
#include "Widgets/W_BigScreenMessage.h"
#include "Widgets/W_StatusEffectBar.h"
#include "Widgets/W_StatusEffectNotification.h"
#include "Widgets/W_Radar.h"
#include "Widgets/W_Boss_Healthbar.h"
#include "Widgets/W_ItemWheelSlot.h"
#include "Widgets/W_WorldMap.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/InventoryManagerComponent.h"
#include "Components/AC_StatusEffectManager.h"
#include "Components/AC_EquipmentManager.h"
#include "Components/EquipmentManagerComponent.h"
#include "Components/StatusEffectManagerComponent.h"
#include "GameFramework/PC_SoulslikeFramework.h"
#include "Framework/SLFPlayerController.h"

UW_HUD::UW_HUD(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	LoadingScreen = nullptr;
	CachedW_Interaction = nullptr;
	CachedW_GameMenu = nullptr;
	CachedW_Inventory = nullptr;
	CachedW_Equipment = nullptr;
	CachedW_Crafting = nullptr;
	CachedW_Status = nullptr;
	CachedW_Settings = nullptr;
	CachedW_Resources = nullptr;
	CachedW_RestMenu = nullptr;
	CachedW_Radar = nullptr;
	CachedW_NPC_Window = nullptr;
	CachedItemLootNotificationsBox = nullptr;
	CachedW_FirstLootNotification = nullptr;
	CachedStatusEffectBox = nullptr;
	CachedW_StatusEffectNotification = nullptr;
	CachedBossHealthbar = nullptr;
	LootNotificationWidgetClass = nullptr;
	StatusEffectBarWidgetClass = nullptr;
	CachedWorldMap = nullptr;
	CachedItemWheelTools = nullptr;
	IsDialogActive = false;
	CinematicMode = false;
}

void UW_HUD::NativeConstruct()
{
	Super::NativeConstruct();
	CacheWidgetReferences();
	InitializeBindings();

	// CRITICAL: Collapse all menu/window widgets on init.
	// These are embedded children in the UMG tree (inside WindowsOL) and may default
	// to Visible. They must be Collapsed until explicitly shown by their respective events.
	if (CachedW_RestMenu) CachedW_RestMenu->SetVisibility(ESlateVisibility::Collapsed);
	if (CachedW_Inventory) CachedW_Inventory->SetVisibility(ESlateVisibility::Collapsed);
	if (CachedW_Equipment) CachedW_Equipment->SetVisibility(ESlateVisibility::Collapsed);
	if (CachedW_Status) CachedW_Status->SetVisibility(ESlateVisibility::Collapsed);
	if (CachedW_Crafting) CachedW_Crafting->SetVisibility(ESlateVisibility::Collapsed);
	if (CachedW_Settings) CachedW_Settings->SetVisibility(ESlateVisibility::Collapsed);
	if (CachedW_GameMenu) CachedW_GameMenu->SetVisibility(ESlateVisibility::Collapsed);

	UE_LOG(LogTemp, Log, TEXT("UW_HUD::NativeConstruct - All menu widgets collapsed"));
}

void UW_HUD::NativeDestruct()
{
	if (CachedWorldMap)
	{
		CachedWorldMap->OnMapClosed.RemoveAll(this);
		CachedWorldMap->OnFastTravelRequested.RemoveAll(this);
		CachedWorldMap->RemoveFromParent();
		CachedWorldMap = nullptr;
	}

	Super::NativeDestruct();
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::NativeDestruct"));
}

void UW_HUD::CacheWidgetReferences()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - START"));

	if (!LoadingScreen)
	{
		LoadingScreen = Cast<UW_LoadingScreen>(GetWidgetFromName(TEXT("LoadingScreen")));
	}

	if (!CachedW_Interaction)
	{
		CachedW_Interaction = Cast<UW_Interaction>(GetWidgetFromName(TEXT("W_Interaction")));
	}
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - CachedW_Interaction: %s"),
		CachedW_Interaction ? TEXT("Found") : TEXT("NOT FOUND"));

	// Find CachedW_GameMenu if not already set
	if (!CachedW_GameMenu)
	{
		CachedW_GameMenu = Cast<UW_GameMenu>(GetWidgetFromName(TEXT("W_GameMenu")));
	}
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - CachedW_GameMenu: %s"),
		CachedW_GameMenu ? TEXT("Found") : TEXT("NOT FOUND"));

	// Find child menu widgets
	if (!CachedW_Inventory)
	{
		CachedW_Inventory = Cast<UW_Inventory>(GetWidgetFromName(TEXT("W_Inventory")));
	}
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - CachedW_Inventory: %s"),
		CachedW_Inventory ? TEXT("Found") : TEXT("NOT FOUND"));

	if (!CachedW_Equipment)
	{
		CachedW_Equipment = Cast<UW_Equipment>(GetWidgetFromName(TEXT("W_Equipment")));
	}
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - CachedW_Equipment: %s"),
		CachedW_Equipment ? TEXT("Found") : TEXT("NOT FOUND"));

	if (!CachedW_Crafting)
	{
		CachedW_Crafting = Cast<UW_Crafting>(GetWidgetFromName(TEXT("W_Crafting")));
	}
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - CachedW_Crafting: %s"),
		CachedW_Crafting ? TEXT("Found") : TEXT("NOT FOUND"));

	if (!CachedW_Status)
	{
		CachedW_Status = Cast<UW_Status>(GetWidgetFromName(TEXT("W_Status")));
	}
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - CachedW_Status: %s"),
		CachedW_Status ? TEXT("Found") : TEXT("NOT FOUND"));

	// Bind to CachedW_GameMenu's OnGameMenuWidgetRequest dispatcher
	if (CachedW_GameMenu)
	{
		CachedW_GameMenu->OnGameMenuWidgetRequest.AddUniqueDynamic(this, &UW_HUD::OnGameMenuWidgetRequestHandler);
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - Bound to OnGameMenuWidgetRequest"));
	}

	// Bind to child widget closed dispatchers
	if (CachedW_Inventory)
	{
		CachedW_Inventory->OnInventoryClosed.AddUniqueDynamic(this, &UW_HUD::OnInventoryClosedHandler);
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - Bound to CachedW_Inventory::OnInventoryClosed"));
	}

	if (CachedW_Equipment)
	{
		CachedW_Equipment->OnEquipmentClosed.AddUniqueDynamic(this, &UW_HUD::OnEquipmentClosedHandler);
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - Bound to CachedW_Equipment::OnEquipmentClosed"));
	}

	if (CachedW_Crafting)
	{
		CachedW_Crafting->OnCraftingClosed.AddUniqueDynamic(this, &UW_HUD::OnCraftingClosedHandler);
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - Bound to CachedW_Crafting::OnCraftingClosed"));
	}

	if (CachedW_Status)
	{
		CachedW_Status->OnStatusClosed.AddUniqueDynamic(this, &UW_HUD::OnStatusClosedHandler);
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - Bound to CachedW_Status::OnStatusClosed"));
	}

	// Find CachedW_Settings widget
	if (!CachedW_Settings)
	{
		CachedW_Settings = Cast<UW_Settings>(GetWidgetFromName(TEXT("W_Settings")));
	}
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - CachedW_Settings: %s"),
		CachedW_Settings ? TEXT("Found") : TEXT("NOT FOUND"));

	// Find CachedW_Resources widget (stat bars)
	if (!CachedW_Resources)
	{
		CachedW_Resources = Cast<UW_Resources>(GetWidgetFromName(TEXT("W_Resources")));
	}
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - CachedW_Resources: %s"),
		CachedW_Resources ? TEXT("Found") : TEXT("NOT FOUND"));

	// Find CachedW_RestMenu widget
	if (!CachedW_RestMenu)
	{
		CachedW_RestMenu = Cast<UW_RestMenu>(GetWidgetFromName(TEXT("RestMenu")));
	}
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - CachedW_RestMenu: %s"),
		CachedW_RestMenu ? TEXT("Found") : TEXT("NOT FOUND"));

	// Bind to RestMenu's OnStorageRequested event
	if (CachedW_RestMenu)
	{
		CachedW_RestMenu->OnStorageRequested.AddUniqueDynamic(this, &UW_HUD::OnStorageRequestedHandler);
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - Bound to CachedW_RestMenu::OnStorageRequested"));
	}

	if (CachedW_Settings)
	{
		CachedW_Settings->OnSettingsClosed.AddUniqueDynamic(this, &UW_HUD::OnSettingsClosedHandler);
		CachedW_Settings->OnQuitRequested.AddUniqueDynamic(this, &UW_HUD::OnQuitRequestedHandler);
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - Bound to CachedW_Settings::OnSettingsClosed and OnQuitRequested"));
	}

	// Cache loot notification widgets
	if (!CachedItemLootNotificationsBox)
	{
		CachedItemLootNotificationsBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("ItemLootNotificationsBox")));
	}
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - CachedItemLootNotificationsBox: %s"),
		CachedItemLootNotificationsBox ? TEXT("Found") : TEXT("NOT FOUND"));

	if (!CachedW_FirstLootNotification)
	{
		CachedW_FirstLootNotification = Cast<UW_FirstLootNotification>(GetWidgetFromName(TEXT("W_FirstLootNotification")));
	}
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - CachedW_FirstLootNotification: %s"),
		CachedW_FirstLootNotification ? TEXT("Found") : TEXT("NOT FOUND"));

	// Pre-load loot notification widget class
	if (!LootNotificationWidgetClass)
	{
		LootNotificationWidgetClass = LoadClass<UW_LootNotification>(nullptr,
			TEXT("/Game/SoulslikeFramework/Widgets/HUD/W_LootNotification.W_LootNotification_C"));
	}
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - LootNotificationWidgetClass: %s"),
		LootNotificationWidgetClass ? TEXT("Loaded") : TEXT("NOT LOADED"));

	// Cache status effect widgets
	if (!CachedStatusEffectBox)
	{
		CachedStatusEffectBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("StatusEffectBox")));
	}
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - CachedStatusEffectBox: %s"),
		CachedStatusEffectBox ? TEXT("Found") : TEXT("NOT FOUND"));

	if (!CachedW_StatusEffectNotification)
	{
		CachedW_StatusEffectNotification = Cast<UW_StatusEffectNotification>(GetWidgetFromName(TEXT("W_StatusEffectNotification")));
	}
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - CachedW_StatusEffectNotification: %s"),
		CachedW_StatusEffectNotification ? TEXT("Found") : TEXT("NOT FOUND"));

	// Pre-load status effect bar widget class
	if (!StatusEffectBarWidgetClass)
	{
		StatusEffectBarWidgetClass = LoadClass<UW_StatusEffectBar>(nullptr,
			TEXT("/Game/SoulslikeFramework/Widgets/HUD/W_StatusEffectBar.W_StatusEffectBar_C"));
	}
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - StatusEffectBarWidgetClass: %s"),
		StatusEffectBarWidgetClass ? TEXT("Loaded") : TEXT("NOT LOADED"));

	// Cache boss health bar widget
	if (!CachedBossHealthbar)
	{
		CachedBossHealthbar = Cast<UW_Boss_Healthbar>(GetWidgetFromName(TEXT("W_Boss_Healthbar")));
	}
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - CachedBossHealthbar: %s"),
		CachedBossHealthbar ? TEXT("Found") : TEXT("NOT FOUND"));

	// Cache W_Radar widget for radar/compass functionality
	if (!CachedW_Radar)
	{
		CachedW_Radar = Cast<UW_Radar>(GetWidgetFromName(TEXT("W_Radar")));
	}
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - CachedW_Radar: %s"),
		CachedW_Radar ? TEXT("Found") : TEXT("NOT FOUND"));

	// Cache W_NPC_Window widget for NPC dialog
	if (!CachedW_NPC_Window)
	{
		CachedW_NPC_Window = Cast<UW_NPC_Window>(GetWidgetFromName(TEXT("W_NPC_Window")));

		// If widget not found in tree, create it dynamically
		if (!CachedW_NPC_Window)
		{
			UE_LOG(LogTemp, Warning, TEXT("UW_HUD::CacheWidgetReferences - W_NPC_Window not in widget tree, creating dynamically"));

			// Load the Blueprint class
			UClass* NPCWindowClass = LoadClass<UW_NPC_Window>(nullptr,
				TEXT("/Game/SoulslikeFramework/Widgets/Vendor/W_NPC_Window.W_NPC_Window_C"));

			if (NPCWindowClass)
			{
				CachedW_NPC_Window = CreateWidget<UW_NPC_Window>(GetOwningPlayer(), NPCWindowClass);

				if (CachedW_NPC_Window)
				{
					// Find the root Overlay and add the widget
					UOverlay* RootOverlay = Cast<UOverlay>(GetWidgetFromName(TEXT("Overlay_0")));
					if (!RootOverlay)
					{
						// Try alternate name patterns
						RootOverlay = Cast<UOverlay>(GetRootWidget());
					}

					if (RootOverlay)
					{
						UOverlaySlot* OverlaySlot = RootOverlay->AddChildToOverlay(CachedW_NPC_Window);
						if (OverlaySlot)
						{
							OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
							OverlaySlot->SetVerticalAlignment(VAlign_Fill);
						}
						CachedW_NPC_Window->SetVisibility(ESlateVisibility::Collapsed);
						UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - Created W_NPC_Window dynamically and added to Overlay"));
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("UW_HUD::CacheWidgetReferences - Could not find root Overlay to add W_NPC_Window"));
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("UW_HUD::CacheWidgetReferences - CreateWidget failed for W_NPC_Window"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("UW_HUD::CacheWidgetReferences - Could not load W_NPC_Window Blueprint class"));
			}
		}
	}
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - CachedW_NPC_Window: %s"),
		CachedW_NPC_Window ? TEXT("Found") : TEXT("NOT FOUND"));

	// Cache W_Dialog widget for dialog text display (named "W_Dialog" in UMG)
	if (!CachedW_Dialog)
	{
		UWidget* RawDialogWidget = GetWidgetFromName(TEXT("W_Dialog"));
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - GetWidgetFromName(W_Dialog) returned: %s"),
			RawDialogWidget ? TEXT("VALID pointer") : TEXT("nullptr"));

		if (RawDialogWidget)
		{
			UClass* WidgetClass = RawDialogWidget->GetClass();
			UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - W_Dialog raw widget class: %s"),
				WidgetClass ? *WidgetClass->GetName() : TEXT("null"));
			UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - W_Dialog raw widget class path: %s"),
				WidgetClass ? *WidgetClass->GetPathName() : TEXT("null"));

			// Check if it's a subclass of UW_Dialog
			UClass* ExpectedClass = UW_Dialog::StaticClass();
			bool bIsSubclass = WidgetClass && WidgetClass->IsChildOf(ExpectedClass);
			UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - IsChildOf(UW_Dialog): %s"),
				bIsSubclass ? TEXT("YES") : TEXT("NO"));

			// Log the class hierarchy
			if (WidgetClass)
			{
				FString Hierarchy;
				UClass* CurrentClass = WidgetClass;
				while (CurrentClass)
				{
					if (!Hierarchy.IsEmpty()) Hierarchy += TEXT(" -> ");
					Hierarchy += CurrentClass->GetName();
					CurrentClass = CurrentClass->GetSuperClass();
				}
				UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - W_Dialog class hierarchy: %s"), *Hierarchy);
			}
		}

		CachedW_Dialog = Cast<UW_Dialog>(RawDialogWidget);
	}
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - CachedW_Dialog: %s"),
		CachedW_Dialog ? TEXT("Found") : TEXT("NOT FOUND"));

	// Cache ItemWheel_Tools and configure SlotsToTrack if empty
	// This fixes throwing knives not appearing in item wheel after C++ migration
	if (!CachedItemWheelTools)
	{
		CachedItemWheelTools = Cast<UW_ItemWheelSlot>(GetWidgetFromName(TEXT("ItemWheel_Tools")));
	}
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - CachedItemWheelTools: %s"),
		CachedItemWheelTools ? TEXT("Found") : TEXT("NOT FOUND"));

	// If SlotsToTrack is empty, configure it with tool slot tags
	// SlotsToTrack is an instance-editable property that may have been lost during Blueprint reparenting
	if (CachedItemWheelTools && CachedItemWheelTools->SlotsToTrack.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::CacheWidgetReferences - ItemWheel_Tools SlotsToTrack is EMPTY! Configuring with tool slots..."));

		// Add all 5 tool slots (matching AC_EquipmentManager::ToolSlots)
		FGameplayTag ToolSlot1 = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Tool 1"), false);
		FGameplayTag ToolSlot2 = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Tool 2"), false);
		FGameplayTag ToolSlot3 = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Tool 3"), false);
		FGameplayTag ToolSlot4 = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Tool 4"), false);
		FGameplayTag ToolSlot5 = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Tool 5"), false);

		if (ToolSlot1.IsValid()) CachedItemWheelTools->SlotsToTrack.AddTag(ToolSlot1);
		if (ToolSlot2.IsValid()) CachedItemWheelTools->SlotsToTrack.AddTag(ToolSlot2);
		if (ToolSlot3.IsValid()) CachedItemWheelTools->SlotsToTrack.AddTag(ToolSlot3);
		if (ToolSlot4.IsValid()) CachedItemWheelTools->SlotsToTrack.AddTag(ToolSlot4);
		if (ToolSlot5.IsValid()) CachedItemWheelTools->SlotsToTrack.AddTag(ToolSlot5);

		UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - Configured ItemWheel_Tools with %d tool slots"),
			CachedItemWheelTools->SlotsToTrack.Num());
	}
	else if (CachedItemWheelTools)
	{
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - ItemWheel_Tools already has %d slots configured"),
			CachedItemWheelTools->SlotsToTrack.Num());
	}

	// Bind to ItemWheel_Tools::OnItemWheelSlotSelected to update ActiveToolSlot
	if (CachedItemWheelTools)
	{
		CachedItemWheelTools->OnItemWheelSlotSelected.AddDynamic(this, &UW_HUD::OnItemWheelToolsSlotSelected);
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - Bound to ItemWheel_Tools::OnItemWheelSlotSelected"));
	}

	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - END"));
}

bool UW_HUD::GetTargetWidgetVisibility_Implementation(UUserWidget* Widget)
{
	// NOTE: Don't check IsInViewport() - child widgets inside W_HUD are not
	// added to viewport separately, so IsInViewport() returns false for them.
	// Just check the widget's visibility directly.
	if (Widget)
	{
		ESlateVisibility WidgetVisibility = Widget->GetVisibility();
		return WidgetVisibility == ESlateVisibility::Visible ||
			   WidgetVisibility == ESlateVisibility::SelfHitTestInvisible ||
			   WidgetVisibility == ESlateVisibility::HitTestInvisible;
	}
	return false;
}

void UW_HUD::BindToStatUpdate_Implementation(const TArray<USLFStatBase*>& AllStats, const TArray<TSubclassOf<USLFStatBase>>& StatsToListenFor)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::BindToStatUpdate - Binding to %d stats, filtering by %d types"),
		AllStats.Num(), StatsToListenFor.Num());

	for (USLFStatBase* Stat : AllStats)
	{
		if (!Stat)
		{
			continue;
		}

		bool bShouldBind = false;
		if (StatsToListenFor.Num() == 0)
		{
			bShouldBind = true;
		}
		else
		{
			UClass* StatClass = Stat->GetClass();
			for (const TSubclassOf<USLFStatBase>& FilterClass : StatsToListenFor)
			{
				if (FilterClass && StatClass->IsChildOf(FilterClass))
				{
					bShouldBind = true;
					break;
				}
			}
		}

		if (bShouldBind)
		{
			Stat->OnStatUpdated.AddUniqueDynamic(this, &UW_HUD::OnStatUpdatedHandler);
			UE_LOG(LogTemp, Log, TEXT("  Bound to stat: %s (%s)"),
				*Stat->StatInfo.Tag.ToString(),
				*Stat->GetClass()->GetName());
		}
	}
}

bool UW_HUD::GetGameMenuVisibility_Implementation()
{
	if (CachedW_GameMenu)
	{
		return GetTargetWidgetVisibility(CachedW_GameMenu);
	}
	else if (UW_GameMenu* GameMenu = Cast<UW_GameMenu>(GetWidgetFromName(TEXT("W_GameMenu"))))
	{
		return GetTargetWidgetVisibility(GameMenu);
	}
	return false;
}

bool UW_HUD::IsRestMenuHud_Implementation()
{
	return false;
}

void UW_HUD::InitializeBindings_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::InitializeBindings"));

	// Also try to bind CachedW_GameMenu here in case CacheWidgetReferences missed it
	if (!CachedW_GameMenu)
	{
		CachedW_GameMenu = Cast<UW_GameMenu>(GetWidgetFromName(TEXT("W_GameMenu")));
		if (CachedW_GameMenu)
		{
			CachedW_GameMenu->OnGameMenuWidgetRequest.AddUniqueDynamic(this, &UW_HUD::OnGameMenuWidgetRequestHandler);
			UE_LOG(LogTemp, Log, TEXT("UW_HUD::InitializeBindings - Bound to CachedW_GameMenu::OnGameMenuWidgetRequest"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UW_HUD::InitializeBindings - CachedW_GameMenu not found!"));
		}
	}

	// Bind to InventoryManager's OnItemLooted event
	// Use FindComponentByClass to work with any controller type (Blueprint or C++)
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (UInventoryManagerComponent* InvMgr = PC->FindComponentByClass<UInventoryManagerComponent>())
		{
			InvMgr->OnItemLooted.AddUniqueDynamic(this, &UW_HUD::OnItemLootedHandler);
			UE_LOG(LogTemp, Log, TEXT("UW_HUD::InitializeBindings - Bound to InventoryManager::OnItemLooted"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UW_HUD::InitializeBindings - InventoryManagerComponent not found on controller"));
		}

		// Bind to StatManager's OnStatsInitialized event and stat updates
		if (APawn* Pawn = PC->GetPawn())
		{
			if (UStatManagerComponent* StatManager = Pawn->FindComponentByClass<UStatManagerComponent>())
			{
				// Bind to OnStatsInitialized for future initializations
				StatManager->OnStatsInitialized.AddUniqueDynamic(this, &UW_HUD::EventOnStatsInitialized);
				UE_LOG(LogTemp, Log, TEXT("UW_HUD::InitializeBindings - Bound to StatManager::OnStatsInitialized"));

				// If stats are already initialized (component already ran BeginPlay), bind now
				TArray<UObject*> StatObjects;
				TMap<TSubclassOf<UObject>, FGameplayTag> StatClassesAndCategories;
				StatManager->GetAllStats(StatObjects, StatClassesAndCategories);

				if (StatObjects.Num() > 0)
				{
					UE_LOG(LogTemp, Log, TEXT("UW_HUD::InitializeBindings - Stats already initialized, binding now..."));
					EventOnStatsInitialized();
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("UW_HUD::InitializeBindings - StatManagerComponent not found on Pawn"));
			}

			// Bind to StatusEffectManager's OnStatusEffectAdded event
			// Try AC_StatusEffectManager first (Blueprint), then StatusEffectManagerComponent (C++)
			UAC_StatusEffectManager* AC_StatusEffectMgr = Pawn->FindComponentByClass<UAC_StatusEffectManager>();
			if (AC_StatusEffectMgr)
			{
				AC_StatusEffectMgr->OnStatusEffectAdded.AddUniqueDynamic(this, &UW_HUD::EventOnStatusEffectAdded);
				UE_LOG(LogTemp, Log, TEXT("UW_HUD::InitializeBindings - Bound to AC_StatusEffectManager::OnStatusEffectAdded"));
			}
			else
			{
				UStatusEffectManagerComponent* StatusEffectMgr = Pawn->FindComponentByClass<UStatusEffectManagerComponent>();
				if (StatusEffectMgr)
				{
					StatusEffectMgr->OnStatusEffectAdded.AddUniqueDynamic(this, &UW_HUD::OnStatusEffectAddedFromComponent);
					UE_LOG(LogTemp, Log, TEXT("UW_HUD::InitializeBindings - Bound to StatusEffectManagerComponent::OnStatusEffectAdded"));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("UW_HUD::InitializeBindings - No StatusEffectManager found on Pawn"));
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UW_HUD::InitializeBindings - No Pawn available yet"));
		}
	}
}

void UW_HUD::SerializeItemWheelData_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::SerializeItemWheelData"));
}

void UW_HUD::InitializeLoadedItemWheelSlotData_Implementation(const TArray<FSLFItemWheelSaveInfo>& LoadedData)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::InitializeLoadedItemWheelSlotData - %d items"), LoadedData.Num());
}

bool UW_HUD::GetDialogWindowVisibility_Implementation()
{
	return IsDialogActive;
}

void UW_HUD::CloseAllMenus_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CloseAllMenus"));
	EventCloseInventory();
	EventCloseEquipment();
	EventCloseCrafting();
	EventCloseStatus();
	EventCloseSettings();
	EventCloseGameMenu();
}

void UW_HUD::EventToggleUiMode_Implementation(bool bToggled)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventToggleUiMode - bToggled: %s"), bToggled ? TEXT("true") : TEXT("false"));

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		if (bToggled)
		{
			PC->SetShowMouseCursor(true);
			FInputModeUIOnly InputMode;
			PC->SetInputMode(InputMode);
		}
		else
		{
			PC->SetShowMouseCursor(false);
			FInputModeGameOnly InputMode;
			PC->SetInputMode(InputMode);
		}
	}
}

void UW_HUD::EventShowInventory_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowInventory"));
	if (UWidget* InventoryWidget = GetWidgetFromName(TEXT("W_Inventory")))
	{
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowInventory - Found CachedW_Inventory, setting visible"));
		InventoryWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::EventShowInventory - CachedW_Inventory not found!"));
	}
	EventToggleUiMode(true);
}

void UW_HUD::EventCloseInventory_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventCloseInventory"));
	if (UWidget* InventoryWidget = GetWidgetFromName(TEXT("W_Inventory")))
	{
		InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UW_HUD::EventShowEquipment_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowEquipment"));
	if (UWidget* EquipmentWidget = GetWidgetFromName(TEXT("W_Equipment")))
	{
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowEquipment - Found CachedW_Equipment, setting visible"));
		EquipmentWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::EventShowEquipment - CachedW_Equipment not found!"));
	}
	EventToggleUiMode(true);
}

void UW_HUD::EventCloseEquipment_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventCloseEquipment"));
	if (UWidget* EquipmentWidget = GetWidgetFromName(TEXT("W_Equipment")))
	{
		EquipmentWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UW_HUD::EventShowStatus_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowStatus"));
	if (UWidget* StatusWidget = GetWidgetFromName(TEXT("W_Status")))
	{
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowStatus - Found CachedW_Status, setting visible"));
		StatusWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::EventShowStatus - CachedW_Status not found!"));
	}
	EventToggleUiMode(true);
}

void UW_HUD::EventCloseStatus_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventCloseStatus"));
	if (UWidget* StatusWidget = GetWidgetFromName(TEXT("W_Status")))
	{
		StatusWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UW_HUD::EventShowCrafting_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowCrafting"));
	if (UWidget* CraftingWidget = GetWidgetFromName(TEXT("W_Crafting")))
	{
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowCrafting - Found CachedW_Crafting, setting visible"));
		CraftingWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::EventShowCrafting - CachedW_Crafting not found!"));
	}
	EventToggleUiMode(true);
}

void UW_HUD::EventCloseCrafting_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventCloseCrafting"));
	if (UWidget* CraftingWidget = GetWidgetFromName(TEXT("W_Crafting")))
	{
		CraftingWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UW_HUD::EventShowSettings_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowSettings"));
	if (CachedW_Settings)
	{
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowSettings - Found CachedW_Settings, setting visible and focus"));
		CachedW_Settings->SetVisibility(ESlateVisibility::Visible);
		// CRITICAL: Set keyboard focus so gamepad R1/L1 go to Settings, not GameMenu
		CachedW_Settings->SetKeyboardFocus();
	}
	else if (UWidget* SettingsWidget = GetWidgetFromName(TEXT("W_Settings")))
	{
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowSettings - Using GetWidgetFromName fallback"));
		SettingsWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::EventShowSettings - CachedW_Settings not found!"));
	}
	EventToggleUiMode(true);
}

void UW_HUD::EventCloseSettings_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventCloseSettings"));
	if (UWidget* SettingsWidget = GetWidgetFromName(TEXT("W_Settings")))
	{
		SettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UW_HUD::EventShowGameMenu_Implementation()
{
	// Block menu opens during cinematic mode
	if (CinematicMode)
	{
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowGameMenu - BLOCKED (CinematicMode active)"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowGameMenu"));
	if (CachedW_GameMenu)
	{
		CachedW_GameMenu->SetVisibility(ESlateVisibility::Visible);
		// Set keyboard focus so gamepad/keyboard input works after returning from sub-menu
		CachedW_GameMenu->SetKeyboardFocus();
	}
	else if (UWidget* GameMenuWidget = GetWidgetFromName(TEXT("W_GameMenu")))
	{
		GameMenuWidget->SetVisibility(ESlateVisibility::Visible);
	}
	EventToggleUiMode(true);

	// Set ActiveWidgetTag and switch input context on PlayerController
	if (ASLFPlayerController* PC = Cast<ASLFPlayerController>(GetOwningPlayer()))
	{
		// Set active widget tag for navigation routing
		PC->ActiveWidgetTag = FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Backend.Widgets.GameMenu")));
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowGameMenu - Set ActiveWidgetTag to GameMenu"));

		// Switch input context: Enable menu context, disable gameplay context
		TArray<UInputMappingContext*> ToEnable;
		TArray<UInputMappingContext*> ToDisable;

		if (PC->IMC_NavigableMenu)
		{
			ToEnable.Add(PC->IMC_NavigableMenu);
		}
		if (PC->IMC_Gameplay)
		{
			ToDisable.Add(PC->IMC_Gameplay);
		}

		PC->SwitchInputContextInternal(ToEnable, ToDisable);
	}
}

void UW_HUD::EventCloseGameMenu_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventCloseGameMenu"));
	if (CachedW_GameMenu)
	{
		CachedW_GameMenu->SetVisibility(ESlateVisibility::Collapsed);
	}
	else if (UWidget* GameMenuWidget = GetWidgetFromName(TEXT("W_GameMenu")))
	{
		GameMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	EventToggleUiMode(false);

	// Clear ActiveWidgetTag and switch input context back to gameplay
	if (ASLFPlayerController* PC = Cast<ASLFPlayerController>(GetOwningPlayer()))
	{
		// Clear active widget tag
		PC->ActiveWidgetTag = FGameplayTag();
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventCloseGameMenu - Cleared ActiveWidgetTag"));

		// Switch input context: Enable gameplay context, disable menu context
		TArray<UInputMappingContext*> ToEnable;
		TArray<UInputMappingContext*> ToDisable;

		if (PC->IMC_Gameplay)
		{
			ToEnable.Add(PC->IMC_Gameplay);
		}
		if (PC->IMC_NavigableMenu)
		{
			ToDisable.Add(PC->IMC_NavigableMenu);
		}

		PC->SwitchInputContextInternal(ToEnable, ToDisable);
	}
}

void UW_HUD::EventShowWorldMap_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowWorldMap"));

	// Create WorldMap widget on first use
	if (!CachedWorldMap)
	{
		if (APlayerController* PC = GetOwningPlayer())
		{
			CachedWorldMap = CreateWidget<UW_WorldMap>(PC);
			if (CachedWorldMap)
			{
				CachedWorldMap->AddToViewport(100); // High Z-order to overlay everything
				CachedWorldMap->SetVisibility(ESlateVisibility::Collapsed);
				CachedWorldMap->OnMapClosed.AddDynamic(this, &UW_HUD::OnWorldMapClosedHandler);
				CachedWorldMap->OnFastTravelRequested.AddDynamic(this, &UW_HUD::OnFastTravelRequestedHandler);
				UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowWorldMap - Created WorldMap widget"));
			}
		}
	}

	if (CachedWorldMap)
	{
		CachedWorldMap->RefreshMarkers();
		CachedWorldMap->SetVisibility(ESlateVisibility::Visible);
		CachedWorldMap->SetFocus();
	}
}

void UW_HUD::EventCloseWorldMap_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventCloseWorldMap"));
	if (CachedWorldMap)
	{
		CachedWorldMap->SetVisibility(ESlateVisibility::Collapsed);
	}

	// Return to game menu
	EventShowGameMenu();
}

void UW_HUD::OnWorldMapClosedHandler()
{
	EventCloseWorldMap();
}

void UW_HUD::OnFastTravelRequestedHandler(const FSLFRestPointSaveInfo& Destination)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnFastTravelRequestedHandler - %s"), *Destination.LocationName.ToString());

	// Close world map
	EventCloseWorldMap();

	// Close game menu
	EventCloseGameMenu();

	// Execute fast travel via SLFPlayerController
	if (APlayerController* PC = GetOwningPlayer())
	{
		// Include the SLFPlayerController check
		if (ASLFPlayerController* SLFPC = Cast<ASLFPlayerController>(PC))
		{
			SLFPC->ExecuteFastTravel(Destination);
		}
	}
}

void UW_HUD::EventOnExitStorage_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventOnExitStorage"));
	EventCloseInventory();
	EventToggleUiMode(false);
}

void UW_HUD::OnStatUpdatedHandler(USLFStatBase* UpdatedStat, double Change, bool bUpdateAffectedStats, ESLFValueType ValueType)
{
	EventOnStatUpdated(UpdatedStat, Change, bUpdateAffectedStats, ValueType);
}

void UW_HUD::EventOnStatUpdated_Implementation(USLFStatBase* UpdatedStat, double Change, bool bUpdateAffectedStats, ESLFValueType ValueType)
{
	if (!UpdatedStat)
	{
		return;
	}

	FGameplayTag StatTag = UpdatedStat->StatInfo.Tag;
	double CurrentValue = UpdatedStat->StatInfo.CurrentValue;
	double MaxValue = UpdatedStat->StatInfo.MaxValue;
	double Percent = UpdatedStat->CalculatePercent();

	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventOnStatUpdated - Stat: %s, Value: %.1f/%.1f (%.1f%%), Change: %.1f"),
		*StatTag.ToString(), CurrentValue, MaxValue, Percent, Change);

	// Forward stat update to W_Resources for visual update
	if (CachedW_Resources)
	{
		CachedW_Resources->EventUpdateStat(UpdatedStat->StatInfo);
	}
}

void UW_HUD::EventOnStatusEffectAdded_Implementation(UB_StatusEffect* StatusEffect)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventOnStatusEffectAdded - Effect: %s"),
		StatusEffect ? *StatusEffect->GetName() : TEXT("None"));

	if (!IsValid(StatusEffect))
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::EventOnStatusEffectAdded - Invalid StatusEffect!"));
		return;
	}

	if (!CachedStatusEffectBox)
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::EventOnStatusEffectAdded - No StatusEffectBox!"));
		return;
	}

	if (!StatusEffectBarWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::EventOnStatusEffectAdded - No StatusEffectBarWidgetClass!"));
		return;
	}

	// Create a new W_StatusEffectBar widget
	UW_StatusEffectBar* NewBar = CreateWidget<UW_StatusEffectBar>(GetOwningPlayer(), StatusEffectBarWidgetClass);
	if (!NewBar)
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::EventOnStatusEffectAdded - Failed to create W_StatusEffectBar!"));
		return;
	}

	// Set the effect reference on the bar
	NewBar->Effect = StatusEffect;

	// Setup the effect binding (binds to OnBuildupUpdated, OnStatusEffectFinished)
	NewBar->SetupEffect();

	// Add to the StatusEffectBox
	CachedStatusEffectBox->AddChild(NewBar);

	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventOnStatusEffectAdded - Created status effect bar for %s"),
		*StatusEffect->GetName());
}

void UW_HUD::EventOnStatsInitialized_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventOnStatsInitialized - Binding to player stats"));

	// Get the player pawn's StatManagerComponent
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::EventOnStatsInitialized - No PlayerController!"));
		return;
	}

	APawn* Pawn = PC->GetPawn();
	if (!Pawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::EventOnStatsInitialized - No Pawn!"));
		return;
	}

	UStatManagerComponent* StatManager = Pawn->FindComponentByClass<UStatManagerComponent>();
	if (!StatManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::EventOnStatsInitialized - No StatManagerComponent!"));
		return;
	}

	// Get all stats and bind to their OnStatUpdated delegates
	TArray<UObject*> StatObjects;
	TMap<TSubclassOf<UObject>, FGameplayTag> StatClassesAndCategories;
	StatManager->GetAllStats(StatObjects, StatClassesAndCategories);

	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventOnStatsInitialized - Found %d stats"), StatObjects.Num());

	for (UObject* StatObj : StatObjects)
	{
		USLFStatBase* Stat = Cast<USLFStatBase>(StatObj);
		if (Stat)
		{
			Stat->OnStatUpdated.AddUniqueDynamic(this, &UW_HUD::OnStatUpdatedHandler);
			UE_LOG(LogTemp, Log, TEXT("  Bound to stat: %s (%.1f/%.1f)"),
				*Stat->StatInfo.Tag.ToString(),
				Stat->StatInfo.CurrentValue,
				Stat->StatInfo.MaxValue);
		}
	}
}

void UW_HUD::EventOnBuffDetected_Implementation(UPDA_Buff* Buff, bool bAdded)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventOnBuffDetected - Buff: %s, Added: %s"),
		Buff ? *Buff->GetName() : TEXT("None"),
		bAdded ? TEXT("true") : TEXT("false"));
}

void UW_HUD::EventOnItemLooted_Implementation(const FSLFItemInfo& InItem, int32 InAmount, bool bExists)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventOnItemLooted - Item: %s, Amount: %d, Exists: %s"),
		*InItem.DisplayName.ToString(), InAmount, bExists ? TEXT("true") : TEXT("false"));

	// If this is a new item (doesn't exist in inventory yet), show the "NEW" notification
	if (!bExists && CachedW_FirstLootNotification)
	{
		// Get icon texture from item
		UTexture2D* IconTexture = nullptr;
		if (!InItem.IconSmall.IsNull())
		{
			IconTexture = InItem.IconSmall.LoadSynchronous();
		}

		// Show the "NEW" first loot notification with item name and icon
		CachedW_FirstLootNotification->EventShowNotification(InItem.DisplayName, IconTexture, 3.0);

		UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventOnItemLooted - Showing NEW notification for first-time item: %s"),
			*InItem.DisplayName.ToString());
	}

	// Create and show the regular loot notification
	if (CachedItemLootNotificationsBox && LootNotificationWidgetClass)
	{
		UW_LootNotification* LootNotif = CreateWidget<UW_LootNotification>(GetOwningPlayer(), LootNotificationWidgetClass);
		if (LootNotif)
		{
			// Get icon texture
			UTexture2D* IconTexture = nullptr;
			if (!InItem.IconSmall.IsNull())
			{
				IconTexture = InItem.IconSmall.LoadSynchronous();
			}

			// Setup the notification with item info
			LootNotif->ItemName = InItem.DisplayName;
			LootNotif->ItemLootedAmount = InAmount;
			LootNotif->ItemTexture = InItem.IconSmall;  // Copy the soft object ptr directly
			LootNotif->Duration = 3.0;

			// Add to the notifications box
			CachedItemLootNotificationsBox->AddChild(LootNotif);

			UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventOnItemLooted - Created loot notification widget"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::EventOnItemLooted - Cannot create notification: Box=%s, Class=%s"),
			CachedItemLootNotificationsBox ? TEXT("OK") : TEXT("NULL"),
			LootNotificationWidgetClass ? TEXT("OK") : TEXT("NULL"));
	}
}

void UW_HUD::EventOnAbilityInfoInvalidated_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventOnAbilityInfoInvalidated"));
}

void UW_HUD::EventOnDataLoaded_Implementation(const FSLFSaveGameInfo& LoadedData)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventOnDataLoaded"));
}

void UW_HUD::EventShowInteractableWidget_Implementation(AActor* Interactable)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowInteractableWidget - Interactable: %s"),
		Interactable ? *Interactable->GetName() : TEXT("None"));

	// Cache reference if not set
	if (!CachedW_Interaction)
	{
		CachedW_Interaction = Cast<UW_Interaction>(GetWidgetFromName(TEXT("W_Interaction")));
	}

	if (!CachedW_Interaction)
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::EventShowInteractableWidget - CachedW_Interaction widget not found!"));
		return;
	}

	if (!Interactable)
	{
		return;
	}

	// Try C++ interface first (ISLFInteractableInterface::TryGetItemInfo returns FSLFItemInfo)
	if (Interactable->GetClass()->ImplementsInterface(USLFInteractableInterface::StaticClass()))
	{
		FSLFItemInfo ItemInfo = ISLFInteractableInterface::Execute_TryGetItemInfo(Interactable);
		// Check DisplayName instead of ItemTag (ItemTag wasn't migrated, but DisplayName was)
		if (!ItemInfo.DisplayName.IsEmpty())
		{
			UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowInteractableWidget - Via ISLFInteractableInterface, showing item: %s"),
				*ItemInfo.DisplayName.ToString());
			CachedW_Interaction->EventOnItemOverlap(ItemInfo);
			return;
		}
	}

	// Fallback: Try Blueprint interface (IBPI_Interactable::TryGetItemInfo has out param signature)
	if (Interactable->GetClass()->ImplementsInterface(UBPI_Interactable::StaticClass()))
	{
		FSLFItemInfo ItemInfo;
		IBPI_Interactable::Execute_TryGetItemInfo(Interactable, ItemInfo);
		// Check DisplayName instead of ItemTag
		if (!ItemInfo.DisplayName.IsEmpty())
		{
			UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowInteractableWidget - Via IBPI_Interactable, showing item: %s"),
				*ItemInfo.DisplayName.ToString());
			CachedW_Interaction->EventOnItemOverlap(ItemInfo);
			return;
		}
	}

	// Get interaction text from the actor - check multiple types
	FText InteractionText = FText::GetEmpty();
	if (ASLFInteractableBase* InteractableBase = Cast<ASLFInteractableBase>(Interactable))
	{
		InteractionText = InteractableBase->InteractionText;
	}
	else if (ASLFBossDoor* BossDoor = Cast<ASLFBossDoor>(Interactable))
	{
		InteractionText = BossDoor->InteractionText;
	}

	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowInteractableWidget - Showing interactable overlap for text: %s"),
		*InteractionText.ToString());
	CachedW_Interaction->EventOnInteractableOverlap(InteractionText);
}

void UW_HUD::EventHideInteractionWidget_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventHideInteractionWidget"));

	// Cache reference if not set
	if (!CachedW_Interaction)
	{
		CachedW_Interaction = Cast<UW_Interaction>(GetWidgetFromName(TEXT("W_Interaction")));
	}

	if (CachedW_Interaction)
	{
		CachedW_Interaction->EventHide();
	}
}

void UW_HUD::EventSetupRestingPointWidget_Implementation(AActor* TargetCampfire)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventSetupRestingPointWidget - Campfire: %s"),
		TargetCampfire ? *TargetCampfire->GetName() : TEXT("None"));

	// Switch ViewportSwitcher to show WindowsOL (index 1) which contains RestMenu
	EventSwitchViewport(1);

	// Cache RestMenu if needed
	if (!CachedW_RestMenu)
	{
		CachedW_RestMenu = Cast<UW_RestMenu>(GetWidgetFromName(TEXT("RestMenu")));
	}

	if (CachedW_RestMenu)
	{
		CachedW_RestMenu->EventSetupRestingPoint(TargetCampfire);
	}

	EventToggleUiMode(true);

	// Set ActiveWidgetTag for navigation routing and switch input context
	// Try APC_SoulslikeFramework first (the actual game's controller), then ASLFPlayerController as fallback
	APlayerController* OwningPlayer = GetOwningPlayer();
	if (APC_SoulslikeFramework* PC = Cast<APC_SoulslikeFramework>(OwningPlayer))
	{
		// Set active widget tag for RestMenu navigation
		PC->ActiveWidgetTag = FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Backend.Widgets.RestMenu")));
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventSetupRestingPointWidget - Set ActiveWidgetTag to RestMenu (via APC_SoulslikeFramework)"));

		// Switch input context: Enable menu context, disable gameplay context
		TArray<UInputMappingContext*> ToEnable;
		TArray<UInputMappingContext*> ToDisable;

		if (PC->NavigableMenuMappingContext)
		{
			ToEnable.Add(PC->NavigableMenuMappingContext);
		}
		if (PC->GameplayMappingContext)
		{
			ToDisable.Add(PC->GameplayMappingContext);
		}

		// Use interface method for input context switch
		IBPI_Controller::Execute_SwitchInputContext(PC, ToEnable, ToDisable);
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventSetupRestingPointWidget - Switched to menu input context"));
	}
	else if (ASLFPlayerController* SLFController = Cast<ASLFPlayerController>(OwningPlayer))
	{
		// Fallback to ASLFPlayerController
		SLFController->ActiveWidgetTag = FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Backend.Widgets.RestMenu")));
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventSetupRestingPointWidget - Set ActiveWidgetTag to RestMenu (via ASLFPlayerController)"));

		TArray<UInputMappingContext*> ToEnable;
		TArray<UInputMappingContext*> ToDisable;

		if (SLFController->IMC_NavigableMenu)
		{
			ToEnable.Add(SLFController->IMC_NavigableMenu);
		}
		if (SLFController->IMC_Gameplay)
		{
			ToDisable.Add(SLFController->IMC_Gameplay);
		}

		SLFController->SwitchInputContextInternal(ToEnable, ToDisable);
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventSetupRestingPointWidget - Switched to menu input context"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::EventSetupRestingPointWidget - Could not cast to APC_SoulslikeFramework or ASLFPlayerController!"));
	}

	// Bind to resting point's OnExited to clear navigation when menu closes
	if (TargetCampfire)
	{
		if (ASLFRestingPointBase* SLFRestPoint = Cast<ASLFRestingPointBase>(TargetCampfire))
		{
			SLFRestPoint->OnExited.AddUniqueDynamic(this, &UW_HUD::OnRestMenuClosedHandler);
		}
		else if (AB_RestingPoint* OldRestPoint = Cast<AB_RestingPoint>(TargetCampfire))
		{
			OldRestPoint->OnExited.AddUniqueDynamic(this, &UW_HUD::OnRestMenuClosedHandler);
		}
	}
}

void UW_HUD::EventShowNpcInteractionWidget_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowNpcInteractionWidget"));
	if (UWidget* NpcWindow = GetWidgetFromName(TEXT("NpcWindow")))
	{
		NpcWindow->SetVisibility(ESlateVisibility::Visible);
	}
	EventToggleUiMode(true);
}

void UW_HUD::EventSetupDialog_Implementation(const FText& InText)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventSetupDialog - Text: '%s' (len=%d)"), *InText.ToString(), InText.ToString().Len());
	IsDialogActive = true;

	// Set ActiveWidgetTag so navigation input routes to dialog
	if (ASLFPlayerController* PC = Cast<ASLFPlayerController>(GetOwningPlayer()))
	{
		PC->ActiveWidgetTag = FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Backend.Widgets.Dialog")));
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventSetupDialog - Set ActiveWidgetTag to Dialog"));
	}

	// Cache W_Dialog if not already cached
	if (!CachedW_Dialog)
	{
		UWidget* RawDialogWidget = GetWidgetFromName(TEXT("W_Dialog"));
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventSetupDialog - GetWidgetFromName(W_Dialog) returned: %s"),
			RawDialogWidget ? TEXT("VALID") : TEXT("nullptr"));

		if (RawDialogWidget)
		{
			UClass* WidgetClass = RawDialogWidget->GetClass();
			UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventSetupDialog - Widget class: %s, path: %s"),
				WidgetClass ? *WidgetClass->GetName() : TEXT("null"),
				WidgetClass ? *WidgetClass->GetPathName() : TEXT("null"));

			bool bIsSubclass = WidgetClass && WidgetClass->IsChildOf(UW_Dialog::StaticClass());
			UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventSetupDialog - IsChildOf(UW_Dialog): %s"),
				bIsSubclass ? TEXT("YES - Cast should work") : TEXT("NO - NEED TO REPARENT BLUEPRINT!"));
		}

		CachedW_Dialog = Cast<UW_Dialog>(RawDialogWidget);
	}

	// Call W_Dialog's EventInitializeDialog to set text and show the widget
	if (CachedW_Dialog)
	{
		CachedW_Dialog->EventInitializeDialog(InText);
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventSetupDialog - Called EventInitializeDialog on W_Dialog"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::EventSetupDialog - CachedW_Dialog is null! Falling back to basic visibility"));
		// Fallback: Set text on the widget directly if possible
		if (UWidget* DialogWindow = GetWidgetFromName(TEXT("W_Dialog")))
		{
			DialogWindow->SetVisibility(ESlateVisibility::Visible);
			// Try to find dialog text widget inside and set it
			if (UUserWidget* DialogUserWidget = Cast<UUserWidget>(DialogWindow))
			{
				if (UTextBlock* DialogText = Cast<UTextBlock>(DialogUserWidget->GetWidgetFromName(TEXT("DialogText"))))
				{
					DialogText->SetText(InText);
					UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventSetupDialog - Fallback: Set DialogText directly to '%s'"), *InText.ToString());
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("UW_HUD::EventSetupDialog - Fallback: Could not find DialogText widget"));
				}
			}
		}
	}
}

void UW_HUD::EventFinishDialog_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventFinishDialog"));
	IsDialogActive = false;

	// Clear ActiveWidgetTag since dialog is ending
	if (ASLFPlayerController* PC = Cast<ASLFPlayerController>(GetOwningPlayer()))
	{
		PC->ActiveWidgetTag = FGameplayTag();
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventFinishDialog - Cleared ActiveWidgetTag"));
	}

	if (UWidget* DialogWindow = GetWidgetFromName(TEXT("W_Dialog")))
	{
		DialogWindow->SetVisibility(ESlateVisibility::Collapsed);
	}
	OnDialogWindowClosed.Broadcast();
}

void UW_HUD::EventSetupNpcWindow_Implementation(const FText& NpcName, UDataAsset* InVendorAsset, UAIInteractionManagerComponent* DialogComponent)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventSetupNpcWindow - NPC: %s, VendorAsset: %s, DialogComponent: %s"),
		*NpcName.ToString(),
		InVendorAsset ? *InVendorAsset->GetName() : TEXT("None"),
		DialogComponent ? *DialogComponent->GetName() : TEXT("None"));

	// NOTE: Do NOT call EventFinishDialog here - W_Dialog shows alongside W_NPC_Window
	// The dialog text is shown via EventSetupDialog, this function only shows the menu

	// Cache reference if not already cached
	if (!CachedW_NPC_Window)
	{
		CachedW_NPC_Window = Cast<UW_NPC_Window>(GetWidgetFromName(TEXT("W_NPC_Window")));
	}

	// Initialize and show the NPC window
	if (CachedW_NPC_Window)
	{
		CachedW_NPC_Window->EventInitializeNpcWindow(NpcName, InVendorAsset, DialogComponent);
		CachedW_NPC_Window->SetVisibility(ESlateVisibility::Visible);
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventSetupNpcWindow - Called EventInitializeNpcWindow on W_NPC_Window"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::EventSetupNpcWindow - CachedW_NPC_Window is null! Cannot initialize NPC window."));
		// NOTE: Do NOT call EventToggleUiMode(true) when menu creation fails!
		// UI-only mode blocks E key input. Without a working menu, we need game input
		// to remain active so player can press E to advance/close dialogue via
		// HandleInteractStarted → OnDialogWindowClosed.Broadcast()
		return;
	}

	// Toggle UI mode for dialog - ONLY if menu was successfully shown
	EventToggleUiMode(true);
}

void UW_HUD::EventFade_Implementation(ESLFFadeTypes FadeType, float PlayRate)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventFade - Type: %d, PlayRate: %f"), (int32)FadeType, PlayRate);
}

void UW_HUD::EventSwitchViewport_Implementation(int32 InIndex)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventSwitchViewport - Index: %d"), InIndex);
	if (UWidgetSwitcher* ViewportSwitcher = Cast<UWidgetSwitcher>(GetWidgetFromName(TEXT("ViewportSwitcher"))))
	{
		ViewportSwitcher->SetActiveWidgetIndex(InIndex);
	}
}

void UW_HUD::EventSendInteractionErrorMessage_Implementation(const FText& InMessage, float Time)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventSendInteractionErrorMessage - Message: %s, Time: %f"),
		*InMessage.ToString(), Time);
}

void UW_HUD::EventBigScreenMessage_Implementation(const FText& InMessage, UMaterialInterface* GradientMaterial, bool bHasBackdrop, float AnimationRateScale)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventBigScreenMessage - Message: %s, AnimationRate: %f"),
		*InMessage.ToString(), AnimationRateScale);

	// Find and show the W_BigScreenMessage widget
	UW_BigScreenMessage* BigScreenMsg = Cast<UW_BigScreenMessage>(GetWidgetFromName(TEXT("W_BigScreenMessage")));
	if (BigScreenMsg)
	{
		// Call EventShowMessage on the widget
		BigScreenMsg->EventShowMessage(InMessage, GradientMaterial, bHasBackdrop, AnimationRateScale);
		UE_LOG(LogTemp, Log, TEXT("[W_HUD] Forwarded to W_BigScreenMessage"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_HUD] W_BigScreenMessage widget not found!"));
	}
}

void UW_HUD::ShowDeathScreen()
{
	UE_LOG(LogTemp, Warning, TEXT("UW_HUD::ShowDeathScreen - Showing 'YOU DIED' screen"));

	// Hide boss health bar when player dies
	EventHideBossBar();

	// Clear status effect widgets on death
	ClearStatusEffectWidgets();

	// Call EventBigScreenMessage with "YOU DIED" text
	EventBigScreenMessage(FText::FromString(TEXT("YOU DIED")), nullptr, true, 1.0f);

	// Also display on-screen message as fallback
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("YOU DIED"));
	}
}

void UW_HUD::ClearStatusEffectWidgets()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::ClearStatusEffectWidgets - Clearing all status effect widgets"));

	if (!CachedStatusEffectBox)
	{
		UE_LOG(LogTemp, Warning, TEXT("  StatusEffectBox is null, cannot clear"));
		return;
	}

	// Clear all children from the status effect box
	int32 NumChildren = CachedStatusEffectBox->GetChildrenCount();
	CachedStatusEffectBox->ClearChildren();

	UE_LOG(LogTemp, Log, TEXT("  Cleared %d status effect widgets"), NumChildren);
}

void UW_HUD::HideDeathScreen()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::HideDeathScreen - Hiding death screen"));

	// Hide the big screen message widget (use correct widget name "W_BigScreenMessage")
	if (UWidget* BigScreenMessage = GetWidgetFromName(TEXT("W_BigScreenMessage")))
	{
		BigScreenMessage->SetVisibility(ESlateVisibility::Collapsed);
		UE_LOG(LogTemp, Log, TEXT("[W_HUD] Hid W_BigScreenMessage widget"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_HUD] HideDeathScreen - W_BigScreenMessage not found!"));
	}

	// Clear any on-screen debug messages
	if (GEngine)
	{
		GEngine->ClearOnScreenDebugMessages();
	}
}

void UW_HUD::EventShowAutoSaveIcon_Implementation(float InDuration)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowAutoSaveIcon - Duration: %f"), InDuration);
	if (UWidget* SaveIcon = GetWidgetFromName(TEXT("AutoSaveIcon")))
	{
		SaveIcon->SetVisibility(ESlateVisibility::Visible);
	}
}

void UW_HUD::EventRemoveAutoSaveIcon_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventRemoveAutoSaveIcon"));
	if (UWidget* SaveIcon = GetWidgetFromName(TEXT("AutoSaveIcon")))
	{
		SaveIcon->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UW_HUD::EventCreateLoadingScreen_Implementation(float AdditionalWaitDuration, bool bAutoRemove)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventCreateLoadingScreen - Duration: %f, AutoRemove: %s"),
		AdditionalWaitDuration, bAutoRemove ? TEXT("true") : TEXT("false"));
	if (LoadingScreen)
	{
		LoadingScreen->SetVisibility(ESlateVisibility::Visible);
	}
}

void UW_HUD::EventOnLoadingScreenFinished_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventOnLoadingScreenFinished"));
	if (LoadingScreen)
	{
		LoadingScreen->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UW_HUD::EventToggleCinematicMode_Implementation(bool bActive, bool bFadeOutIfNotCinematicMode)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventToggleCinematicMode - Active: %s, FadeOut: %s"),
		bActive ? TEXT("true") : TEXT("false"),
		bFadeOutIfNotCinematicMode ? TEXT("true") : TEXT("false"));
	CinematicMode = bActive;

	// bp_only: Use ViewportSwitcher to swap between MainHUD (index 0) and CinematicHUD (index 1)
	if (UWidgetSwitcher* ViewportSwitcher = Cast<UWidgetSwitcher>(GetWidgetFromName(TEXT("ViewportSwitcher"))))
	{
		ViewportSwitcher->SetActiveWidgetIndex(bActive ? 1 : 0);
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventToggleCinematicMode - ViewportSwitcher set to index %d"), bActive ? 1 : 0);
	}

	// CRITICAL: Hide WindowsCanvas during cinematic mode.
	// WindowsCanvas contains WindowsOL which holds RestMenu, Inventory, Equipment, etc.
	// These are siblings of ViewportSwitcher (not children), so switching ViewportSwitcher
	// index doesn't affect them. We must explicitly hide/show the entire windows layer.
	if (UWidget* WindowsCanvas = GetWidgetFromName(TEXT("WindowsCanvas")))
	{
		WindowsCanvas->SetVisibility(bActive ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventToggleCinematicMode - WindowsCanvas %s"),
			bActive ? TEXT("HIDDEN") : TEXT("VISIBLE"));
	}
}

void UW_HUD::EventShowSkipCinematicNotification_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowSkipCinematicNotification"));

	// bp_only: Play SkipCinematicFadeIn animation to show the skip notification
	// The W_SkipCinematic widget is inside CinematicHUD which is shown by ViewportSwitcher index 1
	UWidgetAnimation* FadeInAnim = nullptr;

	// Find SkipCinematicFadeIn animation by name (Blueprint UMG stores animations as UPROPERTY)
	FProperty* AnimProp = GetClass()->FindPropertyByName(TEXT("SkipCinematicFadeIn"));
	if (FObjectProperty* ObjProp = CastField<FObjectProperty>(AnimProp))
	{
		FadeInAnim = Cast<UWidgetAnimation>(ObjProp->GetObjectPropertyValue_InContainer(this));
	}

	if (FadeInAnim)
	{
		PlayAnimationForward(FadeInAnim);
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowSkipCinematicNotification - Playing SkipCinematicFadeIn animation"));
	}
	else
	{
		// Fallback: try to find the widget directly and set visibility
		if (UWidget* SkipWidget = GetWidgetFromName(TEXT("W_SkipCinematic")))
		{
			SkipWidget->SetVisibility(ESlateVisibility::Visible);
			UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowSkipCinematicNotification - Fallback: set W_SkipCinematic visible"));
		}
		else if (UWidget* SkipNotification = GetWidgetFromName(TEXT("SkipCinematicNotification")))
		{
			SkipNotification->SetVisibility(ESlateVisibility::Visible);
			UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowSkipCinematicNotification - Fallback: set SkipCinematicNotification visible"));
		}
	}
}

void UW_HUD::EventShowBossBar_Implementation(const FText& BossName, AActor* BossActor)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowBossBar - Boss: %s, Actor: %s"),
		*BossName.ToString(), BossActor ? *BossActor->GetName() : TEXT("null"));

	// Use cached reference first, fallback to GetWidgetFromName
	UW_Boss_Healthbar* BossHealthbar = CachedBossHealthbar;
	if (!BossHealthbar)
	{
		BossHealthbar = Cast<UW_Boss_Healthbar>(GetWidgetFromName(TEXT("W_Boss_Healthbar")));
	}

	if (BossHealthbar)
	{
		// Make visible first
		BossHealthbar->SetVisibility(ESlateVisibility::Visible);

		// Initialize the boss bar with name and actor reference for health binding
		BossHealthbar->EventInitializeBossBar(BossName, BossActor);

		UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowBossBar - Initialized boss bar with name: %s"), *BossName.ToString());
	}
	else
	{
		// Fallback - just set visibility if we find any widget named BossBar
		if (UWidget* BossBar = GetWidgetFromName(TEXT("BossBar")))
		{
			BossBar->SetVisibility(ESlateVisibility::Visible);
			UE_LOG(LogTemp, Warning, TEXT("UW_HUD::EventShowBossBar - Found BossBar but it's not W_Boss_Healthbar type"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UW_HUD::EventShowBossBar - No boss healthbar widget found!"));
		}
	}
}

void UW_HUD::EventHideBossBar_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventHideBossBar"));

	// Use cached reference first, fallback to GetWidgetFromName
	UW_Boss_Healthbar* BossHealthbar = CachedBossHealthbar;
	if (!BossHealthbar)
	{
		BossHealthbar = Cast<UW_Boss_Healthbar>(GetWidgetFromName(TEXT("W_Boss_Healthbar")));
	}

	if (BossHealthbar)
	{
		// Call EventHideBossBar on the widget itself - this unbinds from HP stat, clears timers, and hides
		BossHealthbar->EventHideBossBar();
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventHideBossBar - Called EventHideBossBar on boss healthbar widget"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::EventHideBossBar - No boss healthbar widget found!"));
	}
}

void UW_HUD::EventOnBossDestroyed_Implementation(AActor* DestroyedActor)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventOnBossDestroyed"));
	EventHideBossBar();
}

void UW_HUD::OnGameMenuWidgetRequestHandler(FGameplayTag WidgetTag)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnGameMenuWidgetRequestHandler - Tag: %s"), *WidgetTag.ToString());

	// Switch on gameplay tag to show the appropriate widget
	static const FGameplayTag InventoryTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Backend.Widgets.Inventory"));
	static const FGameplayTag EquipmentTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Backend.Widgets.Equipment"));
	static const FGameplayTag CraftingTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Backend.Widgets.Crafting"));
	static const FGameplayTag StatusTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Backend.Widgets.Status"));
	static const FGameplayTag SystemTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Backend.Widgets.System"));
	static const FGameplayTag WorldMapTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Backend.Widgets.WorldMap"));

	if (WidgetTag.MatchesTag(InventoryTag))
	{
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnGameMenuWidgetRequestHandler - Showing Inventory"));
		EventShowInventory();
	}
	else if (WidgetTag.MatchesTag(EquipmentTag))
	{
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnGameMenuWidgetRequestHandler - Showing Equipment"));
		EventShowEquipment();
	}
	else if (WidgetTag.MatchesTag(CraftingTag))
	{
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnGameMenuWidgetRequestHandler - Showing Crafting"));
		EventShowCrafting();
	}
	else if (WidgetTag.MatchesTag(StatusTag))
	{
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnGameMenuWidgetRequestHandler - Showing Status"));
		EventShowStatus();
	}
	else if (WidgetTag.MatchesTag(SystemTag))
	{
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnGameMenuWidgetRequestHandler - Showing Settings"));
		EventShowSettings();
	}
	else if (WidgetTag.MatchesTag(WorldMapTag))
	{
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnGameMenuWidgetRequestHandler - Showing WorldMap"));
		EventShowWorldMap();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::OnGameMenuWidgetRequestHandler - Unknown tag: %s"), *WidgetTag.ToString());
	}
}

void UW_HUD::OnInventoryClosedHandler()
{
	// Check if we were in storage mode (came from rest menu)
	bool bWasStorageMode = false;
	if (CachedW_Inventory)
	{
		bWasStorageMode = CachedW_Inventory->StorageMode;
		// Reset storage mode when closing
		if (bWasStorageMode)
		{
			CachedW_Inventory->SetStorageMode(false);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnInventoryClosedHandler - Closing Inventory, bWasStorageMode: %s"),
		bWasStorageMode ? TEXT("true") : TEXT("false"));

	EventCloseInventory();

	if (bWasStorageMode)
	{
		// Return to rest menu (viewport 1)
		EventSwitchViewport(1);
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnInventoryClosedHandler - Returning to RestMenu (viewport 1)"));

		// Set ActiveWidgetTag back to RestMenu
		APlayerController* OwningPlayer = GetOwningPlayer();
		if (APC_SoulslikeFramework* PC = Cast<APC_SoulslikeFramework>(OwningPlayer))
		{
			PC->ActiveWidgetTag = FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Backend.Widgets.RestMenu")));
		}
		else if (ASLFPlayerController* SLFController = Cast<ASLFPlayerController>(OwningPlayer))
		{
			SLFController->ActiveWidgetTag = FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Backend.Widgets.RestMenu")));
		}

		// CRITICAL: Call EventFadeInRestMenu to restore navigation and keyboard focus
		// bp_only Event OnExitStorage calls RestMenu.EventFadeInRestMenu() after switching viewport
		if (CachedW_RestMenu)
		{
			CachedW_RestMenu->EventFadeInRestMenu();
			UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnInventoryClosedHandler - Called RestMenu.EventFadeInRestMenu()"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UW_HUD::OnInventoryClosedHandler - CachedW_RestMenu is null, cannot restore navigation!"));
		}
	}
	else
	{
		// Normal flow - return to game menu
		EventShowGameMenu();
	}
}

void UW_HUD::OnEquipmentClosedHandler()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnEquipmentClosedHandler - Closing Equipment, showing GameMenu"));
	EventCloseEquipment();
	EventShowGameMenu();
}

void UW_HUD::OnCraftingClosedHandler()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnCraftingClosedHandler - Closing Crafting, showing GameMenu"));
	EventCloseCrafting();
	EventShowGameMenu();
}

void UW_HUD::OnStatusClosedHandler()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnStatusClosedHandler - Closing Status, showing GameMenu"));
	EventCloseStatus();
	EventShowGameMenu();
}

void UW_HUD::OnSettingsClosedHandler()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnSettingsClosedHandler - Closing Settings, showing GameMenu"));
	EventCloseSettings();
	EventShowGameMenu();
}

void UW_HUD::OnQuitRequestedHandler(bool Desktop)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnQuitRequestedHandler - Desktop: %s"), Desktop ? TEXT("true") : TEXT("false"));

	// Close settings UI first
	EventCloseSettings();
	EventCloseGameMenu();

	if (Desktop)
	{
		// Quit to desktop
		APlayerController* PC = GetOwningPlayer();
		if (PC)
		{
			UKismetSystemLibrary::QuitGame(GetWorld(), PC, EQuitPreference::Quit, false);
		}
	}
	else
	{
		// Quit to main menu - open the main menu level
		UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("L_Demo_Menu")));
	}
}

void UW_HUD::OnRestMenuClosedHandler()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnRestMenuClosedHandler - RestMenu closed, restoring gameplay input"));

	// Clear ActiveWidgetTag and switch input context back to gameplay
	// Try APC_SoulslikeFramework first, then ASLFPlayerController as fallback
	APlayerController* OwningPlayer = GetOwningPlayer();
	if (APC_SoulslikeFramework* PC = Cast<APC_SoulslikeFramework>(OwningPlayer))
	{
		// Clear active widget tag
		PC->ActiveWidgetTag = FGameplayTag();
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnRestMenuClosedHandler - Cleared ActiveWidgetTag (via APC_SoulslikeFramework)"));

		// Switch input context: Enable gameplay context, disable menu context
		TArray<UInputMappingContext*> ToEnable;
		TArray<UInputMappingContext*> ToDisable;

		if (PC->GameplayMappingContext)
		{
			ToEnable.Add(PC->GameplayMappingContext);
		}
		if (PC->NavigableMenuMappingContext)
		{
			ToDisable.Add(PC->NavigableMenuMappingContext);
		}

		IBPI_Controller::Execute_SwitchInputContext(PC, ToEnable, ToDisable);
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnRestMenuClosedHandler - Switched to gameplay input context"));
	}
	else if (ASLFPlayerController* SLFController = Cast<ASLFPlayerController>(OwningPlayer))
	{
		// Fallback to ASLFPlayerController
		SLFController->ActiveWidgetTag = FGameplayTag();
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnRestMenuClosedHandler - Cleared ActiveWidgetTag (via ASLFPlayerController)"));

		TArray<UInputMappingContext*> ToEnable;
		TArray<UInputMappingContext*> ToDisable;

		if (SLFController->IMC_Gameplay)
		{
			ToEnable.Add(SLFController->IMC_Gameplay);
		}
		if (SLFController->IMC_NavigableMenu)
		{
			ToDisable.Add(SLFController->IMC_NavigableMenu);
		}

		SLFController->SwitchInputContextInternal(ToEnable, ToDisable);
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnRestMenuClosedHandler - Switched to gameplay input context"));
	}
}

void UW_HUD::OnItemLootedHandler(UDataAsset* ItemAsset, int32 Amount)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnItemLootedHandler - Item: %s, Amount: %d"),
		ItemAsset ? *ItemAsset->GetName() : TEXT("null"), Amount);

	if (!ItemAsset)
	{
		return;
	}

	// Get item info from the data asset
	FSLFItemInfo ItemInfo;
	bool bExists = false;

	// Try to cast to PDA_Item to get the item information
	if (UPDA_Item* ItemData = Cast<UPDA_Item>(ItemAsset))
	{
		ItemInfo = ItemData->ItemInformation;

		// Check if item existed in inventory before this add (Amount > 1 means it existed)
		// Note: The inventory manager broadcasts AFTER adding, so if the new amount is
		// greater than the amount just added, it means the item already existed
		if (APlayerController* PC = GetOwningPlayer())
		{
			if (UInventoryManagerComponent* InvMgr = PC->FindComponentByClass<UInventoryManagerComponent>())
			{
				// Get the current amount from inventory
				int32 CurrentAmount = InvMgr->GetOverallCountForItem(ItemAsset);
				// If current amount > Amount we just picked up, item already existed
				bExists = (CurrentAmount > Amount);
			}
		}
	}

	// Call the main event handler with transformed parameters
	EventOnItemLooted(ItemInfo, Amount, bExists);
}

// ═══════════════════════════════════════════════════════════════════════════
// NAVIGATION ROUTING FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════

void UW_HUD::RouteNavigateCancel(const FGameplayTag& ActiveWidgetTag)
{
	FString TagString = ActiveWidgetTag.ToString();
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::RouteNavigateCancel - ActiveWidgetTag: %s"), *TagString);

	// Route based on active widget tag
	if (TagString.Contains(TEXT("GameMenu")))
	{
		if (CachedW_GameMenu) CachedW_GameMenu->EventNavigateCancel();
	}
	else if (TagString.Contains(TEXT("Inventory")))
	{
		if (CachedW_Inventory) CachedW_Inventory->EventNavigateCancel();
	}
	else if (TagString.Contains(TEXT("Equipment")))
	{
		if (CachedW_Equipment) CachedW_Equipment->EventNavigateCancel();
	}
	else if (TagString.Contains(TEXT("Crafting")))
	{
		if (CachedW_Crafting) CachedW_Crafting->EventNavigateCancel();
	}
	else if (TagString.Contains(TEXT("Status")))
	{
		if (CachedW_Status) CachedW_Status->EventNavigateCancel();
	}
	else if (TagString.Contains(TEXT("System")) || TagString.Contains(TEXT("Settings")))
	{
		if (CachedW_Settings) CachedW_Settings->EventNavigateCancel();
	}
	else if (TagString.Contains(TEXT("RestMenu")))
	{
		if (CachedW_RestMenu) CachedW_RestMenu->EventNavigateCancel();
	}
	else if (TagString.Contains(TEXT("NpcWindow")) || TagString.Contains(TEXT("NpcMenu")) || TagString.Contains(TEXT("Vendor")) || TagString.Contains(TEXT("Dialog")))
	{
		if (CachedW_NPC_Window) CachedW_NPC_Window->EventNavigateCancel();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::RouteNavigateCancel - Unknown widget tag: %s"), *TagString);
	}
}

void UW_HUD::RouteNavigateOk(const FGameplayTag& ActiveWidgetTag)
{
	FString TagString = ActiveWidgetTag.ToString();
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::RouteNavigateOk - ActiveWidgetTag: %s"), *TagString);

	// Note: W_Status only has EventNavigateCancel
	if (TagString.Contains(TEXT("GameMenu")))
	{
		if (CachedW_GameMenu) CachedW_GameMenu->EventNavigateOk();
	}
	else if (TagString.Contains(TEXT("Inventory")))
	{
		if (CachedW_Inventory) CachedW_Inventory->EventNavigateOk();
	}
	else if (TagString.Contains(TEXT("Equipment")))
	{
		if (CachedW_Equipment) CachedW_Equipment->EventNavigateOk();
	}
	else if (TagString.Contains(TEXT("Crafting")))
	{
		if (CachedW_Crafting) CachedW_Crafting->EventNavigateOk();
	}
	else if (TagString.Contains(TEXT("System")) || TagString.Contains(TEXT("Settings")))
	{
		if (CachedW_Settings) CachedW_Settings->EventNavigateOk();
	}
	else if (TagString.Contains(TEXT("RestMenu")))
	{
		if (CachedW_RestMenu) CachedW_RestMenu->EventNavigateOk();
	}
	else if (TagString.Contains(TEXT("NpcWindow")) || TagString.Contains(TEXT("NpcMenu")) || TagString.Contains(TEXT("Vendor")) || TagString.Contains(TEXT("Dialog")))
	{
		// Check if W_Dialog is visible (showing dialog text that needs advancing)
		UWidget* DialogWidget = GetWidgetFromName(TEXT("W_Dialog"));
		bool bDialogVisible = DialogWidget && DialogWidget->GetVisibility() == ESlateVisibility::Visible;

		if (bDialogVisible && IsDialogActive)
		{
			// Dialog text is showing - advance it by broadcasting OnDialogWindowClosed
			UE_LOG(LogTemp, Log, TEXT("UW_HUD::RouteNavigateOk - W_Dialog visible, advancing dialog via OnDialogWindowClosed"));
			OnDialogWindowClosed.Broadcast();
		}
		else if (CachedW_NPC_Window)
		{
			// NPC menu is showing - activate the selected button
			UE_LOG(LogTemp, Log, TEXT("UW_HUD::RouteNavigateOk - W_Dialog not visible, calling W_NPC_Window->EventNavigateOk"));
			CachedW_NPC_Window->EventNavigateOk();
		}
		else if (IsDialogActive)
		{
			// Fallback: dialog is active but neither widget found
			UE_LOG(LogTemp, Log, TEXT("UW_HUD::RouteNavigateOk - Fallback: dialog active, broadcasting OnDialogWindowClosed"));
			OnDialogWindowClosed.Broadcast();
		}
	}
	else if (IsDialogActive)
	{
		// Fallback: If dialog is active but tag didn't match (e.g., tag not registered),
		// still broadcast OnDialogWindowClosed to advance the dialog
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::RouteNavigateOk - Dialog active (fallback), broadcasting OnDialogWindowClosed"));
		OnDialogWindowClosed.Broadcast();
	}
}

void UW_HUD::RouteNavigateUp(const FGameplayTag& ActiveWidgetTag)
{
	FString TagString = ActiveWidgetTag.ToString();
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::RouteNavigateUp - ActiveWidgetTag: %s"), *TagString);

	// Note: W_Status only has EventNavigateCancel, no Up/Down navigation
	if (TagString.Contains(TEXT("GameMenu")))
	{
		if (CachedW_GameMenu) CachedW_GameMenu->EventNavigateUp();
	}
	else if (TagString.Contains(TEXT("Inventory")))
	{
		if (CachedW_Inventory) CachedW_Inventory->EventNavigateUp();
	}
	else if (TagString.Contains(TEXT("Equipment")))
	{
		if (CachedW_Equipment) CachedW_Equipment->EventNavigateUp();
	}
	else if (TagString.Contains(TEXT("Crafting")))
	{
		if (CachedW_Crafting) CachedW_Crafting->EventNavigateUp();
	}
	else if (TagString.Contains(TEXT("System")) || TagString.Contains(TEXT("Settings")))
	{
		if (CachedW_Settings) CachedW_Settings->EventNavigateUp();
	}
	else if (TagString.Contains(TEXT("RestMenu")))
	{
		if (CachedW_RestMenu) CachedW_RestMenu->EventNavigateUp();
	}
	else if (TagString.Contains(TEXT("NpcWindow")) || TagString.Contains(TEXT("NpcMenu")) || TagString.Contains(TEXT("Vendor")) || TagString.Contains(TEXT("Dialog")))
	{
		if (CachedW_NPC_Window) CachedW_NPC_Window->EventNavigateUp();
	}
}

void UW_HUD::RouteNavigateDown(const FGameplayTag& ActiveWidgetTag)
{
	FString TagString = ActiveWidgetTag.ToString();
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::RouteNavigateDown - ActiveWidgetTag: %s"), *TagString);

	// Note: W_Status only has EventNavigateCancel, no Up/Down navigation
	if (TagString.Contains(TEXT("GameMenu")))
	{
		if (CachedW_GameMenu) CachedW_GameMenu->EventNavigateDown();
	}
	else if (TagString.Contains(TEXT("Inventory")))
	{
		if (CachedW_Inventory) CachedW_Inventory->EventNavigateDown();
	}
	else if (TagString.Contains(TEXT("Equipment")))
	{
		if (CachedW_Equipment) CachedW_Equipment->EventNavigateDown();
	}
	else if (TagString.Contains(TEXT("Crafting")))
	{
		if (CachedW_Crafting) CachedW_Crafting->EventNavigateDown();
	}
	else if (TagString.Contains(TEXT("System")) || TagString.Contains(TEXT("Settings")))
	{
		if (CachedW_Settings) CachedW_Settings->EventNavigateDown();
	}
	else if (TagString.Contains(TEXT("RestMenu")))
	{
		if (CachedW_RestMenu) CachedW_RestMenu->EventNavigateDown();
	}
	else if (TagString.Contains(TEXT("NpcWindow")) || TagString.Contains(TEXT("NpcMenu")) || TagString.Contains(TEXT("Vendor")) || TagString.Contains(TEXT("Dialog")))
	{
		if (CachedW_NPC_Window) CachedW_NPC_Window->EventNavigateDown();
	}
}

void UW_HUD::RouteNavigateLeft(const FGameplayTag& ActiveWidgetTag)
{
	FString TagString = ActiveWidgetTag.ToString();
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::RouteNavigateLeft - ActiveWidgetTag: %s"), *TagString);

	// Note: W_GameMenu and W_Status don't have Left navigation
	if (TagString.Contains(TEXT("Inventory")))
	{
		if (CachedW_Inventory) CachedW_Inventory->EventNavigateLeft();
	}
	else if (TagString.Contains(TEXT("Equipment")))
	{
		if (CachedW_Equipment) CachedW_Equipment->EventNavigateLeft();
	}
	else if (TagString.Contains(TEXT("Crafting")))
	{
		if (CachedW_Crafting) CachedW_Crafting->EventNavigateLeft();
	}
	else if (TagString.Contains(TEXT("System")) || TagString.Contains(TEXT("Settings")))
	{
		if (CachedW_Settings) CachedW_Settings->EventNavigateLeft();
	}
	// Note: W_NPC_Window only has Up/Down/Ok/Cancel - no Left/Right navigation
}

void UW_HUD::RouteNavigateRight(const FGameplayTag& ActiveWidgetTag)
{
	FString TagString = ActiveWidgetTag.ToString();
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::RouteNavigateRight - ActiveWidgetTag: %s"), *TagString);

	// Note: W_GameMenu and W_Status don't have Right navigation
	if (TagString.Contains(TEXT("Inventory")))
	{
		if (CachedW_Inventory) CachedW_Inventory->EventNavigateRight();
	}
	else if (TagString.Contains(TEXT("Equipment")))
	{
		if (CachedW_Equipment) CachedW_Equipment->EventNavigateRight();
	}
	else if (TagString.Contains(TEXT("Crafting")))
	{
		if (CachedW_Crafting) CachedW_Crafting->EventNavigateRight();
	}
	else if (TagString.Contains(TEXT("System")) || TagString.Contains(TEXT("Settings")))
	{
		if (CachedW_Settings) CachedW_Settings->EventNavigateRight();
	}
	// Note: W_NPC_Window only has Up/Down/Ok/Cancel - no Left/Right navigation
}

void UW_HUD::RouteNavigateCategoryLeft(const FGameplayTag& ActiveWidgetTag)
{
	FString TagString = ActiveWidgetTag.ToString();
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::RouteNavigateCategoryLeft - ActiveWidgetTag: %s"), *TagString);

	// Only Inventory and Settings have category navigation
	if (TagString.Contains(TEXT("Inventory")))
	{
		if (CachedW_Inventory) CachedW_Inventory->EventNavigateCategoryLeft();
	}
	else if (TagString.Contains(TEXT("System")) || TagString.Contains(TEXT("Settings")))
	{
		if (CachedW_Settings) CachedW_Settings->EventNavigateCategoryLeft();
	}
}

void UW_HUD::RouteNavigateCategoryRight(const FGameplayTag& ActiveWidgetTag)
{
	FString TagString = ActiveWidgetTag.ToString();
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::RouteNavigateCategoryRight - ActiveWidgetTag: %s"), *TagString);

	// Only Inventory and Settings have category navigation
	if (TagString.Contains(TEXT("Inventory")))
	{
		if (CachedW_Inventory) CachedW_Inventory->EventNavigateCategoryRight();
	}
	else if (TagString.Contains(TEXT("System")) || TagString.Contains(TEXT("Settings")))
	{
		if (CachedW_Settings) CachedW_Settings->EventNavigateCategoryRight();
	}
}

void UW_HUD::RouteNavigateUnequip(const FGameplayTag& ActiveWidgetTag)
{
	FString TagString = ActiveWidgetTag.ToString();
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::RouteNavigateUnequip - ActiveWidgetTag: %s"), *TagString);

	// Only Equipment has unequip navigation
	if (TagString.Contains(TEXT("Equipment")))
	{
		if (CachedW_Equipment) CachedW_Equipment->EventNavigateUnequip();
	}
}

void UW_HUD::RouteNavigateDetailedView(const FGameplayTag& ActiveWidgetTag)
{
	FString TagString = ActiveWidgetTag.ToString();
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::RouteNavigateDetailedView - ActiveWidgetTag: %s"), *TagString);

	// Inventory and Equipment have detailed view
	if (TagString.Contains(TEXT("Inventory")))
	{
		if (CachedW_Inventory) CachedW_Inventory->EventNavigateDetailedView();
	}
	else if (TagString.Contains(TEXT("Equipment")))
	{
		if (CachedW_Equipment) CachedW_Equipment->EventNavigateDetailedView();
	}
}

void UW_HUD::RouteNavigateResetToDefaults(const FGameplayTag& ActiveWidgetTag)
{
	FString TagString = ActiveWidgetTag.ToString();
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::RouteNavigateResetToDefaults - ActiveWidgetTag: %s"), *TagString);

	// Only Settings has reset to defaults (note: function is EventNavigateResetToDefault, singular)
	if (TagString.Contains(TEXT("System")) || TagString.Contains(TEXT("Settings")))
	{
		if (CachedW_Settings) CachedW_Settings->EventNavigateResetToDefault();
	}
}

void UW_HUD::OnStorageRequestedHandler()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnStorageRequestedHandler - Opening Inventory in Storage Mode"));

	// Switch viewport to show main HUD layer (index 0) where inventory lives
	// Rest menu was on index 1 (WindowsOL layer)
	EventSwitchViewport(0);
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnStorageRequestedHandler - Switched viewport to index 0"));

	// Show the inventory widget
	EventShowInventory();

	// Set inventory to storage mode
	if (CachedW_Inventory)
	{
		CachedW_Inventory->SetStorageMode(true);
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnStorageRequestedHandler - Set StorageMode to true"));
	}
	else
	{
		// Try to find and cache it if not already cached
		CachedW_Inventory = Cast<UW_Inventory>(GetWidgetFromName(TEXT("W_Inventory")));
		if (CachedW_Inventory)
		{
			CachedW_Inventory->SetStorageMode(true);
			UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnStorageRequestedHandler - Found and set StorageMode to true"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UW_HUD::OnStorageRequestedHandler - CachedW_Inventory not found!"));
		}
	}

	// Set ActiveWidgetTag for navigation routing
	APlayerController* OwningPlayer = GetOwningPlayer();
	if (APC_SoulslikeFramework* PC = Cast<APC_SoulslikeFramework>(OwningPlayer))
	{
		PC->ActiveWidgetTag = FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Backend.Widgets.Inventory")));
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnStorageRequestedHandler - Set ActiveWidgetTag to Inventory"));
	}
	else if (ASLFPlayerController* SLFController = Cast<ASLFPlayerController>(OwningPlayer))
	{
		SLFController->ActiveWidgetTag = FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Backend.Widgets.Inventory")));
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnStorageRequestedHandler - Set ActiveWidgetTag to Inventory (via ASLFPlayerController)"));
	}
}

void UW_HUD::OnStatusEffectAddedFromComponent(UObject* StatusEffect)
{
	// StatusEffectManagerComponent uses UObject*, cast to UB_StatusEffect and forward
	UB_StatusEffect* Effect = Cast<UB_StatusEffect>(StatusEffect);
	if (Effect)
	{
		EventOnStatusEffectAdded(Effect);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::OnStatusEffectAddedFromComponent - Failed to cast StatusEffect to UB_StatusEffect"));
	}
}

void UW_HUD::OnItemWheelToolsSlotSelected(FGameplayTag SlotTag)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnItemWheelToolsSlotSelected - SlotTag: %s"), *SlotTag.ToString());

	// Get the equipment manager and set the active tool slot
	APlayerController* OwningPlayer = GetOwningPlayer();
	if (!OwningPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::OnItemWheelToolsSlotSelected - No OwningPlayer"));
		return;
	}

	APawn* ControlledPawn = OwningPlayer->GetPawn();
	if (!ControlledPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::OnItemWheelToolsSlotSelected - No ControlledPawn"));
		return;
	}

	// Try to find equipment manager component on the pawn
	UAC_EquipmentManager* EquipMgr = ControlledPawn->FindComponentByClass<UAC_EquipmentManager>();
	if (EquipMgr)
	{
		EquipMgr->SetActiveToolSlot(SlotTag);
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnItemWheelToolsSlotSelected - Set ActiveToolSlot on UAC_EquipmentManager"));
		return;
	}

	// Also try UEquipmentManagerComponent
	UEquipmentManagerComponent* EquipMgrComp = ControlledPawn->FindComponentByClass<UEquipmentManagerComponent>();
	if (EquipMgrComp)
	{
		EquipMgrComp->SetActiveToolSlot(SlotTag);
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnItemWheelToolsSlotSelected - Set ActiveToolSlot on UEquipmentManagerComponent"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("UW_HUD::OnItemWheelToolsSlotSelected - No equipment manager found on pawn"));
}
