#!/usr/bin/env python3
"""Add PDA_Action to migration map"""

path = 'C:/scripts/SLFConversion/run_migration.py'

with open(path, 'r', encoding='utf-8') as f:
    content = f.read()

old = '''    "B_Buff": "/Script/SLFConversion.SLFBuffBase",
    "B_Action": "/Script/SLFConversion.SLFActionBase",'''

new = '''    "B_Buff": "/Script/SLFConversion.SLFBuffBase",
    # Primary Data Assets (must be reparented for LoadObject to work correctly)
    "PDA_Action": "/Script/SLFConversion.UPDA_Action",
    "B_Action": "/Script/SLFConversion.SLFActionBase",'''

if old in content:
    content = content.replace(old, new)
    with open(path, 'w', encoding='utf-8') as f:
        f.write(content)
    print('Added PDA_Action to MIGRATION_MAP')
elif '"PDA_Action"' in content:
    print('PDA_Action already in MIGRATION_MAP')
else:
    print('Could not find insertion point')
