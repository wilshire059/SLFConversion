# Widget Blueprint Migration Skill

## Purpose
Migrate Widget Blueprint logic and data to C++ while preserving UMG designer layouts. Widgets require special handling because they combine visual design (preserved) with logic (migrated to C++).

## When to Use
Use this skill when:
- Widget Blueprints display incorrect data (placeholder text, white squares for images)
- Widget events don't trigger properly (button clicks, selection changes)
- Dynamic widget creation is needed (category entries, inventory slots)
- Widget visual states aren't updating (selected/unselected colors)

## The Problem

Widget Blueprints have two parts:
1. **UMG Designer Layout** - Visual hierarchy of widgets (preserved in .uasset)
2. **Blueprint Logic** - EventGraph, custom events, variable bindings (migrated to C++)

After C++ reparenting:
- Widget layout is preserved
- Blueprint logic is cleared
- Data connections are broken
- Visual state functions become stubs

## The Four-Phase Solution

### Phase 1: Data Migration (Struct Properties)

When widget data shows placeholders (lorem ipsum, white squares):

**Root Cause:** Blueprint struct properties use GUID-suffixed names that don't match C++ property names.

**Solution:**
1. Extract data from JSON exports using regex on struct fields
2. Convert JSON booleans (`true`/`false`) to Python (`True`/`False`)
3. Apply via Unreal Python `set_editor_property()`
4. Save assets with `EditorAssetLibrary.save_loaded_asset()`

**Example: Item Data Migration**
```python
# Extract from JSON export (regex pattern)
pattern = rf'{field_prefix}_\d+_[A-F0-9]+=NSLOCTEXT\(\s*"[^"]*"\s*,\s*"[^"]*"\s*,\s*"([^"]*)\"'
match = re.search(pattern, text)
display_name = match.group(1) if match else ""

# Apply to C++ property
item_asset.set_editor_property('item_information', item_info_struct)
unreal.EditorAssetLibrary.save_loaded_asset(item_asset)
```

### Phase 2: Widget Instance Data Migration

When UMG designer has NO children for dynamic containers:

**Root Cause:** The original Blueprint created widgets at runtime, not in the designer.

**Solution:**
1. Check if widget exists via `GetWidgetFromName()`
2. If missing, create dynamically with `CreateWidget<>()`
3. Set properties BEFORE adding to parent
4. Bind event dispatchers with `AddDynamic()`

**Example: Category Entry Creation**
```cpp
void UW_Inventory::CreateCategoryEntries()
{
    UHorizontalBox* CategoriesBox = Cast<UHorizontalBox>(GetWidgetFromName(TEXT("CategoriesBox")));
    if (!CategoriesBox) return;

    TSubclassOf<UW_Inventory_CategoryEntry> EntryClass = LoadClass<UW_Inventory_CategoryEntry>(nullptr,
        TEXT("/Game/SoulslikeFramework/Widgets/Inventory/W_Inventory_CategoryEntry.W_Inventory_CategoryEntry_C"));

    for (const FCategorySetup& Setup : Categories)
    {
        UW_Inventory_CategoryEntry* Entry = CreateWidget<UW_Inventory_CategoryEntry>(GetOwningPlayer(), EntryClass);
        Entry->InventoryCategoryData.Category = Setup.Category;
        Entry->InventoryCategoryData.CategoryIcon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(Setup.IconPath));
        Entry->OnSelected.AddDynamic(this, &UW_Inventory::EventOnCategorySelected);
        CategoriesBox->AddChild(Entry);
    }
}
```

### Phase 3: Event Handler Implementation

