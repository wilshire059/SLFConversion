// W_NPC_Window.cpp
// C++ Widget implementation for W_NPC_Window
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session

#include "Widgets/W_NPC_Window.h"
#include "Widgets/W_RestMenu_Button.h"
#include "Widgets/W_NPC_Window_Vendor.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/AIInteractionManagerComponent.h"
#include "Interfaces/BPI_Controller.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/SLFPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/WidgetTree.h"

UW_NPC_Window::UW_NPC_Window(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UW_NPC_Window::NativeConstruct()
{
	Super::NativeConstruct();

	// Make widget focusable - CRITICAL for NativeOnKeyDown to work
	SetIsFocusable(true);

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window::NativeConstruct"));
}

void UW_NPC_Window::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window::NativeDestruct"));
}

FReply UW_NPC_Window::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FKey Key = InKeyEvent.GetKey();

	UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] NativeOnKeyDown - Key: %s"), *Key.ToString());

	// Navigate Up
	if (Key == EKeys::W || Key == EKeys::Up || Key == EKeys::Gamepad_DPad_Up || Key == EKeys::Gamepad_LeftStick_Up)
	{
		EventNavigateUp();
		return FReply::Handled();
	}

	// Navigate Down
	if (Key == EKeys::S || Key == EKeys::Down || Key == EKeys::Gamepad_DPad_Down || Key == EKeys::Gamepad_LeftStick_Down)
	{
		EventNavigateDown();
		return FReply::Handled();
	}

	// Confirm/OK
	if (Key == EKeys::Enter || Key == EKeys::SpaceBar || Key == EKeys::Gamepad_FaceButton_Bottom)
	{
		EventNavigateOk();
		return FReply::Handled();
	}

	// Cancel/Back
	if (Key == EKeys::Escape || Key == EKeys::Gamepad_FaceButton_Right || Key == EKeys::Gamepad_Special_Right)
	{
		EventNavigateCancel();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UW_NPC_Window::CacheWidgetReferences()
{
	// Find the MainButtonsBox and populate NpcMenuButtons array
	// The buttons (W_RMB_Sell, W_RMB_Talk, W_RMB_Leave) are pre-placed in the widget hierarchy

	NpcMenuButtons.Empty();

	// Look for MainButtonsBox - the container for NPC menu buttons
	if (UVerticalBox* MainButtonsBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("MainButtonsBox"))))
	{
		UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Found MainButtonsBox with %d children"), MainButtonsBox->GetChildrenCount());

		// Iterate through all children and add W_RestMenu_Button widgets to our array
		for (int32 i = 0; i < MainButtonsBox->GetChildrenCount(); i++)
		{
			UWidget* Child = MainButtonsBox->GetChildAt(i);
			if (UW_RestMenu_Button* Button = Cast<UW_RestMenu_Button>(Child))
			{
				NpcMenuButtons.Add(Button);
				UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Added button %d: %s"), i, *Button->GetName());

				// Bind button press handlers based on button name
				BindButtonHandler(Button);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_NPC_Window] MainButtonsBox not found!"));

		// Fallback: try to find buttons by name
		const TArray<FName> ButtonNames = {
			TEXT("W_RMB_Talk"),
			TEXT("W_RMB_Sell"),
			TEXT("W_RMB_Leave")
		};

		for (const FName& ButtonName : ButtonNames)
		{
			if (UW_RestMenu_Button* Button = Cast<UW_RestMenu_Button>(GetWidgetFromName(ButtonName)))
			{
				NpcMenuButtons.Add(Button);
				UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Found button by name: %s"), *ButtonName.ToString());
				BindButtonHandler(Button);
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] CacheWidgetReferences - Found %d buttons"), NpcMenuButtons.Num());

	// Cache reference to W_Vendor child widget (W_NPC_Window_Vendor)
	// This widget handles the vendor item grid for Buy/Sell functionality
	// NOTE: The Blueprint may not be reparented to C++ class, so we search by name first

	// First try to find by name in widget tree - look for any widget named "W_Vendor"
	if (WidgetTree)
	{
		WidgetTree->ForEachWidget([this](UWidget* Widget)
		{
			if (!CachedW_Vendor && Widget)
			{
				FString WidgetName = Widget->GetName();
				// Check for exact match or Blueprint instance name (W_Vendor_C_0, etc.)
				if (WidgetName.Equals(TEXT("W_Vendor")) || WidgetName.StartsWith(TEXT("W_Vendor_")))
				{
					// Try cast to our C++ class first
					if (UW_NPC_Window_Vendor* VendorWidget = Cast<UW_NPC_Window_Vendor>(Widget))
					{
						CachedW_Vendor = VendorWidget;
						UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Found W_Vendor (C++ class) via WidgetTree: %s"), *WidgetName);
					}
					else if (UUserWidget* UserWidget = Cast<UUserWidget>(Widget))
					{
						// Blueprint not reparented - check if it's a vendor widget by class name
						FString ClassName = Widget->GetClass()->GetName();
						if (ClassName.Contains(TEXT("NPC_Window_Vendor")))
						{
							// Store as base UUserWidget and we'll call functions via BlueprintCallable
							UE_LOG(LogTemp, Warning, TEXT("[W_NPC_Window] Found W_Vendor as Blueprint (not reparented): %s, Class: %s"),
								*WidgetName, *ClassName);
							// We can't use CachedW_Vendor directly, but we found the widget
							// Store a generic reference for later use
							CachedW_VendorGeneric = UserWidget;
						}
					}
				}
			}
		});
	}

	// Also try direct GetWidgetFromName
	if (!CachedW_Vendor && !CachedW_VendorGeneric)
	{
		if (UWidget* FoundWidget = GetWidgetFromName(TEXT("W_Vendor")))
		{
			CachedW_Vendor = Cast<UW_NPC_Window_Vendor>(FoundWidget);
			if (!CachedW_Vendor)
			{
				CachedW_VendorGeneric = Cast<UUserWidget>(FoundWidget);
				if (CachedW_VendorGeneric)
				{
					UE_LOG(LogTemp, Warning, TEXT("[W_NPC_Window] Found W_Vendor via GetWidgetFromName (generic): %s"),
						*FoundWidget->GetClass()->GetName());
				}
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Found W_Vendor via GetWidgetFromName (C++ class)"));
			}
		}
	}

	if (CachedW_Vendor)
	{
		UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Found W_Vendor child widget (C++ class): %s"), *CachedW_Vendor->GetName());

		// Bind to vendor window closed event - restores navigation to NPC menu when vendor closes
		CachedW_Vendor->OnVendorWindowClosed.AddDynamic(this, &UW_NPC_Window::HandleVendorWindowClosed);
		UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Bound to W_Vendor.OnVendorWindowClosed"));
	}
	else if (CachedW_VendorGeneric)
	{
		UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Found W_Vendor as Blueprint (generic): %s - will use UFunction calls"),
			*CachedW_VendorGeneric->GetClass()->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_NPC_Window] W_Vendor child widget not found! Vendor functionality will not work."));
	}

	// Initialize navigation index
	NavigationIndex = 0;

	// Select first button if available
	if (NpcMenuButtons.Num() > 0 && NpcMenuButtons[0])
	{
		EventOnButtonSelected(NpcMenuButtons[0]);
	}
}

void UW_NPC_Window::BindButtonHandler(UW_RestMenu_Button* Button)
{
	if (!Button)
	{
		return;
	}

	FString ButtonName = Button->GetName();
	UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Binding handler for button: %s"), *ButtonName);

	// Bind based on button name
	// W_RMB_Leave -> Close menu
	// W_RMB_Talk -> Initiate talk dialog
	// W_RMB_Sell -> Open vendor window (if vendor available)
	// W_RMB_Store -> Open storage

	if (ButtonName.Contains(TEXT("Leave")))
	{
		Button->OnRestMenuButtonPressed.AddDynamic(this, &UW_NPC_Window::HandleLeavePressed);
	}
	else if (ButtonName.Contains(TEXT("Talk")))
	{
		Button->OnRestMenuButtonPressed.AddDynamic(this, &UW_NPC_Window::HandleTalkPressed);
	}
	else if (ButtonName.Contains(TEXT("Sell")))
	{
		Button->OnRestMenuButtonPressed.AddDynamic(this, &UW_NPC_Window::HandleSellPressed);
	}
	else if (ButtonName.Contains(TEXT("Buy")))
	{
		Button->OnRestMenuButtonPressed.AddDynamic(this, &UW_NPC_Window::HandleBuyPressed);
	}
	else if (ButtonName.Contains(TEXT("Store")))
	{
		Button->OnRestMenuButtonPressed.AddDynamic(this, &UW_NPC_Window::HandleStorePressed);
	}
}

void UW_NPC_Window::HandleLeavePressed()
{
	UE_LOG(LogTemp, Warning, TEXT("[W_NPC_Window] *** HandleLeavePressed CALLED *** - closing NPC menu"));
	EventCloseNpcMenu();
}

void UW_NPC_Window::HandleTalkPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("[W_NPC_Window] *** HandleTalkPressed CALLED *** - initiating dialog"));

	// Broadcast the talk initiated event
	OnNpcWindowTalkInitiated.Broadcast();

	// The dialog system would advance to the talk content here
	// For now, just log and let higher-level systems handle it
}

