import os
from pathlib import Path

widget_dirs = set()
for root, dirs, files in os.walk('Content/SoulslikeFramework/Widgets'):
    for f in files:
        if f.startswith('W_') and f.endswith('.uasset'):
            rel_path = Path(root).relative_to('Content')
            game_path = '/Game/' + str(rel_path).replace(os.sep, '/')
            widget_dirs.add(game_path)

print('Widget directories:')
for d in sorted(widget_dirs):
    print(f'    "{d}",')
