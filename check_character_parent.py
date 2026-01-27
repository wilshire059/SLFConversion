"""Check the parent class of B_Soulslike_Character"""
import unreal

def main():
    bp_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
    bp = unreal.load_asset(bp_path)
    if bp:
        gen_class = bp.generated_class()
        if gen_class:
            parent = gen_class.get_super_class()
            print(f"B_Soulslike_Character parent class: {parent.get_name() if parent else 'None'}")
            print(f"Full path: {parent.get_path_name() if parent else 'None'}")
            
            # Check if it's our C++ class
            if parent and "SLFSoulslikeCharacter" in parent.get_name():
                print("CORRECT: Parented to C++ SLFSoulslikeCharacter")
            else:
                print("WRONG: NOT parented to C++ class - input won't work!")
        else:
            print("Could not get generated class")
    else:
        print("Could not load blueprint")
        
if __name__ == "__main__":
    main()