void UW_NPC_Window::HandleSellPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("[W_NPC_Window] *** HandleSellPressed CALLED *** - opening vendor window"));

	// If we have a vendor asset, open the vendor window
	if (VendorAsset)
	{
		UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] VendorAsset available: %s (Class: %s)"),
			*VendorAsset->GetName(), *VendorAsset->GetClass()->GetName());

		// Try C++ typed reference first - pass VendorAsset directly (accepts UDataAsset*)
		if (CachedW_Vendor)
		{
			// ESLFVendorType::Sell = 1
			CachedW_Vendor->EventInitializeVendor(NpcName, VendorAsset, static_cast<uint8>(ESLFVendorType::Sell));
			CachedW_Vendor->SetVisibility(ESlateVisibility::Visible);
			CachedW_Vendor->SetKeyboardFocus();
			UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Called EventInitializeVendor on W_Vendor (C++) with Sell type"));
		}
		// Fallback to generic Blueprint reference - call the Blueprint function via ProcessEvent
		else if (CachedW_VendorGeneric)
		{
			// Try to find and call the Blueprint's "Event InitializeVendor" function
			UFunction* InitFunc = CachedW_VendorGeneric->FindFunction(FName("Event InitializeVendor"));
			if (!InitFunc)
			{
				InitFunc = CachedW_VendorGeneric->FindFunction(FName("EventInitializeVendor"));
			}

			if (InitFunc)
			{
				UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Found Blueprint function: %s"), *InitFunc->GetName());

				// Build parameters struct matching the function signature:
				// EventInitializeVendor(FText NpcName, UDataAsset* InVendorAsset, uint8 InVendorType)
				struct FInitVendorParams
				{
					FText NpcName;
					UDataAsset* InVendorAsset;
					uint8 InVendorType;
				};

				FInitVendorParams Params;
				Params.NpcName = NpcName;
				Params.InVendorAsset = VendorAsset;
				Params.InVendorType = static_cast<uint8>(ESLFVendorType::Sell);

				CachedW_VendorGeneric->ProcessEvent(InitFunc, &Params);
				UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Called Blueprint EventInitializeVendor via ProcessEvent (Sell mode)"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[W_NPC_Window] Blueprint function 'Event InitializeVendor' not found - falling back to property set"));

				// Fallback: Set properties directly
				UClass* VendorClass = CachedW_VendorGeneric->GetClass();

				// Set CurrentNpcName
				if (FProperty* NameProp = VendorClass->FindPropertyByName(TEXT("CurrentNpcName")))
				{
					if (FTextProperty* TextProp = CastField<FTextProperty>(NameProp))
					{
						TextProp->SetPropertyValue_InContainer(CachedW_VendorGeneric, NpcName);
					}
				}

				// Set CurrentVendorAsset
				if (FProperty* AssetProp = VendorClass->FindPropertyByName(TEXT("CurrentVendorAsset")))
				{
					if (FObjectProperty* ObjProp = CastField<FObjectProperty>(AssetProp))
					{
						ObjProp->SetObjectPropertyValue_InContainer(CachedW_VendorGeneric, VendorAsset);
					}
				}

				// Set VendorType = Sell (1)
				if (FProperty* TypeProp = VendorClass->FindPropertyByName(TEXT("VendorType")))
				{
					if (FByteProperty* ByteProp = CastField<FByteProperty>(TypeProp))
					{
						ByteProp->SetPropertyValue_InContainer(CachedW_VendorGeneric, static_cast<uint8>(ESLFVendorType::Sell));
					}
					else if (FEnumProperty* EnumProp = CastField<FEnumProperty>(TypeProp))
					{
						void* ValuePtr = EnumProp->ContainerPtrToValuePtr<void>(CachedW_VendorGeneric);
						EnumProp->GetUnderlyingProperty()->SetIntPropertyValue(ValuePtr, static_cast<int64>(ESLFVendorType::Sell));
					}
				}
			}

			// Show and focus the vendor widget
			CachedW_VendorGeneric->SetVisibility(ESlateVisibility::Visible);
			CachedW_VendorGeneric->SetKeyboardFocus();
			UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Vendor widget shown and focused (Sell mode)"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_NPC_Window] No vendor widget available!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_NPC_Window] Sell pressed but no VendorAsset available!"));
	}
}

