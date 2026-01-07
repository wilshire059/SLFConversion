# Remove the non-working POST-MIGRATION: Dodge Montages section
import re

with open("C:/scripts/SLFConversion/run_migration.py", "r", encoding="utf-8") as f:
    content = f.read()

# Find and replace the dodge montages section
old_section_start = "    # =========================================================================\n    # POST-MIGRATION: Dodge Montages"
old_section_end = "print(\"Dodge montages: Could not load DA_Action_Dodge\")"

if old_section_start in content and old_section_end in content:
    start_idx = content.find(old_section_start)
    end_idx = content.find(old_section_end) + len(old_section_end)

    replacement = '''    # =========================================================================
    # POST-MIGRATION: Dodge Montages - DISABLED
    # =========================================================================
    # NOTE: This section is DISABLED because struct property changes made via Python
    # don't persist to disk across sessions. Even though save_asset() returns True,
    # the TSoftObjectPtr<> values inside the struct are not serialized correctly.
    #
    # WORKAROUND: After running migration and restoring backup, run:
    #   git checkout d3237aa -- "Content/SoulslikeFramework/Data/Actions/ActionData/"
    #
    # This restores the ActionData folder from the commit that has:
    # 1. PDA_Action reparented to UPDA_ActionBase (C++ class with DodgeMontages property)
    # 2. DA_Action_Dodge with all 9 montages populated
    #
    # The separate migrate_dodge_montages.py script also exists but has the same
    # persistence issue when struct properties contain soft object references.
    unreal.log_warning("POST-MIGRATION: Dodge montages SKIPPED (use git checkout instead)")'''

    content = content[:start_idx] + replacement + content[end_idx:]

    with open("C:/scripts/SLFConversion/run_migration.py", "w", encoding="utf-8") as f:
        f.write(content)
    print("Replaced dodge montages section with DISABLED comment")
else:
    print("Could not find section to replace")
    if old_section_start in content:
        print("Found start")
    else:
        print("Missing start")
    if old_section_end in content:
        print("Found end")
    else:
        print("Missing end")
