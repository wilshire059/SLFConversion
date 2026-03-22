"""
Surgical NPC System Restore

This script copies ONLY NPC-related assets from bp_only to SLFConversion.
It does NOT touch any other assets to avoid regressions.

Assets to restore:
1. NPC Characters (B_Soulslike_NPC and children)
2. NPC Component (AC_AI_InteractionManager)
3. Dialog Data Assets (PDA_Dialog, DA_ExampleDialog, etc.)
4. Dialog DataTables
5. Vendor Data Assets
6. NPC Widgets (W_Dialog, W_NPC_Window, W_VendorSlot, etc.)
"""
import shutil
import os

BP_ONLY = "C:/scripts/bp_only/Content"
SLF_CONV = "C:/scripts/SLFConversion/Content"

# Define all NPC-related assets to restore
NPC_ASSETS = {
    # NPC Characters
    "SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC.uasset": "Blueprint",
    "SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseGuide.uasset": "Blueprint",
    "SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor.uasset": "Blueprint",

    # NPC Component
    "SoulslikeFramework/Blueprints/Components/AC_AI_InteractionManager.uasset": "Component",

    # Dialog Primary Data Asset class
    "SoulslikeFramework/Data/Dialog/PDA_Dialog.uasset": "PDA Class",

    # Dialog Data Assets
    "SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DA_ExampleDialog.uasset": "Data Asset",
    "SoulslikeFramework/Data/Dialog/ShowcaseVendorNpc/DA_ExampleDialog_Vendor.uasset": "Data Asset",

    # Dialog DataTables
    "SoulslikeFramework/Data/Dialog/DT_GenericDefaultDialog.uasset": "DataTable",
    "SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_NoProgress.uasset": "DataTable",
    "SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_Progress.uasset": "DataTable",
    "SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_Completed.uasset": "DataTable",
    "SoulslikeFramework/Data/Dialog/ShowcaseVendorNpc/DT_ShowcaseVendorNpc_Generic.uasset": "DataTable",

    # Vendor Primary Data Asset class
    "SoulslikeFramework/Data/Vendor/PDA_Vendor.uasset": "PDA Class",

    # Vendor Data Asset
    "SoulslikeFramework/Data/Vendor/DA_ExampleVendor.uasset": "Data Asset",

    # NPC Widgets
    "SoulslikeFramework/Widgets/HUD/W_Dialog.uasset": "Widget",
    "SoulslikeFramework/Widgets/Vendor/W_NPC_Window.uasset": "Widget",
    "SoulslikeFramework/Widgets/Vendor/W_NPC_Window_Vendor.uasset": "Widget",
    "SoulslikeFramework/Widgets/Vendor/W_VendorSlot.uasset": "Widget",
    "SoulslikeFramework/Widgets/Vendor/W_VendorAction.uasset": "Widget",

    # Dialog Structures (if they exist as assets)
    "SoulslikeFramework/Structures/Dialog/FDialogEntry.uasset": "Struct",
    "SoulslikeFramework/Structures/Dialog/FDialogGameplayEvent.uasset": "Struct",
    "SoulslikeFramework/Structures/Dialog/FDialogProgress.uasset": "Struct",
    "SoulslikeFramework/Structures/Dialog/FDialogRequirement.uasset": "Struct",
    "SoulslikeFramework/Structures/Vendor/FVendorItems.uasset": "Struct",
}

def surgical_restore():
    """Perform surgical restore of NPC assets only"""
    print("=" * 60)
    print("SURGICAL NPC SYSTEM RESTORE")
    print("=" * 60)
    print(f"Source: {BP_ONLY}")
    print(f"Destination: {SLF_CONV}")
    print(f"Assets to restore: {len(NPC_ASSETS)}")
    print("=" * 60)

    restored = 0
    skipped = 0
    failed = 0

    for relative_path, asset_type in NPC_ASSETS.items():
        src = os.path.join(BP_ONLY, relative_path)
        dst = os.path.join(SLF_CONV, relative_path)

        # Check if source exists
        if not os.path.exists(src):
            print(f"[SKIP] {asset_type}: {relative_path} (not found in bp_only)")
            skipped += 1
            continue

        # Create destination directory if needed
        dst_dir = os.path.dirname(dst)
        if not os.path.exists(dst_dir):
            os.makedirs(dst_dir)
            print(f"[MKDIR] Created: {dst_dir}")

        # Copy the file
        try:
            shutil.copy2(src, dst)
            print(f"[OK] {asset_type}: {os.path.basename(relative_path)}")
            restored += 1
        except Exception as e:
            print(f"[FAIL] {asset_type}: {relative_path} - {str(e)}")
            failed += 1

    print("\n" + "=" * 60)
    print("RESTORE SUMMARY")
    print("=" * 60)
    print(f"Restored: {restored}")
    print(f"Skipped: {skipped}")
    print(f"Failed: {failed}")
    print("=" * 60)

    return restored, skipped, failed

if __name__ == "__main__":
    surgical_restore()
