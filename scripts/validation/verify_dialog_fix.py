"""
Verify the dialog system fix:
1. Check PDA_Dialog parent class (should be UPDA_Dialog)
2. Check DA_ExampleDialog.DefaultDialogTable (should be DT_ShowcaseGuideNpc_NoProgress)
"""
import unreal

def main():
    unreal.log_warning("\n" + "="*70)
    unreal.log_warning("VERIFYING DIALOG SYSTEM FIX")
    unreal.log_warning("="*70 + "\n")

    # Check 1: PDA_Dialog parent class
    unreal.log_warning("=== Check 1: PDA_Dialog Parent Class ===")
    pda_dialog_path = "/Game/SoulslikeFramework/Data/Dialog/PDA_Dialog"
    pda_dialog = unreal.EditorAssetLibrary.load_asset(pda_dialog_path)
    if pda_dialog:
        unreal.log_warning(f"  [OK] PDA_Dialog loaded")
        gen_class = pda_dialog.generated_class()
        if gen_class:
            class_name = gen_class.get_name()
            unreal.log_warning(f"  Generated class: {class_name}")

            # Check parent by looking at class path
            class_path = gen_class.get_path_name()
            unreal.log_warning(f"  Class path: {class_path}")

            # Try to get parent via outer
            parent_pkg = gen_class.get_outer()
            if parent_pkg:
                unreal.log_warning(f"  Outer: {parent_pkg.get_name()}")
    else:
        unreal.log_warning(f"  [FAIL] Could not load PDA_Dialog")

    # Check 2: DA_ExampleDialog DefaultDialogTable
    unreal.log_warning("\n=== Check 2: DA_ExampleDialog DefaultDialogTable ===")
    da_dialog_path = "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DA_ExampleDialog"
    da_dialog = unreal.EditorAssetLibrary.load_asset(da_dialog_path)
    if da_dialog:
        unreal.log_warning(f"  [OK] DA_ExampleDialog loaded")
        unreal.log_warning(f"  Class: {da_dialog.get_class().get_name()}")

        # Check DefaultDialogTable property
        try:
            default_table = da_dialog.get_editor_property('default_dialog_table')
            if default_table:
                # Handle soft object reference
                if hasattr(default_table, 'get_path_name'):
                    table_path = default_table.get_path_name()
                elif hasattr(default_table, 'to_soft_object_path'):
                    table_path = str(default_table.to_soft_object_path())
                else:
                    table_path = str(default_table)

                unreal.log_warning(f"  DefaultDialogTable: {table_path}")

                # Check if correct
                expected = "DT_ShowcaseGuideNpc_NoProgress"
                if expected in table_path:
                    unreal.log_warning(f"  [PASS] Correct dialog table!")
                else:
                    unreal.log_warning(f"  [WARN] Expected {expected} but got different table")
            else:
                unreal.log_warning(f"  [WARN] DefaultDialogTable is None")
        except Exception as e:
            unreal.log_warning(f"  [ERROR] Could not read DefaultDialogTable: {e}")
    else:
        unreal.log_warning(f"  [FAIL] Could not load DA_ExampleDialog")

    # Check 3: The correct dialog table exists
    unreal.log_warning("\n=== Check 3: Correct Dialog Table Exists ===")
    correct_table_path = "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_NoProgress"
    correct_table = unreal.EditorAssetLibrary.load_asset(correct_table_path)
    if correct_table:
        unreal.log_warning(f"  [OK] DT_ShowcaseGuideNpc_NoProgress exists")

        # Try to get row count
        if hasattr(correct_table, 'get_row_names'):
            row_names = correct_table.get_row_names()
            unreal.log_warning(f"  Row count: {len(row_names)}")
            if len(row_names) > 0:
                unreal.log_warning(f"  First few rows: {[str(r) for r in row_names[:3]]}")
    else:
        unreal.log_warning(f"  [FAIL] Could not load correct dialog table")

    # Check 4: Cast compatibility (the real test)
    unreal.log_warning("\n=== Check 4: Cast Compatibility ===")
    # Try to load UPDA_Dialog class
    try:
        cpp_class = unreal.load_class(None, "/Script/SLFConversion.PDA_Dialog")
        if cpp_class:
            unreal.log_warning(f"  [OK] C++ UPDA_Dialog class loaded")
            unreal.log_warning(f"  C++ class name: {cpp_class.get_name()}")

            # Check if DA_ExampleDialog is an instance of it
            if da_dialog:
                da_class = da_dialog.get_class()
                # Check class hierarchy
                is_child = unreal.MathLibrary.class_is_child_of(da_class, cpp_class)
                unreal.log_warning(f"  DA_ExampleDialog IS-A UPDA_Dialog: {is_child}")
                if is_child:
                    unreal.log_warning(f"  [PASS] Cast will succeed at runtime!")
                else:
                    unreal.log_warning(f"  [FAIL] Cast will fail - reparenting may not have worked")
        else:
            unreal.log_warning(f"  [FAIL] Could not load C++ UPDA_Dialog class")
    except Exception as e:
        unreal.log_warning(f"  [ERROR] Exception loading C++ class: {e}")

    unreal.log_warning("\n" + "="*70)
    unreal.log_warning("VERIFICATION COMPLETE")
    unreal.log_warning("="*70 + "\n")

if __name__ == "__main__":
    main()