void UW_NPC_Window::HandleStorePressed()
{
	UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Store button pressed"));
	// Storage functionality would be handled here
}

void UW_NPC_Window::HandleBuyPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("[W_NPC_Window] *** HandleBuyPressed CALLED *** - opening vendor buy window"));

	// If we have a vendor asset, open the buy interface
	if (VendorAsset)
	{
		UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] VendorAsset available: %s (Class: %s)"),
			*VendorAsset->GetName(), *VendorAsset->GetClass()->GetName());

		// Try C++ typed reference first - pass VendorAsset directly (accepts UDataAsset*)
		if (CachedW_Vendor)
		{
			// ESLFVendorType::Buy = 0
			CachedW_Vendor->EventInitializeVendor(NpcName, VendorAsset, static_cast<uint8>(ESLFVendorType::Buy));
			CachedW_Vendor->SetVisibility(ESlateVisibility::Visible);
			CachedW_Vendor->SetKeyboardFocus();
			UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Called EventInitializeVendor on W_Vendor (C++) with Buy type"));
		}
		// Fallback to generic Blueprint reference - call the Blueprint function via ProcessEvent
		else if (CachedW_VendorGeneric)
		{
			// Try to find and call the Blueprint's "Event InitializeVendor" function
			UFunction* InitFunc = CachedW_VendorGeneric->FindFunction(FName("Event InitializeVendor"));
			if (!InitFunc)
			{
				InitFunc = CachedW_VendorGeneric->FindFunction(FName("EventInitializeVendor"));
			}

			if (InitFunc)
			{
				UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Found Blueprint function: %s"), *InitFunc->GetName());

				// Build parameters struct matching the function signature
				struct FInitVendorParams
				{
					FText NpcName;
					UDataAsset* InVendorAsset;
					uint8 InVendorType;
				};

				FInitVendorParams Params;
				Params.NpcName = NpcName;
				Params.InVendorAsset = VendorAsset;
				Params.InVendorType = static_cast<uint8>(ESLFVendorType::Buy);

				CachedW_VendorGeneric->ProcessEvent(InitFunc, &Params);
				UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Called Blueprint EventInitializeVendor via ProcessEvent (Buy mode)"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[W_NPC_Window] Blueprint function 'Event InitializeVendor' not found - falling back to property set"));

				// Fallback: Set properties directly
				UClass* VendorClass = CachedW_VendorGeneric->GetClass();

				// Set CurrentNpcName
				if (FProperty* NameProp = VendorClass->FindPropertyByName(TEXT("CurrentNpcName")))
				{
					if (FTextProperty* TextProp = CastField<FTextProperty>(NameProp))
					{
						TextProp->SetPropertyValue_InContainer(CachedW_VendorGeneric, NpcName);
					}
				}

				// Set CurrentVendorAsset
				if (FProperty* AssetProp = VendorClass->FindPropertyByName(TEXT("CurrentVendorAsset")))
				{
					if (FObjectProperty* ObjProp = CastField<FObjectProperty>(AssetProp))
					{
						ObjProp->SetObjectPropertyValue_InContainer(CachedW_VendorGeneric, VendorAsset);
					}
				}

				// Set VendorType = Buy (0)
				if (FProperty* TypeProp = VendorClass->FindPropertyByName(TEXT("VendorType")))
				{
					if (FByteProperty* ByteProp = CastField<FByteProperty>(TypeProp))
					{
						ByteProp->SetPropertyValue_InContainer(CachedW_VendorGeneric, static_cast<uint8>(ESLFVendorType::Buy));
					}
					else if (FEnumProperty* EnumProp = CastField<FEnumProperty>(TypeProp))
					{
						void* ValuePtr = EnumProp->ContainerPtrToValuePtr<void>(CachedW_VendorGeneric);
						EnumProp->GetUnderlyingProperty()->SetIntPropertyValue(ValuePtr, static_cast<int64>(ESLFVendorType::Buy));
					}
				}
			}

			// Show and focus the vendor widget
			CachedW_VendorGeneric->SetVisibility(ESlateVisibility::Visible);
			CachedW_VendorGeneric->SetKeyboardFocus();
			UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Vendor widget shown and focused (Buy mode)"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_NPC_Window] No vendor widget available!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_NPC_Window] Buy pressed but no VendorAsset available!"));
	}
}

