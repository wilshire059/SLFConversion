"""Run NPC Dialog PIE test."""
import unreal
import sys

unreal.log_warning("=" * 70)
unreal.log_warning("NPC DIALOG PIE TEST")
unreal.log_warning("=" * 70)

# This needs to run in PIE mode, so we check if we're in a game world
world = unreal.EditorLevelLibrary.get_editor_world()

if world:
    unreal.log_warning(f"Current world: {world.get_name()}")
else:
    unreal.log_warning("No world loaded")

# Check if DataTables can be read
unreal.log_warning("")
unreal.log_warning("Testing DataTable access...")

test_tables = [
    "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_NoProgress",
    "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_Completed",
]

for table_path in test_tables:
    table_name = table_path.split('/')[-1]

    # Test reading first row
    text = unreal.SLFAutomationLibrary.get_dialog_entry_text(table_path, "NewRow")
    event_count = unreal.SLFAutomationLibrary.get_dialog_entry_event_count(table_path, "NewRow")

    unreal.log_warning(f"")
    unreal.log_warning(f"{table_name}:")
    unreal.log_warning(f"  Row NewRow text: {text[:50]}...")
    unreal.log_warning(f"  Row NewRow events: {event_count}")

# Test specific rows with events
unreal.log_warning("")
unreal.log_warning("Testing rows with GameplayEvents:")

# NoProgress NewRow_3 has AddItem event
text = unreal.SLFAutomationLibrary.get_dialog_entry_text(
    "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_NoProgress",
    "NewRow_3"
)
events = unreal.SLFAutomationLibrary.get_dialog_entry_event_count(
    "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_NoProgress",
    "NewRow_3"
)
unreal.log_warning(f"NoProgress NewRow_3:")
unreal.log_warning(f"  Text: {text[:60]}...")
unreal.log_warning(f"  GameplayEvents: {events}")
unreal.log_warning(f"  Expected: 1 (AddItem HealthFlask)")

# Completed NewRow_2 has AddCurrency event
text = unreal.SLFAutomationLibrary.get_dialog_entry_text(
    "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_Completed",
    "NewRow_2"
)
events = unreal.SLFAutomationLibrary.get_dialog_entry_event_count(
    "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DialogTables/DT_ShowcaseGuideNpc_Completed",
    "NewRow_2"
)
unreal.log_warning(f"Completed NewRow_2:")
unreal.log_warning(f"  Text: {text[:60]}...")
unreal.log_warning(f"  GameplayEvents: {events}")
unreal.log_warning(f"  Expected: 1 (AddCurrency 100)")

unreal.log_warning("")
unreal.log_warning("=" * 70)
unreal.log_warning("TEST COMPLETE - DataTables are accessible")
unreal.log_warning("For full PIE test, run: SLF.Test.NPCDialog in console")
unreal.log_warning("=" * 70)
