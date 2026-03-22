"""Generate a catalog of all Python scripts with descriptions, functions, and git dates."""
import os, sys, ast, subprocess, json
from datetime import datetime

def get_git_dates(filepath, repo_root):
    """Get first created and last modified dates from git."""
    rel = os.path.relpath(filepath, repo_root).replace("\\", "/")
    try:
        # Last modified
        result = subprocess.run(
            ["git", "log", "-1", "--format=%aI", "--", rel],
            capture_output=True, text=True, cwd=repo_root, timeout=10
        )
        last_mod = result.stdout.strip()[:10] if result.stdout.strip() else "unknown"
        
        # First created (follow renames)
        result = subprocess.run(
            ["git", "log", "--follow", "--diff-filter=A", "--format=%aI", "--", rel],
            capture_output=True, text=True, cwd=repo_root, timeout=10
        )
        lines = result.stdout.strip().split("\n")
        created = lines[-1][:10] if lines and lines[-1].strip() else last_mod
    except:
        last_mod = "unknown"
        created = "unknown"
    return created, last_mod

def analyze_python_file(filepath):
    """Extract docstring and function/class names from a Python file."""
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            source = f.read()
        
        tree = ast.parse(source)
        
        # Get module docstring
        docstring = ast.get_docstring(tree) or ""
        if docstring:
            docstring = docstring.split('\n')[0].strip()  # First line only
        
        # If no docstring, try first comment
        if not docstring:
            for line in source.split('\n')[:10]:
                line = line.strip()
                if line.startswith('#') and len(line) > 3:
                    docstring = line.lstrip('# ').strip()
                    break
        
        # Get top-level functions and classes
        functions = []
        classes = []
        for node in ast.iter_child_nodes(tree):
            if isinstance(node, ast.FunctionDef):
                functions.append(node.name)
            elif isinstance(node, ast.ClassDef):
                classes.append(node.name)
        
        return docstring, functions, classes
    except:
        return "", [], []

def scan_directory(dir_path, repo_root, label):
    """Scan a directory and return catalog entries sorted by last_modified desc."""
    entries = []
    if not os.path.exists(dir_path):
        return entries
    
    for fname in sorted(os.listdir(dir_path)):
        if not fname.endswith('.py'):
            continue
        fpath = os.path.join(dir_path, fname)
        if not os.path.isfile(fpath):
            continue
        
        docstring, functions, classes = analyze_python_file(fpath)
        created, last_mod = get_git_dates(fpath, repo_root)
        
        entries.append({
            'name': fname,
            'description': docstring[:120] if docstring else "(no description)",
            'functions': functions[:10],  # Cap at 10
            'classes': classes[:5],
            'created': created,
            'last_modified': last_mod,
            'dir': label
        })
    
    # Sort by last_modified desc (newest first)
    entries.sort(key=lambda e: e['last_modified'], reverse=True)
    return entries

def format_catalog(entries, label, dir_path):
    """Format entries as markdown."""
    lines = [f"## {label} (`{dir_path}`)", f"**{len(entries)} files**", ""]
    lines.append("| File | Description | Key Functions/Classes | Created | Modified |")
    lines.append("|------|-------------|----------------------|---------|----------|")
    
    for e in entries:
        funcs = ", ".join(e['functions'][:5])
        if e['classes']:
            cls = ", ".join(e['classes'][:3])
            funcs = f"**{cls}**; {funcs}" if funcs else f"**{cls}**"
        if len(funcs) > 80:
            funcs = funcs[:77] + "..."
        desc = e['description'][:80]
        lines.append(f"| {e['name']} | {desc} | {funcs} | {e['created']} | {e['last_modified']} |")
    
    lines.append("")
    return "\n".join(lines)

def main():
    repo_root = sys.argv[1] if len(sys.argv) > 1 else "C:/scripts/SLFConversion"
    
    directories = [
        ("Migration Scripts", "scripts/migration"),
        ("Extraction Scripts", "scripts/extraction"),
        ("Validation Scripts", "scripts/validation"),
        ("Debug Scripts", "scripts/debug"),
        ("Misc Scripts", "scripts/misc"),
        ("Blender Pipeline", "scripts/blender"),
        ("Test Scripts", "scripts/tests"),
    ]
    
    # Also scan elden_ring_tools
    elden_ring_dirs = [
        ("Elden Ring Tools (root)", ""),
        ("Elden Ring Tools (test_meshes)", "test_meshes"),
    ]
    
    output = ["# Script Catalog", ""]
    output.append("Auto-generated index of all project scripts. **Check this before writing new scripts.**")
    output.append(f"Last generated: {datetime.now().strftime('%Y-%m-%d %H:%M')}")
    output.append("")
    output.append("---")
    output.append("")
    
    total = 0
    for label, rel_dir in directories:
        full_path = os.path.join(repo_root, rel_dir)
        print(f"Scanning {label}...", file=sys.stderr)
        entries = scan_directory(full_path, repo_root, rel_dir)
        total += len(entries)
        if entries:
            output.append(format_catalog(entries, label, rel_dir))
    
    # Elden ring tools
    elden_root = "C:/scripts/elden_ring_tools"
    if os.path.exists(elden_root):
        for label, rel_dir in elden_ring_dirs:
            full_path = os.path.join(elden_root, rel_dir)
            print(f"Scanning {label}...", file=sys.stderr)
            entries = scan_directory(full_path, elden_root, f"elden_ring_tools/{rel_dir}")
            total += len(entries)
            if entries:
                output.append(format_catalog(entries, label, f"C:/scripts/elden_ring_tools/{rel_dir}"))
    
    output.append(f"\n---\n**Total: {total} scripts cataloged**\n")
    
    catalog_path = os.path.join(repo_root, "scripts", "CATALOG.md")
    with open(catalog_path, 'w', encoding='utf-8') as f:
        f.write("\n".join(output))
    
    print(f"Wrote {total} entries to {catalog_path}", file=sys.stderr)

if __name__ == "__main__":
    main()
