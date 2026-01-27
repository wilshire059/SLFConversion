"""
Check PDA_Item Blueprint parent class.
"""
import unreal
import json

def check_pda_parent():
    """Check what class PDA_Item Blueprint inherits from."""
    results = {}

    # Load the PDA_Item Generated Class directly
    gen_class_path = "/Game/SoulslikeFramework/Data/PDA_Item.PDA_Item_C"
    gen_class = unreal.load_class(None, gen_class_path)

    if gen_class:
        results["loaded"] = True
        results["gen_class_name"] = gen_class.get_name()
        
        # Check if it derives from C++ PDA_Item
        cpp_class = unreal.load_class(None, "/Script/SLFConversion.PDA_Item")
        if cpp_class:
            results["cpp_class_found"] = True
            results["cpp_class_name"] = cpp_class.get_name()
            # Check inheritance
            is_child = unreal.MathLibrary.class_is_child_of(gen_class, cpp_class)
            results["is_child_of_cpp_pda_item"] = is_child
        else:
            results["cpp_class_found"] = False
            
        # Also check against UPrimaryDataAsset
        pda_class = unreal.load_class(None, "/Script/Engine.PrimaryDataAsset")
        if pda_class:
            is_pda_child = unreal.MathLibrary.class_is_child_of(gen_class, pda_class)
            results["is_child_of_primary_data_asset"] = is_pda_child
    else:
        results["loaded"] = False

    # Write results to file
    output_path = "C:/scripts/slfconversion/migration_cache/pda_parent_check.json"
    with open(output_path, "w") as f:
        json.dump(results, f, indent=2)
    
    print(f"Results saved to {output_path}")

if __name__ == "__main__":
    check_pda_parent()
