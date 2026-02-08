// W_Settings_CategoryEntry.cpp
// C++ Widget implementation for W_Settings_CategoryEntry
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// UPDATED: 2026-01-22 - Full implementation with icon display and button events
//
// Blueprint Logic:
// - PreConstruct: Load Icon and apply to CategoryIcon image widget
// - OnClicked: Call SetCategorySelected(true) which broadcasts OnSelected
// - OnHovered: If not Selected, apply SelectedColor tint
// - OnUnhovered: If Selected keep SelectedColor, else DeselectedColor

#include "Widgets/W_Settings_CategoryEntry.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Engine/Texture2D.h"

UW_Settings_CategoryEntry::UW_Settings_CategoryEntry(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SwitcherIndex = 0;
	CategoryIndex = 0;  // Position in category list (set by W_Settings::InitializeCategories)
	Selected = false;
	SelectedColor = FLinearColor(1.0f, 0.84f, 0.0f, 1.0f); // Gold when selected
	DeselectedColor = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f); // Gray when deselected
	W_Settings = nullptr;
	CachedButton = nullptr;
	CachedCategoryIcon = nullptr;
	CachedSelectedGlow = nullptr;
}

void UW_Settings_CategoryEntry::NativePreConstruct()
{
	Super::NativePreConstruct();

	UE_LOG(LogTemp, Warning, TEXT("[W_Settings_CategoryEntry] NativePreConstruct - Name: %s, Icon.IsNull: %s"),
		*GetName(), Icon.IsNull() ? TEXT("true") : TEXT("false"));

	// Apply icon to the CategoryIcon widget (same as Blueprint PreConstruct)
	ApplyIconToWidget();
}

void UW_Settings_CategoryEntry::NativeConstruct()
{
	Super::NativeConstruct();

	// Fix SwitcherIndex based on widget name (per-instance values lost during reparenting)
	// Values from bp_only: Display=0, Camera=1, Gameplay=-1, Keybinds=2, Sounds=-1, Quit=3
	// -1 means "no dedicated settings panel" for that category
	FString WidgetName = GetName();
	if (WidgetName.Contains(TEXT("Display")))
	{
		SwitcherIndex = 0;
	}
	else if (WidgetName.Contains(TEXT("Camera")))
	{
		SwitcherIndex = 1;
	}
	else if (WidgetName.Contains(TEXT("Gameplay")))
	{
		SwitcherIndex = -1;  // No dedicated panel (was incorrectly 2)
	}
	else if (WidgetName.Contains(TEXT("Keybind")))
	{
		SwitcherIndex = 2;  // Panel index 2 (was incorrectly 3)
	}
	else if (WidgetName.Contains(TEXT("Sound")))
	{
		SwitcherIndex = -1;  // No dedicated panel (was incorrectly 4)
	}
	else if (WidgetName.Contains(TEXT("Quit")))
	{
		SwitcherIndex = 3;  // Panel index 3 (was incorrectly 5)
	}

	// CRITICAL FIX: Force correct colors - Blueprint CDO may have wrong serialized values
	// bp_only uses a tan/gold for selected and brown for deselected
	// These are the correct colors from bp_only:
	// SelectedColor = tan/beige (matches bp_only)
	// But we should use WHITE tint to show the actual texture colors
	SelectedColor = FLinearColor::White;  // No tint when selected - show true colors
	DeselectedColor = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);  // Gray when deselected

	UE_LOG(LogTemp, Warning, TEXT("[W_Settings_CategoryEntry] NativeConstruct - Name: %s, Category: %s, SwitcherIndex: %d"),
		*WidgetName, *SettingCategory.ToString(), SwitcherIndex);

	// Cache widget references
	CacheWidgetReferences();

	// Bind button events
	BindButtonEvents();

	// Apply icon (in case PreConstruct didn't have it yet)
	ApplyIconToWidget();

	// Set initial visual state based on Selected
	if (CachedCategoryIcon)
	{
		// CRITICAL: Check and fix ColorAndOpacity - this can make icons invisible if alpha=0
		FLinearColor CurrentColorOpacity = CachedCategoryIcon->GetColorAndOpacity();
		if (CurrentColorOpacity.A < 0.01f)
		{
			UE_LOG(LogTemp, Error, TEXT("[W_Settings_CategoryEntry] %s - CategoryIcon has ZERO alpha! Fixing..."), *WidgetName);
			CachedCategoryIcon->SetColorAndOpacity(FLinearColor::White);
		}

		FLinearColor InitialColor = Selected ? SelectedColor : DeselectedColor;
		CachedCategoryIcon->SetBrushTintColor(FSlateColor(InitialColor));

		UE_LOG(LogTemp, Warning, TEXT("[W_Settings_CategoryEntry] %s - Applied tint: R=%.3f G=%.3f B=%.3f A=%.3f"),
			*WidgetName, InitialColor.R, InitialColor.G, InitialColor.B, InitialColor.A);
	}
}

void UW_Settings_CategoryEntry::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_Settings_CategoryEntry::NativeDestruct"));
}

