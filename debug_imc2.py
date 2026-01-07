# debug_imc2.py
# Debug InputMappingContextMappingData structure

import unreal

def debug():
    results = []
    results.append("=" * 60)
    results.append("InputMappingContextMappingData Debug")
    results.append("=" * 60)

    imc = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Input/IMC_Gameplay")

    if imc:
        key_mappings = imc.get_editor_property('default_key_mappings')

        results.append(f"\nType: {type(key_mappings)}")

        # List all methods and properties
        results.append("\nAll attributes (non-callable):")
        for attr in dir(key_mappings):
            if not attr.startswith('_'):
                try:
                    val = getattr(key_mappings, attr)
                    if not callable(val):
                        results.append(f"  {attr}: {type(val).__name__} = {val}")
                except Exception as e:
                    results.append(f"  {attr}: ERROR - {e}")

        results.append("\nAll methods:")
        for attr in dir(key_mappings):
            if not attr.startswith('_'):
                try:
                    val = getattr(key_mappings, attr)
                    if callable(val):
                        results.append(f"  {attr}()")
                except:
                    pass

        # Try to get mappings_data or similar
        results.append("\n--- Trying common property names ---")
        for prop_name in ['mappings', 'data', 'key_mappings', 'mapping_data', 'mappings_data', 'items']:
            try:
                val = key_mappings.get_editor_property(prop_name)
                results.append(f"  {prop_name}: {type(val).__name__}")
                if hasattr(val, '__len__'):
                    results.append(f"    length: {len(val)}")
            except Exception as e:
                results.append(f"  {prop_name}: NOT FOUND ({e})")

        # Try to access it as array
        results.append("\n--- Trying array access ---")
        try:
            results.append(f"len(key_mappings): {len(key_mappings)}")
        except Exception as e:
            results.append(f"len(): {e}")

        try:
            results.append(f"key_mappings[0]: {key_mappings[0]}")
        except Exception as e:
            results.append(f"[0]: {e}")

        # Look at the deprecated mappings property more closely
        results.append("\n--- Raw mappings inspection ---")
        raw_mappings = imc.get_editor_property('mappings')
        results.append(f"Raw mappings type: {type(raw_mappings)}")
        results.append(f"Raw mappings dir: {[x for x in dir(raw_mappings) if not x.startswith('_')][:20]}")

    # Write results
    output = "\n".join(results)
    with open("C:/scripts/SLFConversion/debug_imc2_result.txt", "w") as f:
        f.write(output)

debug()
