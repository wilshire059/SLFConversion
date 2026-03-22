#!/usr/bin/env python3
"""Add PDA_Action to migration map so DA_Action_* assets will be UPDA_Action instances"""

migration_path = 'C:/scripts/SLFConversion/run_migration.py'

with open(migration_path, 'r', encoding='utf-8') as f:
    content = f.read()

# Find the MIGRATION_MAP and add PDA_Action
old_migration = '''    # Actions
    "B_Action": "/Script/SLFConversion.SLFActionBase",'''

new_migration = '''    # Primary Data Assets (must be reparented for LoadObject to work)
    "PDA_Action": "/Script/SLFConversion.UPDA_Action",
    # Actions
    "B_Action": "/Script/SLFConversion.SLFActionBase",'''

if old_migration in content:
    content = content.replace(old_migration, new_migration)
    with open(migration_path, 'w', encoding='utf-8') as f:
        f.write(content)
    print('Added PDA_Action to MIGRATION_MAP')
else:
    print('Could not find expected location in MIGRATION_MAP')

# Also add to PATH_OVERRIDES
old_path = '''    # Actions
    "B_Action": "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action",'''

new_path = '''    # Primary Data Assets
    "PDA_Action": "/Game/SoulslikeFramework/Data/Actions/ActionData/PDA_Action",
    # Actions
    "B_Action": "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action",'''

with open(migration_path, 'r', encoding='utf-8') as f:
    content = f.read()

if old_path in content:
    content = content.replace(old_path, new_path)
    with open(migration_path, 'w', encoding='utf-8') as f:
        f.write(content)
    print('Added PDA_Action to PATH_OVERRIDES')
else:
    print('Could not find expected location in PATH_OVERRIDES')
