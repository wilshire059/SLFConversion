// SLFPlayerController.cpp
#include "SLFPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UnrealType.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Widget.h"
#include "Components/WidgetSwitcher.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/PanelWidget.h"
#include "Widgets/W_HUD.h"
#include "Engine/Engine.h"

ASLFPlayerController::ASLFPlayerController()
{
	HUDWidgetRef = nullptr;
	HUDWidgetClass = nullptr;
	GameMenuWidgetClass = nullptr;
	GameMenuWidgetRef = nullptr;

	// Load IA_GameMenu input action
	static ConstructorHelpers::FObjectFinder<UInputAction> GameMenuActionFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/IA_GameMenu.IA_GameMenu"));
	if (GameMenuActionFinder.Succeeded())
	{
		IA_GameMenu = GameMenuActionFinder.Object;
	}
	else
	{
		IA_GameMenu = nullptr;
	}

	// NOTE: Don't load widget classes in constructor - they have compile errors
	// that cause the editor to hang. Widget search is done at runtime instead.
}

void ASLFPlayerController::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] BeginPlay - C++ PlayerController active"));

	// From Blueprint DNA: BeginPlay → Sequence → Native_InitializeHUD
	// Initialize the HUD widget (creates W_HUD and adds to viewport)
	// Note: Using Native_InitializeHUD instead of Native_InitializeHUD to avoid Blueprint name conflict
	Native_InitializeHUD();

	// Note: W_GameMenu has compile errors, so we handle toggle in HandleGameMenuInput
	// using reflection search fallback
	if (GameMenuWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] GameMenuWidgetClass loaded successfully"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] GameMenuWidgetClass is NULL (widget has compile errors?)"));
	}
}

void ASLFPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] SetupInputComponent"));

	// Get Enhanced Input Component
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Bind IA_GameMenu to our handler
		if (IA_GameMenu)
		{
			EnhancedInput->BindAction(IA_GameMenu, ETriggerEvent::Started, this, &ASLFPlayerController::HandleGameMenuInput);
			UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] Bound IA_GameMenu to HandleGameMenuInput"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] IA_GameMenu is NULL - could not bind"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] No EnhancedInputComponent found"));
	}
}