void UW_NPC_Window::HandleVendorWindowClosed()
{
	UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] HandleVendorWindowClosed - restoring navigation to NPC menu"));

	// Hide the vendor widget
	if (CachedW_Vendor)
	{
		CachedW_Vendor->SetVisibility(ESlateVisibility::Collapsed);
	}

	// Restore active navigable widget back to this NPC window
	APlayerController* PC = GetOwningPlayer();
	if (PC && PC->Implements<UBPI_Controller>())
	{
		IBPI_Controller::Execute_SetActiveWidgetForNavigation(PC, NavigableTag);
		UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Restored active widget to NPC_Window.NavigableTag: %s"),
			*NavigableTag.ToString());
	}

	// Reset navigation to first button
	NavigationIndex = 0;
	if (NpcMenuButtons.Num() > 0 && NpcMenuButtons[0])
	{
		EventOnButtonSelected(NpcMenuButtons[0]);
	}

	// CRITICAL: Restore keyboard focus to this widget so NativeOnKeyDown receives input
	SetKeyboardFocus();
	UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Restored keyboard focus to NPC window"));
}

void UW_NPC_Window::EventCloseNpcMenu_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window::EventCloseNpcMenu_Implementation - Closing NPC window"));

	// Get the player controller to call interface functions
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		// Restore input mode to GameOnly (was GameAndUI for menu navigation)
		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;
		UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Restored input mode to GameOnly"));

		// Set active widget to none (clear navigation target)
		if (PC->GetClass()->ImplementsInterface(UBPI_Controller::StaticClass()))
		{
			FGameplayTag EmptyTag;
			IBPI_Controller::Execute_SetActiveWidgetForNavigation(PC, EmptyTag);
			UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Called SetActiveWidgetForNavigation with empty tag"));
		}

		// Switch input context DIRECTLY (Blueprint's Event SwitchInputContext was cleared during migration)
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				// Remove navigable menu context
				if (NavigableInputMapping)
				{
					InputSubsystem->RemoveMappingContext(NavigableInputMapping);
					UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] DIRECT: Removed NavigableInputMapping: %s"), *NavigableInputMapping->GetName());
				}

				// Re-enable the gameplay context
				UInputMappingContext* GameplayContext = CachedGameplayContext;
				if (!GameplayContext)
				{
					// Fallback: load directly
					GameplayContext = LoadObject<UInputMappingContext>(nullptr,
						TEXT("/Game/SoulslikeFramework/Input/IMC_Gameplay.IMC_Gameplay"));
				}

				if (GameplayContext)
				{
					InputSubsystem->AddMappingContext(GameplayContext, 0);
					UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] DIRECT: Added GameplayMappingContext: %s"), *GameplayContext->GetName());
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("[W_NPC_Window] Failed to get GameplayMappingContext!"));
				}

				UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] DIRECT: Restored gameplay input context"));
			}
		}

		// Re-enable input
		PC->EnableInput(PC);
		UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Re-enabled player input"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_NPC_Window] No PlayerController from GetOwningPlayer!"));
	}

	// Hide this widget
	SetVisibility(ESlateVisibility::Collapsed);
	UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Set visibility to Collapsed"));

	// Broadcast the closed event
	OnNpcWindowClosed.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Broadcast OnNpcWindowClosed"));

	// Clear references
	ActiveDialogComponent = nullptr;
	VendorAsset = nullptr;
	CachedGameplayContext = nullptr;
}


