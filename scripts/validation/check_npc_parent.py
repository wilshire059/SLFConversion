"""
Check actual parent class of NPC Blueprints using export_text.
"""
import unreal
import re

def check_parent_classes():
    unreal.log_warning("\n" + "="*70)
    unreal.log_warning("CHECKING NPC BLUEPRINT PARENT CLASSES")
    unreal.log_warning("="*70 + "\n")

    paths = [
        "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseGuide",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor",
        "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_InteractionManager",
    ]

    for path in paths:
        bp = unreal.EditorAssetLibrary.load_asset(path)
        if not bp:
            unreal.log_warning(f"[MISSING] {path}")
            continue

        name = bp.get_name()
        unreal.log_warning(f"\n{name}:")

        # Export to text to get parent class
        try:
            export_text = unreal.EditorAssetLibrary.get_metadata_tags(path)
            unreal.log_warning(f"  Metadata: {export_text}")
        except Exception as e:
            unreal.log_warning(f"  Metadata error: {e}")

        # Try to get parent class from generated class
        try:
            gen_class = bp.generated_class()
            if gen_class:
                # Walk up the parent chain
                current = gen_class
                hierarchy = []
                for _ in range(10):  # Limit iterations
                    if current:
                        hierarchy.append(current.get_name())
                        # Try to get parent
                        try:
                            parent_class = current.get_super_class()
                            current = parent_class
                        except:
                            break
                    else:
                        break
                unreal.log_warning(f"  Class Chain: {' -> '.join(hierarchy)}")
        except Exception as e:
            unreal.log_warning(f"  Class chain error: {e}")

        # Check parent class via Blueprint properties
        try:
            parent_class = bp.get_editor_property("parent_class")
            unreal.log_warning(f"  Parent Class Property: {parent_class}")
        except Exception as e:
            unreal.log_warning(f"  Parent class property error: {e}")

    unreal.log_warning("\n" + "="*70)

if __name__ == "__main__":
    check_parent_classes()