void ASLFPlayerController::HandleGameMenuInput(const FInputActionValue& Value)
{
	// Debounce check - prevent double-fire from Blueprint + C++ bindings
	double CurrentTime = FPlatformTime::Seconds();
	if (CurrentTime - LastMenuInputTime < 0.1)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] HandleGameMenuInput DEBOUNCED (%.3f sec since last)"),
			CurrentTime - LastMenuInputTime);
		return;
	}
	LastMenuInputTime = CurrentTime;

	UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] HandleGameMenuInput triggered (Tab pressed)!"));

	// Direct approach: Find W_HUD and access its W_GameMenu variable via reflection
	// W_HUD has compile errors so we can't rely on its Blueprint events

	TArray<UUserWidget*> AllWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), AllWidgets, UUserWidget::StaticClass(), false);

	for (UUserWidget* Widget : AllWidgets)
	{
		FString ClassName = Widget->GetClass()->GetName();

		// Find W_HUD and access its W_GameMenu child widget
		if (ClassName.Equals(TEXT("W_HUD_C")))
		{
			UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] Found W_HUD_C, looking for W_GameMenu property..."));

			// Access W_GameMenu variable via reflection
			FProperty* GameMenuProp = Widget->GetClass()->FindPropertyByName(FName(TEXT("W_GameMenu")));
			if (GameMenuProp)
			{
				FObjectProperty* ObjProp = CastField<FObjectProperty>(GameMenuProp);
				if (ObjProp)
				{
					UObject* GameMenuObj = ObjProp->GetObjectPropertyValue_InContainer(Widget);
					UUserWidget* GameMenu = Cast<UUserWidget>(GameMenuObj);

					if (GameMenu)
					{
						UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] Found W_GameMenu widget via reflection!"));

						// Toggle visibility
						ESlateVisibility CurrentVis = GameMenu->GetVisibility();
						bool bIsVisible = CurrentVis == ESlateVisibility::Visible;
						ESlateVisibility NewVis = bIsVisible ? ESlateVisibility::Collapsed : ESlateVisibility::Visible;
						GameMenu->SetVisibility(NewVis);

						UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] W_GameMenu visibility: %d -> %d"),
							static_cast<int32>(CurrentVis), static_cast<int32>(NewVis));

						// Debug: Check size and render opacity
						FVector2D DesiredSize = GameMenu->GetDesiredSize();
						float RenderOpacity = GameMenu->GetRenderOpacity();
						UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] W_GameMenu DesiredSize: %.1f x %.1f, RenderOpacity: %.2f"),
							DesiredSize.X, DesiredSize.Y, RenderOpacity);

						// Force layout update
						GameMenu->ForceLayoutPrepass();
						FVector2D SizeAfterLayout = GameMenu->GetDesiredSize();
						UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] W_GameMenu after ForceLayoutPrepass: %.1f x %.1f"),
							SizeAfterLayout.X, SizeAfterLayout.Y);

						// Check if widget has a Canvas slot and its position/size
						if (UPanelSlot* Slot = GameMenu->Slot)
						{
							if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Slot))
							{
								FAnchors Anchors = CanvasSlot->GetAnchors();
								FVector2D SlotPos = CanvasSlot->GetPosition();
								FVector2D SlotSize = CanvasSlot->GetSize();
								UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] CanvasSlot - Pos: (%.1f, %.1f), Size: (%.1f, %.1f), Anchors: (%.2f,%.2f)-(%.2f,%.2f)"),
									SlotPos.X, SlotPos.Y, SlotSize.X, SlotSize.Y,
									Anchors.Minimum.X, Anchors.Minimum.Y, Anchors.Maximum.X, Anchors.Maximum.Y);
							}
							else
							{
								UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] W_GameMenu Slot type: %s (not CanvasPanelSlot)"),
									*Slot->GetClass()->GetName());
							}
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] W_GameMenu has NO Slot!"));
						}

						// Log parent widget hierarchy visibility
						UWidget* ParentWidget = GameMenu->GetParent();
						int32 ParentLevel = 0;
						while (ParentWidget && ParentLevel < 5)
						{
							UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] Parent[%d]: %s - Visibility: %d"),
								ParentLevel, *ParentWidget->GetClass()->GetName(), static_cast<int32>(ParentWidget->GetVisibility()));
							ParentWidget = ParentWidget->GetParent();
							ParentLevel++;
						}

						// CRITICAL: Also need to ensure ViewportSwitcher is showing MainHUD (index 0)
						// The menu is inside: W_GameMenu -> HudElementsCanvas -> MainHUD -> ViewportSwitcher
						// ViewportSwitcher must be set to show MainHUD for W_GameMenu to be visible
						FProperty* SwitcherProp = Widget->GetClass()->FindPropertyByName(FName(TEXT("ViewportSwitcher")));
						if (SwitcherProp)
						{
							FObjectProperty* SwitcherObjProp = CastField<FObjectProperty>(SwitcherProp);
							if (SwitcherObjProp)
							{
								UObject* SwitcherObj = SwitcherObjProp->GetObjectPropertyValue_InContainer(Widget);
								UWidgetSwitcher* Switcher = Cast<UWidgetSwitcher>(SwitcherObj);
								if (Switcher)
								{
									int32 CurrentIndex = Switcher->GetActiveWidgetIndex();
									UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] ViewportSwitcher current index: %d, NumWidgets: %d"),
										CurrentIndex, Switcher->GetNumWidgets());

									// MainHUD is at index 0 (default gameplay view)
									// When opening menu, we stay on MainHUD but make W_GameMenu visible
									// The switcher should already be at index 0 for normal gameplay
									if (CurrentIndex != 0)
									{
										Switcher->SetActiveWidgetIndex(0);
										UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] Set ViewportSwitcher to index 0 (MainHUD)"));
									}
								}
								else
								{
									UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] ViewportSwitcher cast failed"));
								}
							}
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] ViewportSwitcher property not found"));
						}

						// Update input mode and cursor
						if (bIsVisible)
						{
							SetInputMode(FInputModeGameOnly());
							bShowMouseCursor = false;
							UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] Menu CLOSED"));
							if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("Menu CLOSED"));
						}
						else
						{
							SetInputMode(FInputModeGameAndUI());
							bShowMouseCursor = true;
							UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] Menu OPENED"));
							if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Menu OPENED"));
						}
						return;
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("[SLFPlayerController] W_GameMenu property found but Cast to UUserWidget failed!"));
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("[SLFPlayerController] W_GameMenu property is not an FObjectProperty!"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[SLFPlayerController] W_GameMenu property not found on W_HUD_C!"));
			}

			// If reflection failed, log all properties for debugging
			UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] Listing W_HUD properties:"));
			for (TFieldIterator<FProperty> PropIt(Widget->GetClass()); PropIt; ++PropIt)
			{
				FProperty* Prop = *PropIt;
				if (Prop->GetName().Contains(TEXT("GameMenu")) || Prop->GetName().Contains(TEXT("Menu")))
				{
					UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController]   - %s (type: %s)"),
						*Prop->GetName(), *Prop->GetClass()->GetName());
				}
			}
			return;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] W_HUD_C not found in widget list"));
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("W_HUD not found!"));
	}
}