When stub implementations only log (don't update widgets):

**Root Cause:** C++ implementations were generated as stubs with `UE_LOG` only.

**Solution:**
1. Read JSON export to find original Blueprint logic
2. Trace: Function Entry → Node calls → Widget updates
3. Implement actual widget updates in `_Implementation()` method

**Example: Button Selection State**
```cpp
void UW_GenericButton::SetButtonSelected_Implementation(bool InSelected)
{
    Selected = InSelected;

    // Update ButtonBorder color based on selection
    if (UBorder* ButtonBorder = Cast<UBorder>(GetWidgetFromName(TEXT("ButtonBorder"))))
    {
        FLinearColor BrushColor = InSelected ? SelectedButtonColor : ButtonColor;
        ButtonBorder->SetBrushColor(BrushColor);
    }
}
```

### Phase 4: Connection Verification

When data flows partially (some widgets update, others don't):

**Root Cause:** Multi-step data flow has broken links.

**Solution:**
1. Trace data flow: Source → Handler → Widget
2. Verify each step executes
3. Add debug logging at connection points
4. Test: Set breakpoint on each step, verify data reaches widget

**Example: Item Info Panel Flow**
```
ItemSlot.OnSelected → W_Inventory.EventOnSlotSelected → EventSetupItemInfoPanel
                                                            ↓
                                                    GetWidgetFromName("ItemOnUseDescription")
                                                            ↓
                                                    TextBlock->SetText(ItemInfo.OnUseDescription)
```

## Widget Property Access Patterns

### Getting Widgets by Name
```cpp
// Find widget in UMG tree
if (UTextBlock* Text = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemDescription"))))
{
    Text->SetText(ItemInfo.ShortDescription);
}
```

### Setting Visual Properties
```cpp
// Border color
Border->SetBrushColor(FLinearColor(0.5f, 0.4f, 0.3f, 1.0f));

// Image texture
Image->SetBrushFromTexture(IconTexture);
Image->SetVisibility(ESlateVisibility::Visible);

// Text content
TextBlock->SetText(FText::FromString(TEXT("Content")));
```

### Event Binding
```cpp
// Bind button click
Button->OnClicked.AddDynamic(this, &UMyWidget::OnButtonClicked);

// Bind custom dispatcher
Entry->OnSelected.AddDynamic(this, &UW_Inventory::EventOnCategorySelected);
```

## Common Widget Types

| Widget Class | Common Operations |
|--------------|-------------------|
| `UTextBlock` | `SetText()`, `SetColorAndOpacity()` |
| `UImage` | `SetBrushFromTexture()`, `SetBrushTintColor()` |
| `UBorder` | `SetBrushColor()`, `SetBrush()` |
| `UButton` | `OnClicked.AddDynamic()`, `OnHovered.AddDynamic()` |
| `UHorizontalBox` | `AddChild()`, `ClearChildren()` |
| `UUniformGridPanel` | `AddChildToUniformGrid(Widget, Row, Col)` |
| `UWidgetSwitcher` | `SetActiveWidgetIndex()`, `GetActiveWidgetIndex()` |

## Debugging Widget Issues

### Check if Widget Exists
```cpp
if (UWidget* W = GetWidgetFromName(TEXT("MyWidget")))
{
    UE_LOG(LogTemp, Log, TEXT("Found MyWidget: %s"), *W->GetClass()->GetName());
}
else
{
    UE_LOG(LogTemp, Warning, TEXT("MyWidget NOT FOUND!"));
}
```

### Log Widget Tree
```cpp
if (WidgetTree)
{
    WidgetTree->ForEachWidget([](UWidget* Widget) {
        UE_LOG(LogTemp, Log, TEXT("Widget: %s (%s)"),
            *Widget->GetName(), *Widget->GetClass()->GetName());
    });
}
```

## JSON Export Analysis

To understand original Blueprint logic:

1. Find the function graph in JSON: `Logic.AllGraphs[].GraphName == "FunctionName"`
2. Trace node connections via `LinkedTo` arrays
3. Identify: variable gets, function calls, branch conditions
4. Map to C++ equivalents

**Example JSON Node**
```json
{
    "NodeClass": "K2Node_CallFunction",
    "NodeTitle": "Set Brush Color",
    "FunctionName": "SetBrushColor",
    "FunctionClass": "Border",
    "Pins": [
        {"Name": "self", "LinkedTo": [{"NodeTitle": "Get ButtonBorder"}]},
        {"Name": "InBrushColor", "LinkedTo": [{"NodeTitle": "Select Color"}]}
    ]
}
```

This tells us: Call `SetBrushColor` on `ButtonBorder` with result from `Select Color` node.

## Verification Checklist

After widget migration, verify:

- [ ] Widget displays correct data (not placeholders)
- [ ] Dynamic widgets created and visible
- [ ] Button clicks trigger handlers
- [ ] Selection states update visually
- [ ] Navigation (keyboard/gamepad) works
- [ ] Event dispatchers broadcast to listeners
- [ ] C++ compiles without errors
- [ ] PIE test shows correct behavior