void UW_Settings_CategoryEntry::CacheWidgetReferences()
{
	// Cache Button widget
	CachedButton = Cast<UButton>(GetWidgetFromName(TEXT("Button")));
	if (!CachedButton)
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_Settings_CategoryEntry - Could not find 'Button' widget"));
	}

	// Cache CategoryIcon (Image widget that displays the icon)
	CachedCategoryIcon = Cast<UImage>(GetWidgetFromName(TEXT("CategoryIcon")));
	if (!CachedCategoryIcon)
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_Settings_CategoryEntry - Could not find 'CategoryIcon' widget"));
	}

	// Cache SelectedGlow (Border for selection effect)
	CachedSelectedGlow = Cast<UBorder>(GetWidgetFromName(TEXT("SelectedGlow")));
	if (!CachedSelectedGlow)
	{
		UE_LOG(LogTemp, Log, TEXT("UW_Settings_CategoryEntry - No 'SelectedGlow' border (optional)"));
	}
}

void UW_Settings_CategoryEntry::BindButtonEvents()
{
	if (CachedButton)
	{
		// Log button visibility state for debugging
		UE_LOG(LogTemp, Warning, TEXT("[W_Settings_CategoryEntry] %s - Button found, visibility: %d"),
			*GetName(),
			(int32)CachedButton->GetVisibility());

		// CRITICAL FIX: Clear any Blueprint-bound events before rebinding to C++
		// During migration, Blueprint events may still be bound but point to invalid functions
		CachedButton->OnClicked.Clear();
		CachedButton->OnHovered.Clear();
		CachedButton->OnUnhovered.Clear();

		// Bind C++ handlers
		CachedButton->OnClicked.AddDynamic(this, &UW_Settings_CategoryEntry::HandleButtonClicked);
		CachedButton->OnHovered.AddDynamic(this, &UW_Settings_CategoryEntry::HandleButtonHovered);
		CachedButton->OnUnhovered.AddDynamic(this, &UW_Settings_CategoryEntry::HandleButtonUnhovered);

		UE_LOG(LogTemp, Warning, TEXT("[W_Settings_CategoryEntry] %s - Button events bound successfully"), *GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[W_Settings_CategoryEntry] %s - CachedButton is NULL, cannot bind events!"), *GetName());
	}
}

void UW_Settings_CategoryEntry::ApplyIconToWidget()
{
	if (!CachedCategoryIcon)
	{
		// Try to find it if not cached yet
		CachedCategoryIcon = Cast<UImage>(GetWidgetFromName(TEXT("CategoryIcon")));
	}

	if (!CachedCategoryIcon)
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Settings_CategoryEntry] %s - No CategoryIcon widget to apply icon to"), *GetName());
		return;
	}

	// Get widget name to determine correct icon
	FString WidgetName = GetName();

	// ALWAYS use name-based icon selection
	// The Blueprint's per-instance Icon property may have wrong values after migration
	// (e.g., Gameplay has T_Controller instead of T_Gesture)
	FString TextureName;
	FString AlternateTextureName;

	// Check name patterns - order matters, check more specific patterns first
	if (WidgetName.Contains(TEXT("Display")))
	{
		TextureName = TEXT("T_Computer");
	}
	else if (WidgetName.Contains(TEXT("Camera")))
	{
		TextureName = TEXT("T_Camera");
	}
	else if (WidgetName.Contains(TEXT("Gameplay")))
	{
		// User description: "finger pushing button" - this is T_Gesture
		TextureName = TEXT("T_Gesture");
		AlternateTextureName = TEXT("T_Controller"); // Fallback
	}
	else if (WidgetName.Contains(TEXT("Keybind")))
	{
		TextureName = TEXT("T_Keybinds");
	}
	else if (WidgetName.Contains(TEXT("Sound")))
	{
		TextureName = TEXT("T_Volume");
	}
	else if (WidgetName.Contains(TEXT("Quit")))
	{
		TextureName = TEXT("T_Quit");
	}

	if (TextureName.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("[W_Settings_CategoryEntry] %s - No icon mapping for widget name!"), *WidgetName);
		return;
	}

	// Try loading with full path format (Package.Asset)
	FString BasePath = TEXT("/Game/SoulslikeFramework/Widgets/_Textures/");
	FString IconPath = BasePath + TextureName + TEXT(".") + TextureName;

	UE_LOG(LogTemp, Log, TEXT("[W_Settings_CategoryEntry] %s - Trying to load: %s"), *WidgetName, *IconPath);

	UTexture2D* IconTexture = LoadObject<UTexture2D>(nullptr, *IconPath);

	// If primary path fails, try alternate path format (without double name)
	if (!IconTexture)
	{
		FString AltPath = BasePath + TextureName;
		UE_LOG(LogTemp, Warning, TEXT("[W_Settings_CategoryEntry] %s - Primary path failed, trying: %s"), *WidgetName, *AltPath);
		IconTexture = LoadObject<UTexture2D>(nullptr, *AltPath);
	}

	// If still failing and we have an alternate texture name, try that
	if (!IconTexture && !AlternateTextureName.IsEmpty())
	{
		FString AltTexturePath = BasePath + AlternateTextureName + TEXT(".") + AlternateTextureName;
		UE_LOG(LogTemp, Warning, TEXT("[W_Settings_CategoryEntry] %s - Trying alternate texture: %s"), *WidgetName, *AltTexturePath);
		IconTexture = LoadObject<UTexture2D>(nullptr, *AltTexturePath);

		if (!IconTexture)
		{
			// Try without double name
			AltTexturePath = BasePath + AlternateTextureName;
			IconTexture = LoadObject<UTexture2D>(nullptr, *AltTexturePath);
		}
	}

	if (IconTexture)
	{
		// Use SetBrushFromTexture with bMatchSize=true to ensure proper sizing
		CachedCategoryIcon->SetBrushFromTexture(IconTexture, true);

		// Debug: Check brush after setting
		const FSlateBrush& Brush = CachedCategoryIcon->GetBrush();
		FVector2D BrushSize = Brush.GetImageSize();
		UE_LOG(LogTemp, Warning, TEXT("[W_Settings_CategoryEntry] %s - Successfully applied icon: %s, BrushSize: %.1fx%.1f"),
			*WidgetName, *IconTexture->GetPathName(), BrushSize.X, BrushSize.Y);

		// CRITICAL: Ensure the image widget itself is visible and has proper opacity
		CachedCategoryIcon->SetVisibility(ESlateVisibility::Visible);
		CachedCategoryIcon->SetRenderOpacity(1.0f);
		CachedCategoryIcon->SetColorAndOpacity(FLinearColor::White);

		// Log widget state for debugging
		UE_LOG(LogTemp, Warning, TEXT("[W_Settings_CategoryEntry] %s - Icon widget visibility: %d, opacity: %.2f"),
			*WidgetName,
			(int32)CachedCategoryIcon->GetVisibility(),
			CachedCategoryIcon->GetRenderOpacity());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[W_Settings_CategoryEntry] %s - FAILED to load any icon! Tried: %s"), *WidgetName, *TextureName);

		// Last resort: check if the image widget has any existing brush
		const FSlateBrush& CurrentBrush = CachedCategoryIcon->GetBrush();
		if (CurrentBrush.GetResourceObject())
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_Settings_CategoryEntry] %s - Image already has a brush resource: %s"),
				*WidgetName, *CurrentBrush.GetResourceObject()->GetPathName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[W_Settings_CategoryEntry] %s - Image has no brush resource set!"), *WidgetName);
		}
	}
}