void ASLFPlayerController::Native_InitializeHUD()
{
	UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] Native_InitializeHUD - Initializing HUD"));

	// From Blueprint DNA PC_SoulslikeFramework.json:
	// Native_InitializeHUD flow:
	// 1. SetInputMode_GameOnly
	// 2. bShowMouseCursor = false
	// 3. CreateWidget<W_HUD_C> (class: /Game/SoulslikeFramework/Widgets/HUD/W_HUD.W_HUD_C)
	// 4. Set W_HUD variable
	// 5. AddToViewport

	// Step 1: Set input mode to game only
	SetInputMode(FInputModeGameOnly());
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] Native_InitializeHUD - Set InputMode to GameOnly"));

	// Step 2: Hide mouse cursor
	bShowMouseCursor = false;
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] Native_InitializeHUD - Set bShowMouseCursor = false"));

	// Step 3: Load W_HUD widget class at runtime (avoiding constructor loading issues)
	if (!HUDWidgetClass)
	{
		// Load the Blueprint widget class from content path
		const FString HUDWidgetPath = TEXT("/Game/SoulslikeFramework/Widgets/HUD/W_HUD.W_HUD_C");
		HUDWidgetClass = LoadClass<UUserWidget>(nullptr, *HUDWidgetPath);

		if (HUDWidgetClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] Native_InitializeHUD - Loaded HUDWidgetClass: %s"), *HUDWidgetClass->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[SLFPlayerController] Native_InitializeHUD - FAILED to load W_HUD class from: %s"), *HUDWidgetPath);
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("FAILED to load W_HUD widget class!"));
			}
			return;
		}
	}

	// Step 4: Create widget
	if (HUDWidgetClass)
	{
		HUDWidgetRef = CreateWidget<UW_HUD>(this, HUDWidgetClass);

		if (HUDWidgetRef)
		{
			UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] Native_InitializeHUD - Created W_HUD widget: %s"), *HUDWidgetRef->GetName());

			// Step 5: Add to viewport
			HUDWidgetRef->AddToViewport();
			UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] Native_InitializeHUD - Added W_HUD to viewport"));

			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("W_HUD created and added to viewport!"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[SLFPlayerController] Native_InitializeHUD - FAILED to create W_HUD widget instance"));
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("FAILED to create W_HUD widget instance!"));
			}
		}
	}
}
