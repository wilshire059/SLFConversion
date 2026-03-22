"""
migrate_enum_to_cpp.py

Phase 2.1: Migrate E_ValueType Blueprint enum to C++ enum.

APPROACH:
1. Delete the Blueprint E_ValueType.uasset from disk (editor must be CLOSED)
2. CoreRedirects in DefaultEngine.ini will redirect references to C++ E_ValueType
3. When editor opens, all Blueprints will resolve to C++ enum

USAGE (IMPORTANT - Editor must be CLOSED):
    python C:\scripts\SLFConversion\migrate_enum_to_cpp.py

After running:
1. Open Unreal Editor
2. If prompted to resave assets, click "Save All"
3. Open AC_StatManager and verify E_ValueType references work
4. Compile all Blueprints
5. Test in Play mode
"""

import os
import shutil
from datetime import datetime

print("=" * 60)
print("  E_ValueType Enum Migration")
print("  Phase 2.1: Blueprint -> C++")
print("=" * 60)

# Paths
ENUM_UASSET = r"C:\scripts\SLFConversion\Content\SoulslikeFramework\Enums\E_ValueType.uasset"
BACKUP_DIR = r"C:\scripts\SLFConversion_Migration\Backups\enum_migration_backup"

# Step 1: Check if file exists
print("\n[Step 1] Checking Blueprint enum file...")
if not os.path.exists(ENUM_UASSET):
    print(f"  [INFO] E_ValueType.uasset not found - already deleted or migrated")
    print("  Path: " + ENUM_UASSET)
    exit(0)

print(f"  [OK] Found: {ENUM_UASSET}")

# Step 2: Create backup
print("\n[Step 2] Creating backup...")
os.makedirs(BACKUP_DIR, exist_ok=True)
timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
backup_file = os.path.join(BACKUP_DIR, f"E_ValueType_{timestamp}.uasset")
shutil.copy2(ENUM_UASSET, backup_file)
print(f"  [OK] Backed up to: {backup_file}")

# Step 3: Delete the Blueprint enum
print("\n[Step 3] Deleting Blueprint enum...")
try:
    os.remove(ENUM_UASSET)
    print("  [OK] Deleted E_ValueType.uasset")
except Exception as e:
    print(f"  [ERROR] Failed to delete: {e}")
    print("  Make sure Unreal Editor is CLOSED!")
    exit(1)

# Step 4: Verify deletion
print("\n[Step 4] Verifying deletion...")
if os.path.exists(ENUM_UASSET):
    print("  [ERROR] File still exists!")
    exit(1)
print("  [OK] File successfully deleted")

# Print summary
print("\n" + "=" * 60)
print("  ENUM MIGRATION COMPLETE")
print("=" * 60)
print("""
What happened:
- E_ValueType.uasset was deleted from disk
- Backup saved at: """ + backup_file + """

CoreRedirects in DefaultEngine.ini will redirect:
- /Game/SoulslikeFramework/Enums/E_ValueType.E_ValueType
  -> /Script/SLFConversion.E_ValueType

Next steps:
1. Open Unreal Editor
2. If "Failed to load" warnings appear, click through them
3. Check Output Log for redirect messages
4. Open AC_StatManager Blueprint
5. Verify Switch on E_ValueType nodes work
6. Compile all Blueprints (should succeed)
7. Test in Play mode

If errors occur:
1. Close Unreal Editor
2. Restore from backup:
   copy "backup_file" "ENUM_UASSET"
3. Or restore from Phase 1 backup:
   rm -rf "C:/scripts/SLFConversion/Content"
   cp -r "C:/scripts/SLFConversion_Migration/Backups/phase1_complete_20251229_110210/Content" "C:/scripts/SLFConversion/"
""".replace("backup_file", backup_file).replace("ENUM_UASSET", ENUM_UASSET))