void UW_NPC_Window::EventInitializeNpcWindow_Implementation(const FText& InName, UDataAsset* InVendorAsset, UAIInteractionManagerComponent* DialogComponent)
{
	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window::EventInitializeNpcWindow_Implementation - Name: %s, VendorAsset: %s, DialogComponent: %s"),
		*InName.ToString(),
		InVendorAsset ? *InVendorAsset->GetName() : TEXT("None"),
		DialogComponent ? *DialogComponent->GetName() : TEXT("None"));

	// Store the passed values
	NpcName = InName;
	VendorAsset = InVendorAsset;
	ActiveDialogComponent = DialogComponent;

	// Set the NPC name text in the UI
	if (UTextBlock* NpcNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("NpcNameText"))))
	{
		NpcNameText->SetText(InName);
		UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window - Set NpcNameText to: %s"), *InName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_NPC_Window - NpcNameText widget not found!"));
	}

	// Make the widget visible
	SetVisibility(ESlateVisibility::Visible);

	// ═══════════════════════════════════════════════════════════════════════════
	// CRITICAL: Set up input context for NPC menu navigation
	// This enables the navigable input mapping and sets this widget as active
	// ═══════════════════════════════════════════════════════════════════════════
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_NPC_Window] No PlayerController from GetOwningPlayer!"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] PC class: %s"), *PC->GetClass()->GetName());

		// ═══════════════════════════════════════════════════════════════════════════
		// CRITICAL: Set input mode to GameAndUI to allow Enhanced Input to receive
		// keyboard events while UI widget has focus. Without this, keyboard/gamepad
		// input only goes to UI widgets, not to Enhanced Input handlers.
		// ═══════════════════════════════════════════════════════════════════════════
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
		UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Set input mode to GameAndUI (allows Enhanced Input + UI)"));

		// Re-enable input using interface (OnDialogStarted disables it)
		if (PC->GetClass()->ImplementsInterface(UBPI_Controller::StaticClass()))
		{
			IBPI_Controller::Execute_ToggleInput(PC, true);
			UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Re-enabled player input via interface"));

			// Set active widget tag for navigation routing
			IBPI_Controller::Execute_SetActiveWidgetForNavigation(PC, NavigableTag);
			UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Set ActiveWidgetTag via interface to: %s"), *NavigableTag.ToString());

			// Try to get GameplayMappingContext from PC
			// Cast to ASLFPlayerController (the active C++ controller class)
			ASLFPlayerController* SLFPlayerController = Cast<ASLFPlayerController>(PC);
			if (SLFPlayerController)
			{
				CachedGameplayContext = SLFPlayerController->IMC_Gameplay;
				UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Got IMC_Gameplay from ASLFPlayerController: %s"),
					CachedGameplayContext ? *CachedGameplayContext->GetName() : TEXT("null"));
			}

			// Fallback: load the gameplay context directly if we couldn't get it from PC
			if (!CachedGameplayContext)
			{
				CachedGameplayContext = LoadObject<UInputMappingContext>(nullptr,
					TEXT("/Game/SoulslikeFramework/Input/IMC_Gameplay.IMC_Gameplay"));
				UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Loaded GameplayMappingContext fallback: %s"),
					CachedGameplayContext ? *CachedGameplayContext->GetName() : TEXT("FAILED"));
			}

			// Switch input context: Enable navigable menu context, disable gameplay context
			// NOTE: Call Enhanced Input subsystem DIRECTLY because Blueprint's Event SwitchInputContext
			// overrides the C++ implementation but the Blueprint logic was cleared during migration
			if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
			{
				if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
				{
					// Remove gameplay context
					if (CachedGameplayContext)
					{
						InputSubsystem->RemoveMappingContext(CachedGameplayContext);
						UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] DIRECT: Removed GameplayMappingContext: %s"), *CachedGameplayContext->GetName());
					}

					// Add navigable menu context
					if (NavigableInputMapping)
					{
						InputSubsystem->AddMappingContext(NavigableInputMapping, 0);
						UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] DIRECT: Added NavigableInputMapping: %s"), *NavigableInputMapping->GetName());
					}

					UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] DIRECT: Switched input context - enabled menu, disabled gameplay"));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("[W_NPC_Window] Could not get EnhancedInputLocalPlayerSubsystem!"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[W_NPC_Window] Could not get LocalPlayer from PC!"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_NPC_Window] PC does not implement BPI_Controller interface!"));
		}
	}

	// ═══════════════════════════════════════════════════════════════════════════
	// Show/hide vendor-specific buttons based on VendorAsset
	// Sell, Buy buttons should only appear for vendor NPCs
	// ═══════════════════════════════════════════════════════════════════════════
	bool bIsVendor = (VendorAsset != nullptr);
	UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] IsVendor: %s"), bIsVendor ? TEXT("YES") : TEXT("NO"));

	// Rebuild NpcMenuButtons to only include visible buttons
	TArray<UW_RestMenu_Button*> VisibleButtons;

	for (UW_RestMenu_Button* Button : NpcMenuButtons)
	{
		if (!Button)
		{
			continue;
		}

		FString ButtonName = Button->GetName();
		bool bShouldBeVisible = true;

		// Hide Sell and Buy buttons if not a vendor NPC
		if (!bIsVendor && (ButtonName.Contains(TEXT("Sell")) || ButtonName.Contains(TEXT("Buy"))))
		{
			bShouldBeVisible = false;
			Button->SetVisibility(ESlateVisibility::Collapsed);
			UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Hiding vendor button: %s"), *ButtonName);
		}
		else
		{
			Button->SetVisibility(ESlateVisibility::Visible);
		}

		if (bShouldBeVisible)
		{
			VisibleButtons.Add(Button);
		}
	}

	// Replace NpcMenuButtons with only visible buttons for navigation
	NpcMenuButtons = VisibleButtons;
	UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Visible buttons for navigation: %d"), NpcMenuButtons.Num());

	// Initialize navigation index to first button
	NavigationIndex = 0;
	if (NpcMenuButtons.Num() > 0 && NpcMenuButtons[0])
	{
		EventOnButtonSelected(NpcMenuButtons[0]);
	}

	// CRITICAL: Set keyboard focus to this widget so NativeOnKeyDown receives input
	SetKeyboardFocus();
	UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Set keyboard focus to NPC window"));

	// Broadcast the OnNpcWindowActive event
	OnNpcWindowActive.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window - Broadcast OnNpcWindowActive"));
}


