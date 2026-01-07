# debug_key.py
# Debug how to create an FKey in UE Python

import unreal

def debug():
    results = []
    results.append("=" * 60)
    results.append("Key Debug")
    results.append("=" * 60)

    # Try different ways to create a Tab key
    results.append("\n1. unreal.Key()")
    try:
        k = unreal.Key()
        results.append(f"   Created: {k}")
        results.append(f"   Type: {type(k)}")
        results.append(f"   Dir: {[x for x in dir(k) if not x.startswith('_')]}")
    except Exception as e:
        results.append(f"   Error: {e}")

    results.append("\n2. unreal.Key('Tab')")
    try:
        k = unreal.Key("Tab")
        results.append(f"   Created: {k}")
        results.append(f"   Type: {type(k)}")
    except Exception as e:
        results.append(f"   Error: {e}")

    results.append("\n3. unreal.Key(key_name='Tab')")
    try:
        k = unreal.Key(key_name="Tab")
        results.append(f"   Created: {k}")
    except Exception as e:
        results.append(f"   Error: {e}")

    results.append("\n4. Check existing mapping key")
    imc = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Input/IMC_Gameplay")
    if imc:
        key_mappings_data = imc.get_editor_property('default_key_mappings')
        mappings = list(key_mappings_data.get_editor_property('mappings'))
        if mappings:
            m = mappings[0]
            key = m.get_editor_property('key')
            results.append(f"   Key type: {type(key)}")
            results.append(f"   Key value: {key}")
            results.append(f"   Key dir: {[x for x in dir(key) if not x.startswith('_')]}")
            # Try to access properties
            for attr in ['key_name', 'name', 'Name']:
                try:
                    val = getattr(key, attr, "N/A")
                    results.append(f"   key.{attr}: {val}")
                except:
                    pass
            try:
                val = key.get_editor_property('key_name')
                results.append(f"   key.get_editor_property('key_name'): {val}")
            except Exception as e:
                results.append(f"   get_editor_property error: {e}")

    results.append("\n5. Import text approach")
    try:
        k = unreal.Key()
        k.import_text('(KeyName="Tab")')
        results.append(f"   import_text result: {k}")
    except Exception as e:
        results.append(f"   Error: {e}")

    # Write results
    with open("C:/scripts/SLFConversion/debug_key_result.txt", "w") as f:
        f.write("\n".join(results))

debug()
