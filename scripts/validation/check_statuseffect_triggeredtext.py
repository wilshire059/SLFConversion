"""
Check TriggeredText on status effect data assets in both projects.
"""

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/statuseffect_triggeredtext.txt"
log_lines = []

def log(msg):
    print(msg)
    log_lines.append(str(msg))

log("=" * 70)
log("CHECKING STATUS EFFECT TRIGGERED TEXT")
log("=" * 70)

STATUS_EFFECTS = [
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Bleed",
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Burn",
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Poison",
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Frostbite",
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Madness",
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Plague",
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Corruption",
]

for path in STATUS_EFFECTS:
    name = path.split("/")[-1]
    log(f"\n--- {name} ---")

    asset = unreal.EditorAssetLibrary.load_asset(path)
    if not asset:
        log("  NOT FOUND")
        continue

    try:
        triggered_text = asset.get_editor_property("triggered_text")
        tag = asset.get_editor_property("tag")

        log(f"  TriggeredText: '{triggered_text}'")
        log(f"  Tag: {tag}")

        # Check if empty
        if not triggered_text or str(triggered_text) == "" or str(triggered_text) == "None":
            log(f"  *** EMPTY - needs value! ***")

    except Exception as e:
        log(f"  Error: {e}")

# Save
with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(log_lines))
log(f"\nSaved to: {OUTPUT_FILE}")