void UW_NPC_Window::EventNavigateCancel_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window::EventNavigateCancel_Implementation"));

	// Check if we can navigate (widget is visible and interactive)
	if (CanNavigate())
	{
		UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] CanNavigate=true, calling EventCloseNpcMenu"));
		EventCloseNpcMenu();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] CanNavigate=false, not closing"));
	}
}


void UW_NPC_Window::EventNavigateDown_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window::EventNavigateDown_Implementation"));

	if (!CanNavigate())
	{
		return;
	}

	// Navigate to next button in list
	if (NpcMenuButtons.Num() > 0)
	{
		int32 OldIndex = NavigationIndex;
		NavigationIndex = FMath::Min(NavigationIndex + 1, NpcMenuButtons.Num() - 1);

		if (OldIndex != NavigationIndex)
		{
			UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Navigate down: %d -> %d"), OldIndex, NavigationIndex);

			// Deselect old button
			if (NpcMenuButtons.IsValidIndex(OldIndex) && NpcMenuButtons[OldIndex])
			{
				// Call the button's deselect/unhover function if available
			}

			// Select new button
			if (NpcMenuButtons.IsValidIndex(NavigationIndex) && NpcMenuButtons[NavigationIndex])
			{
				EventOnButtonSelected(NpcMenuButtons[NavigationIndex]);
			}
		}
	}
}


