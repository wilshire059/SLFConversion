# check_loot_manager_reparent.py
# Verify AC_LootDropManager Blueprint is reparented to ULootDropManagerComponent

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("CHECKING AC_LootDropManager REPARENTING")
    unreal.log_warning("=" * 70)

    bp_path = "/Game/SoulslikeFramework/Blueprints/Components/AC_LootDropManager"
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Could not load: {bp_path}")
        return

    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_error("No generated class")
        return

    unreal.log_warning(f"Generated class: {gen_class.get_name()}")

    # Get super class chain
    cpp_class = unreal.load_class(None, "/Script/SLFConversion.LootDropManagerComponent")
    if cpp_class:
        unreal.log_warning(f"\nC++ class ULootDropManagerComponent: {cpp_class.get_name()}")

        # Check if generated class is subclass of the C++ class
        is_subclass = gen_class.is_child_of(cpp_class)
        unreal.log_warning(f"\nIs {gen_class.get_name()} a subclass of {cpp_class.get_name()}? {is_subclass}")

        if is_subclass:
            unreal.log_warning("*** SUCCESS - Blueprint is correctly reparented! ***")
        else:
            unreal.log_warning("*** FAILURE - Blueprint needs to be reparented! ***")

            # Check what it currently inherits from
            actor_component = unreal.load_class(None, "/Script/Engine.ActorComponent")
            if actor_component:
                inherits_actor = gen_class.is_child_of(actor_component)
                unreal.log_warning(f"  Inherits from ActorComponent: {inherits_actor}")

    unreal.log_warning("\n" + "=" * 70)

if __name__ == "__main__":
    main()