void UW_Settings_CategoryEntry::HandleButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("====== UW_Settings_CategoryEntry::HandleButtonClicked ======"));
	UE_LOG(LogTemp, Warning, TEXT("Widget: %s, SwitcherIndex: %d"), *GetName(), SwitcherIndex);

	// Set this category as selected
	SetCategorySelected(true);
}

void UW_Settings_CategoryEntry::HandleButtonHovered()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Settings_CategoryEntry::HandleButtonHovered"));

	// Blueprint logic: If not selected, apply SelectedColor on hover
	if (!Selected && CachedCategoryIcon)
	{
		CachedCategoryIcon->SetBrushTintColor(FSlateColor(SelectedColor));
	}
}

void UW_Settings_CategoryEntry::HandleButtonUnhovered()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Settings_CategoryEntry::HandleButtonUnhovered"));

	// Blueprint logic: If selected, keep SelectedColor; else apply DeselectedColor
	if (CachedCategoryIcon)
	{
		FLinearColor TintColor = Selected ? SelectedColor : DeselectedColor;
		CachedCategoryIcon->SetBrushTintColor(FSlateColor(TintColor));
	}
}

void UW_Settings_CategoryEntry::SetCategorySelected_Implementation(bool InSelected)
{
	// Check if state is actually changing to prevent infinite recursion
	// (UpdateCategorySelection calls this, which would broadcast, which would trigger EventOnCategorySelected again)
	bool bStateChanged = (Selected != InSelected);

	UE_LOG(LogTemp, Log, TEXT("UW_Settings_CategoryEntry::SetCategorySelected - Selected: %s, SwitcherIndex: %d, StateChanged: %s"),
		InSelected ? TEXT("true") : TEXT("false"), SwitcherIndex, bStateChanged ? TEXT("true") : TEXT("false"));

	// Update Selected state
	Selected = InSelected;

	// Update CategoryIcon tint color based on selection
	if (CachedCategoryIcon)
	{
		FLinearColor TintColor = Selected ? SelectedColor : DeselectedColor;
		CachedCategoryIcon->SetBrushTintColor(FSlateColor(TintColor));
	}

	// Update SelectedGlow visibility if present
	if (CachedSelectedGlow)
	{
		CachedSelectedGlow->SetVisibility(Selected ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	// Only broadcast OnSelected when state actually changes from false to true
	// This prevents infinite recursion: SetCategorySelected -> OnSelected.Broadcast ->
	// EventOnCategorySelected -> UpdateCategorySelection -> SetCategorySelected
	// CRITICAL: Broadcast CategoryIndex (position in list), NOT SwitcherIndex (panel index)
	if (InSelected && bStateChanged)
	{
		OnSelected.Broadcast(this, CategoryIndex);
	}
}