void UW_NPC_Window::EventNavigateOk_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window::EventNavigateOk_Implementation"));

	if (!CanNavigate())
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_NPC_Window] EventNavigateOk - CanNavigate returned false!"));
		return;
	}

	// Activate the currently selected button
	if (NpcMenuButtons.IsValidIndex(NavigationIndex) && NpcMenuButtons[NavigationIndex])
	{
		UW_RestMenu_Button* SelectedButton = NpcMenuButtons[NavigationIndex];
		UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Activating button at index %d: %s"),
			NavigationIndex, *SelectedButton->GetName());

		// Check if delegate has any bindings
		bool bHasBindings = SelectedButton->OnRestMenuButtonPressed.IsBound();
		UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Button delegate IsBound: %s"), bHasBindings ? TEXT("YES") : TEXT("NO"));

		if (bHasBindings)
		{
			UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Broadcasting OnRestMenuButtonPressed..."));
			SelectedButton->OnRestMenuButtonPressed.Broadcast();
			UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Broadcast complete"));
		}
		else
		{
			// Fallback: Call the button's EventRestMenuButtonPressed directly
			UE_LOG(LogTemp, Warning, TEXT("[W_NPC_Window] No bindings! Calling EventRestMenuButtonPressed directly..."));
			SelectedButton->EventRestMenuButtonPressed();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_NPC_Window] EventNavigateOk - Invalid NavigationIndex %d (array size: %d)"),
			NavigationIndex, NpcMenuButtons.Num());
	}
}


