# NPC System Documentation

This folder contains complete, line-by-line documentation of the NPC system from bp_only.
Every variable, component, setting, and asset reference is documented in YAML format.

## Purpose

When migrating from Blueprint to C++:
1. **ONLY** event graphs and function logic are replaced with C++
2. **EVERYTHING ELSE** must be PERFECTLY PRESERVED
3. This documentation serves as the source of truth for validation

## Asset Structure Overview

```
NPC System
├── Blueprints/
│   ├── Characters/
│   │   ├── B_Soulslike_NPC.uasset              # Base NPC class
│   │   ├── B_Soulslike_NPC_ShowcaseGuide.uasset # Guide NPC (dialog only)
│   │   └── B_Soulslike_NPC_ShowcaseVendor.uasset # Vendor NPC (dialog + menu)
│   ├── Components/
│   │   ├── AC_AI_InteractionManager.uasset     # NPC interaction component
│   │   └── AC_InteractionManager.uasset        # Player interaction component
│   └── Interfaces/
│       └── BPI_NPC.uasset                      # NPC interface
├── Data/
│   ├── Dialog/
│   │   ├── PDA_Dialog.uasset                   # Dialog primary data asset class
│   │   ├── DT_GenericDefaultDialog.uasset      # Fallback dialog table
│   │   ├── ShowcaseGuideNpc/
│   │   │   ├── DA_ExampleDialog.uasset         # Guide's dialog configuration
│   │   │   └── DialogTables/
│   │   │       ├── DT_ShowcaseGuideNpc_NoProgress.uasset
│   │   │       ├── DT_ShowcaseGuideNpc_Progress.uasset
│   │   │       └── DT_ShowcaseGuideNpc_Completed.uasset
│   │   └── ShowcaseVendorNpc/
│   │       ├── DA_ExampleDialog_Vendor.uasset  # Vendor's dialog configuration
│   │       └── DT_ShowcaseVendorNpc_Generic.uasset
│   └── Vendor/
│       ├── PDA_Vendor.uasset                   # Vendor primary data asset class
│       └── DA_ExampleVendor.uasset             # Vendor inventory/pricing data
├── Widgets/
│   ├── HUD/
│   │   └── W_Dialog.uasset                     # Dialog text display widget
│   └── Vendor/
│       ├── W_NPC_Window.uasset                 # Base NPC window (for Guide)
│       ├── W_NPC_Window_Vendor.uasset          # Vendor window with buy/sell
│       ├── W_VendorSlot.uasset                 # Individual item slot
│       └── W_VendorAction.uasset               # Buy/Sell action buttons
├── Structures/
│   ├── Dialog/
│   │   ├── FDialogEntry.uasset                 # Single dialog line
│   │   ├── FDialogGameplayEvent.uasset         # Events triggered by dialog
│   │   ├── FDialogProgress.uasset              # Progress tracking
│   │   └── FDialogRequirement.uasset           # Conditions for dialog tables
│   └── Vendor/
│       └── FVendorItems.uasset                 # Vendor item listing
├── Enums/
│   └── E_VendorType.uasset                     # Buy/Sell/Trade enum
├── Input/
│   └── IMC_Dialog.uasset                       # Input mapping for dialog
└── Animation/
    └── ABP_SoulslikeNPC.uasset                 # NPC animation blueprint
```

## YAML Documentation Files

Each asset has a corresponding YAML file with COMPLETE documentation:

### Blueprints
- `blueprints/B_Soulslike_NPC.yaml`
- `blueprints/B_Soulslike_NPC_ShowcaseGuide.yaml`
- `blueprints/B_Soulslike_NPC_ShowcaseVendor.yaml`

### Components
- `components/AC_AI_InteractionManager.yaml`

### Data Assets
- `data/DA_ExampleDialog.yaml`
- `data/DA_ExampleDialog_Vendor.yaml`
- `data/DA_ExampleVendor.yaml`
- `data/PDA_Dialog.yaml`
- `data/PDA_Vendor.yaml`

### DataTables
- `datatables/DT_GenericDefaultDialog.yaml`
- `datatables/DT_ShowcaseGuideNpc_NoProgress.yaml`
- `datatables/DT_ShowcaseGuideNpc_Progress.yaml`
- `datatables/DT_ShowcaseGuideNpc_Completed.yaml`
- `datatables/DT_ShowcaseVendorNpc_Generic.yaml`

### Widgets
- `widgets/W_Dialog.yaml`
- `widgets/W_NPC_Window.yaml`
- `widgets/W_NPC_Window_Vendor.yaml`
- `widgets/W_VendorSlot.yaml`
- `widgets/W_VendorAction.yaml`

### Structures
- `structures/FDialogEntry.yaml`
- `structures/FDialogGameplayEvent.yaml`
- `structures/FDialogProgress.yaml`
- `structures/FDialogRequirement.yaml`
- `structures/FVendorItems.yaml`

## NPC Interaction Flow

```
Player approaches NPC
    │
    ▼
Player presses Interact (via AC_InteractionManager)
    │
    ▼
NPC's AC_AI_InteractionManager receives interaction
    │
    ├── Has DialogAsset? ──► Get dialog text from DataTable
    │                           │
    │                           ▼
    │                       W_Dialog shows text at bottom of screen
    │
    ├── Has VendorAsset? ──► W_NPC_Window_Vendor opens
    │   (Vendor NPC)            │
    │                           ├── Buy tab: W_VendorSlot for each item
    │                           ├── Sell tab: Player inventory items
    │                           └── W_VendorAction for confirm/cancel
    │
    └── No VendorAsset ────► W_NPC_Window (dialog only, no menu)
        (Guide NPC)
```

## Key Differences: Guide vs Vendor NPC

| Aspect | Guide NPC | Vendor NPC |
|--------|-----------|------------|
| Blueprint | B_Soulslike_NPC_ShowcaseGuide | B_Soulslike_NPC_ShowcaseVendor |
| DialogAsset | DA_ExampleDialog | DA_ExampleDialog_Vendor |
| VendorAsset | **NULL** | DA_ExampleVendor |
| Widget | W_NPC_Window (no tabs) | W_NPC_Window_Vendor (with tabs) |
| Shows Menu | NO | YES |
| Shows Dialog | YES | YES |

## Validation Checklist

After migration, verify:

- [ ] All variables have identical values
- [ ] All component properties match
- [ ] All widget hierarchies match
- [ ] All DataTable rows have identical content
- [ ] All asset references point to correct paths
- [ ] All struct properties have correct values
- [ ] Guide NPC shows dialog only (no menu)
- [ ] Vendor NPC shows dialog AND menu
- [ ] Buy/Sell functionality works
- [ ] Dialog text matches bp_only exactly
