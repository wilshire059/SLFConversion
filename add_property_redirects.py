# Add property redirectors for the ? suffix issue
import os

# Property redirects for variables with ? suffix that need to map to C++ names
redirects = [
    # AC_SaveLoadManager
    ('AC_SaveLoadManager', 'AutoSaveNeeded?', 'AutoSaveNeeded'),
    ('AC_SaveLoadManager', 'CanResave?', 'CanResave'),
    # AC_InputBuffer
    ('AC_InputBuffer', 'BufferOpen?', 'BufferOpen'),
    # AC_RadarManager
    ('AC_RadarManager', 'Visible?', 'Visible'),
    ('AC_RadarManager', 'ShouldUpdate?', 'ShouldUpdate'),
]

# Read existing DefaultEngine.ini
ini_path = 'Config/DefaultEngine.ini'
with open(ini_path, 'r', encoding='utf-8') as f:
    content = f.read()

# Check if CoreRedirects section exists
if '[CoreRedirects]' not in content:
    content += '\n\n[CoreRedirects]\n'

# Add property redirects
new_redirects = []
for comp_class, old_prop, new_prop in redirects:
    redirect_line = f'+PropertyRedirects=(OldName="/Script/SLFConversion.U{comp_class}.{old_prop}",NewName="/Script/SLFConversion.U{comp_class}.{new_prop}")'
    if redirect_line not in content:
        new_redirects.append(redirect_line)
        print(f'Adding redirect: {comp_class}.{old_prop} -> {new_prop}')

if new_redirects:
    # Find CoreRedirects section and append
    lines = content.split('\n')
    new_lines = []
    added = False
    for i, line in enumerate(lines):
        new_lines.append(line)
        if line.strip() == '[CoreRedirects]' and not added:
            for redirect in new_redirects:
                new_lines.append(redirect)
            added = True
    
    content = '\n'.join(new_lines)
    
    with open(ini_path, 'w', encoding='utf-8') as f:
        f.write(content)
    
    print(f'\nAdded {len(new_redirects)} property redirects to DefaultEngine.ini')
else:
    print('All redirects already present')