void UW_NPC_Window::EventNavigateUp_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window::EventNavigateUp_Implementation"));

	if (!CanNavigate())
	{
		return;
	}

	// Navigate to previous button in list
	if (NpcMenuButtons.Num() > 0)
	{
		int32 OldIndex = NavigationIndex;
		NavigationIndex = FMath::Max(NavigationIndex - 1, 0);

		if (OldIndex != NavigationIndex)
		{
			UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Navigate up: %d -> %d"), OldIndex, NavigationIndex);

			// Deselect old button
			if (NpcMenuButtons.IsValidIndex(OldIndex) && NpcMenuButtons[OldIndex])
			{
				// Call the button's deselect/unhover function if available
			}

			// Select new button
			if (NpcMenuButtons.IsValidIndex(NavigationIndex) && NpcMenuButtons[NavigationIndex])
			{
				EventOnButtonSelected(NpcMenuButtons[NavigationIndex]);
			}
		}
	}
}


void UW_NPC_Window::EventOnButtonSelected_Implementation(UW_RestMenu_Button* Button)
{
	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window::EventOnButtonSelected_Implementation - Button: %s"),
		Button ? *Button->GetName() : TEXT("null"));

	// Deselect all buttons first
	for (UW_RestMenu_Button* MenuButton : NpcMenuButtons)
	{
		if (MenuButton && MenuButton != Button)
		{
			MenuButton->SetRestMenuButtonSelected(false);
		}
	}

	// Select the specified button
	if (Button)
	{
		Button->SetRestMenuButtonSelected(true);
		UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window] Button '%s' selected"), *Button->GetName());
	}
}
