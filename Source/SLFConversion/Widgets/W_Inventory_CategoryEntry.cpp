// W_Inventory_CategoryEntry.cpp
// C++ Widget implementation for W_Inventory_CategoryEntry
//
// 20-PASS VALIDATION: 2026-01-08

#include "Widgets/W_Inventory_CategoryEntry.h"
#include "Components/Image.h"
#include "Components/Button.h"

UW_Inventory_CategoryEntry::UW_Inventory_CategoryEntry(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AllCategoriesButton = false;
	Selected = false;
}

void UW_Inventory_CategoryEntry::NativePreConstruct()
{
	Super::NativePreConstruct();

	// Set up the category icon from InventoryCategoryData
	SetupCategoryIcon();
}

void UW_Inventory_CategoryEntry::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	// Bind button events
	BindButtonEvents();

	UE_LOG(LogTemp, Log, TEXT("UW_Inventory_CategoryEntry::NativeConstruct - Category: %d, AllCategories: %s"),
		(int32)InventoryCategoryData.Category, AllCategoriesButton ? TEXT("true") : TEXT("false"));
}

void UW_Inventory_CategoryEntry::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_Inventory_CategoryEntry::NativeDestruct"));
}

void UW_Inventory_CategoryEntry::CacheWidgetReferences()
{
	// Find CategoryIcon image widget
	CategoryIconWidget = Cast<UImage>(GetWidgetFromName(TEXT("CategoryIcon")));

	// Find Button widget
	ButtonWidget = Cast<UButton>(GetWidgetFromName(TEXT("Button")));
}

void UW_Inventory_CategoryEntry::SetupCategoryIcon()
{
	// Find the CategoryIcon image widget
	UImage* IconImage = Cast<UImage>(GetWidgetFromName(TEXT("CategoryIcon")));
	if (!IconImage)
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Inventory_CategoryEntry] CategoryIcon widget not found!"));
		return;
	}

	// Check if we have a valid category icon texture
	if (!InventoryCategoryData.CategoryIcon.IsNull())
	{
		UTexture2D* IconTexture = InventoryCategoryData.CategoryIcon.LoadSynchronous();
		if (IconTexture)
		{
			IconImage->SetBrushFromTexture(IconTexture);
			UE_LOG(LogTemp, Log, TEXT("[W_Inventory_CategoryEntry] Set category icon: %s for category %d"),
				*IconTexture->GetName(), (int32)InventoryCategoryData.Category);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_Inventory_CategoryEntry] Failed to load category icon texture: %s"),
				*InventoryCategoryData.CategoryIcon.ToString());
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[W_Inventory_CategoryEntry] No category icon set for category %d"),
			(int32)InventoryCategoryData.Category);
	}
}

void UW_Inventory_CategoryEntry::BindButtonEvents()
{
	UButton* Button = Cast<UButton>(GetWidgetFromName(TEXT("Button")));
	if (Button)
	{
		Button->OnClicked.AddDynamic(this, &UW_Inventory_CategoryEntry::OnButtonClicked);
		Button->OnHovered.AddDynamic(this, &UW_Inventory_CategoryEntry::OnButtonHovered);
		Button->OnUnhovered.AddDynamic(this, &UW_Inventory_CategoryEntry::OnButtonUnhovered);
	}
}

void UW_Inventory_CategoryEntry::OnButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Inventory_CategoryEntry] Button clicked - Category: %d"), (int32)InventoryCategoryData.Category);

	// Call SetInventoryCategorySelected with true
	SetInventoryCategorySelected(true);
}

void UW_Inventory_CategoryEntry::OnButtonHovered()
{
	// Only change tint if not already selected
	if (!Selected)
	{
		if (UImage* IconImage = Cast<UImage>(GetWidgetFromName(TEXT("CategoryIcon"))))
		{
			// Lighter tint on hover
			FSlateColor HoverTint = FSlateColor(FLinearColor(0.858824f, 0.745098f, 0.619608f, 1.0f));
			IconImage->SetBrushTintColor(HoverTint);
		}
	}
}

void UW_Inventory_CategoryEntry::OnButtonUnhovered()
{
	// Only change tint if not already selected
	if (!Selected)
	{
		if (UImage* IconImage = Cast<UImage>(GetWidgetFromName(TEXT("CategoryIcon"))))
		{
			// Darker tint when not hovered
			FSlateColor NormalTint = FSlateColor(FLinearColor(0.494118f, 0.392157f, 0.282353f, 1.0f));
			IconImage->SetBrushTintColor(NormalTint);
		}
	}
}

void UW_Inventory_CategoryEntry::SetInventoryCategorySelected_Implementation(bool InSelected)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory_CategoryEntry::SetInventoryCategorySelected - Selected: %s, Category: %d"),
		InSelected ? TEXT("true") : TEXT("false"), (int32)InventoryCategoryData.Category);

	Selected = InSelected;

	// Update visual state
	if (UImage* IconImage = Cast<UImage>(GetWidgetFromName(TEXT("CategoryIcon"))))
	{
		if (InSelected)
		{
			// Lighter tint when selected
			FSlateColor SelectedTint = FSlateColor(FLinearColor(0.858824f, 0.745098f, 0.619608f, 1.0f));
			IconImage->SetBrushTintColor(SelectedTint);
		}
		else
		{
			// Darker tint when not selected
			FSlateColor NormalTint = FSlateColor(FLinearColor(0.494118f, 0.392157f, 0.282353f, 1.0f));
			IconImage->SetBrushTintColor(NormalTint);
		}
	}

	// Broadcast selection event
	OnSelected.Broadcast(this, InventoryCategoryData.Category);
}
