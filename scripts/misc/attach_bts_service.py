# attach_bts_service.py
# Attach BTS_TryGetAbility service to BT_Combat's IN COMBAT node

import unreal

def log(msg):
    print(f"[ATTACH_SVC] {msg}")

def main():
    log("=" * 70)
    log("Attaching BTS_TryGetAbility Service to BT_Combat")
    log("=" * 70)

    bt_path = "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Combat"
    service_path = "/Game/SoulslikeFramework/Blueprints/_AI/Services/BTS_TryGetAbility"

    # Load BT
    bt = unreal.load_asset(bt_path)
    if not bt:
        log(f"ERROR: Could not load BT {bt_path}")
        return

    log(f"Loaded BT: {bt.get_name()}")

    # Load service class
    service_bp = unreal.load_asset(service_path)
    if not service_bp:
        log(f"ERROR: Could not load service {service_path}")
        return

    service_class = service_bp.generated_class()
    log(f"Service class: {service_class.get_name() if service_class else 'None'}")

    # Get root node
    root = bt.get_editor_property('root_node')
    if not root:
        log("ERROR: No root node")
        return

    log(f"Root: {root.get_class().get_name()}")

    # Find the IN COMBAT node (BTComposite_Selector with specific pattern)
    def find_in_combat_node(node):
        if not node:
            return None

        node_name = str(node.get_name()) if hasattr(node, 'get_name') else ""
        node_class = node.get_class().get_name()

        # Try to get node_name property for display name
        try:
            display_name = node.get_editor_property('node_name')
            if display_name and "IN COMBAT" in str(display_name).upper():
                return node
            if display_name and "COMBAT MODE" in str(display_name).upper():
                return node
        except:
            pass

        # Recurse children
        try:
            children = node.get_editor_property('children')
            if children:
                for child in children:
                    child_node = child.get_editor_property('child_node') if hasattr(child, 'get_editor_property') else child
                    result = find_in_combat_node(child_node)
                    if result:
                        return result
        except:
            pass

        return None

    target_node = find_in_combat_node(root)

    if not target_node:
        # Fall back to root
        target_node = root
        log("Using root node as target")
    else:
        log(f"Found target node: {target_node.get_class().get_name()}")

    # Try to add service
    try:
        services = target_node.get_editor_property('services')
        log(f"Current services: {len(services) if services else 0}")

        # Create new service instance
        service_instance = unreal.new_object(service_class, outer=target_node)
        log(f"Created service instance: {service_instance}")

        if service_instance:
            # Add to services array
            if services is None:
                services = []

            services = list(services) if services else []
            services.append(service_instance)

            target_node.set_editor_property('services', services)
            log(f"Added service. New count: {len(services)}")

            # Save BT
            saved = unreal.EditorAssetLibrary.save_asset(bt_path, only_if_is_dirty=False)
            log(f"Saved: {saved}")
    except Exception as e:
        log(f"ERROR: {e}")

    log("")
    log("=" * 70)

if __name__ == "__main__":
    main()
