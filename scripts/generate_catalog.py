"""Generate a catalog of all Python and C++ scripts with descriptions, functions, and git dates."""
import os, sys, ast, re, subprocess
from datetime import datetime


def get_git_dates(filepath, repo_root):
    """Get first created and last modified dates from git."""
    rel = os.path.relpath(filepath, repo_root).replace("\\", "/")
    try:
        result = subprocess.run(
            ["git", "log", "-1", "--format=%aI", "--", rel],
            capture_output=True, text=True, cwd=repo_root, timeout=10
        )
        last_mod = result.stdout.strip()[:10] if result.stdout.strip() else "unknown"
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
        docstring = ast.get_docstring(tree) or ""
        if docstring:
            docstring = docstring.split('\n')[0].strip()
        if not docstring:
            for line in source.split('\n')[:10]:
                line = line.strip()
                if line.startswith('#') and len(line) > 3:
                    docstring = line.lstrip('# ').strip()
                    break
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


def analyze_cpp_file(filepath):
    """Extract description, class names, and key function names from a C++ header or source file."""
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            lines = f.readlines()
    except:
        return "", [], []

    docstring = ""
    classes = []
    functions = []
    skip_words = {'if', 'for', 'while', 'switch', 'return', 'else', 'class',
                  'struct', 'enum', 'typedef', 'using', 'Super', 'GENERATED_BODY',
                  'UPROPERTY', 'UFUNCTION', 'UCLASS', 'USTRUCT', 'TEXT', 'Cast',
                  'FindObject', 'NewObject', 'CreateDefaultSubobject', 'check',
                  'ensure', 'verify'}

    # Get description from first comment block
    for line in lines[:25]:
        stripped = line.strip()
        if stripped.startswith('//') and len(stripped) > 4 and not stripped.startswith('// Copyright'):
            docstring = stripped.lstrip('/ ').strip()
            break
        elif stripped.startswith('*') and len(stripped) > 3 and not stripped.startswith('*/'):
            cand = stripped.lstrip('* ').strip()
            if cand and not cand.startswith('@'):
                docstring = cand
                break

    is_header = filepath.endswith('.h')

    for line in lines:
        stripped = line.strip()
        if stripped.startswith('//') or stripped.startswith('*'):
            continue

        # Class/struct declarations
        m = re.match(r'(?:UCLASS|USTRUCT|UENUM)?\s*(?:\([^)]*\)\s*)?class\s+(?:\w+_API\s+)?(\w+)', stripped)
        if m:
            name = m.group(1)
            if len(name) > 2 and name not in skip_words:
                classes.append(name)

        if is_header:
            # UFUNCTION-marked methods
            m = re.match(r'\s*(?:virtual\s+|static\s+|FORCEINLINE\s+)*(?:[\w:*&<>,\s]+?)\s+(\w+)\s*\([^;]*$', stripped)
            if m:
                fname = m.group(1)
                if fname not in skip_words and not fname.startswith('DECLARE_') and not fname.startswith('GENERATED_'):
                    if fname not in functions:
                        functions.append(fname)
        else:
            # Source: ClassName::Method definitions
            m = re.match(r'[\w:*&<>,\s]+\s+\w+::(\w+)\s*\(', stripped)
            if m:
                fname = m.group(1)
                if fname not in skip_words and fname not in functions:
                    functions.append(fname)

    return docstring, functions[:15], classes[:5]


def scan_directory(dir_path, repo_root, label):
    """Scan a directory for Python files and return catalog entries."""
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
            'functions': functions[:10],
            'classes': classes[:5],
            'created': created,
            'last_modified': last_mod,
            'dir': label
        })
    entries.sort(key=lambda e: e['last_modified'], reverse=True)
    return entries


def scan_cpp_tree(dir_path, repo_root, label):
    """Recursively scan a directory tree for C++ .h/.cpp files."""
    entries = []
    if not os.path.exists(dir_path):
        return entries
    for root, dirs, files in os.walk(dir_path):
        for fname in sorted(files):
            if not (fname.endswith('.h') or fname.endswith('.cpp')):
                continue
            fpath = os.path.join(root, fname)
            rel_dir = os.path.relpath(root, dir_path).replace("\\", "/")
            display_name = f"{rel_dir}/{fname}" if rel_dir != "." else fname

            docstring, functions, classes = analyze_cpp_file(fpath)
            created, last_mod = get_git_dates(fpath, repo_root)

            entries.append({
                'name': display_name,
                'description': docstring[:120] if docstring else "(no description)",
                'functions': functions,
                'classes': classes,
                'created': created,
                'last_modified': last_mod,
                'dir': label
            })
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

    output = ["# Script & Source Catalog", ""]
    output.append("Auto-generated index of all project scripts and C++ source files.")
    output.append("**Check this before writing new scripts or C++ classes.**")
    output.append(f"Last generated: {datetime.now().strftime('%Y-%m-%d %H:%M')}")
    output.append("")
    output.append("---")
    output.append("")

    total = 0

    # Python scripts
    py_directories = [
        ("Migration Scripts", "scripts/migration"),
        ("Extraction Scripts", "scripts/extraction"),
        ("Validation Scripts", "scripts/validation"),
        ("Debug Scripts", "scripts/debug"),
        ("Misc Scripts", "scripts/misc"),
        ("Blender Pipeline", "scripts/blender"),
        ("Test Scripts", "scripts/tests"),
    ]
    for label, rel_dir in py_directories:
        full_path = os.path.join(repo_root, rel_dir)
        print(f"Scanning {label}...", file=sys.stderr)
        entries = scan_directory(full_path, repo_root, rel_dir)
        total += len(entries)
        if entries:
            output.append(format_catalog(entries, label, rel_dir))

    # C++ source
    cpp_directories = [
        ("C++ Source (root)", "Source/SLFConversion"),
        ("C++ AI", "Source/SLFConversion/AI"),
        ("C++ AnimNotifies", "Source/SLFConversion/AnimNotifies"),
        ("C++ Animation", "Source/SLFConversion/Animation"),
        ("C++ Blueprints", "Source/SLFConversion/Blueprints"),
        ("C++ Components", "Source/SLFConversion/Components"),
        ("C++ Dungeon", "Source/SLFConversion/Dungeon"),
        ("C++ Framework", "Source/SLFConversion/Framework"),
        ("C++ GameFramework", "Source/SLFConversion/GameFramework"),
        ("C++ Interfaces", "Source/SLFConversion/Interfaces"),
        ("C++ Testing", "Source/SLFConversion/Testing"),
        ("C++ Tests", "Source/SLFConversion/Tests"),
        ("C++ Utilities", "Source/SLFConversion/Utilities"),
        ("C++ Widgets", "Source/SLFConversion/Widgets"),
    ]
    for label, rel_dir in cpp_directories:
        full_path = os.path.join(repo_root, rel_dir)
        if not os.path.exists(full_path):
            continue
        print(f"Scanning {label}...", file=sys.stderr)
        # Root Source dir: non-recursive (just top-level files)
        # Subdirs: recursive
        if rel_dir == "Source/SLFConversion":
            entries = []
            for fname in sorted(os.listdir(full_path)):
                fpath = os.path.join(full_path, fname)
                if not os.path.isfile(fpath):
                    continue
                if not (fname.endswith('.h') or fname.endswith('.cpp')):
                    continue
                docstring, functions, classes = analyze_cpp_file(fpath)
                created, last_mod = get_git_dates(fpath, repo_root)
                entries.append({
                    'name': fname,
                    'description': docstring[:120] if docstring else "(no description)",
                    'functions': functions,
                    'classes': classes,
                    'created': created,
                    'last_modified': last_mod,
                    'dir': rel_dir
                })
            entries.sort(key=lambda e: e['last_modified'], reverse=True)
        else:
            entries = scan_cpp_tree(full_path, repo_root, rel_dir)
        total += len(entries)
        if entries:
            output.append(format_catalog(entries, label, rel_dir))

    # Elden ring tools
    elden_root = "C:/scripts/elden_ring_tools"
    if os.path.exists(elden_root):
        for label, rel_dir in [("Elden Ring Tools (root)", ""), ("Elden Ring Tools (test_meshes)", "test_meshes")]:
            full_path = os.path.join(elden_root, rel_dir)
            print(f"Scanning {label}...", file=sys.stderr)
            entries = scan_directory(full_path, elden_root, f"elden_ring_tools/{rel_dir}")
            total += len(entries)
            if entries:
                output.append(format_catalog(entries, label, f"C:/scripts/elden_ring_tools/{rel_dir}"))

    output.append(f"\n---\n**Total: {total} files cataloged**\n")

    catalog_path = os.path.join(repo_root, "scripts", "CATALOG.md")
    with open(catalog_path, 'w', encoding='utf-8') as f:
        f.write("\n".join(output))

    print(f"Wrote {total} entries to {catalog_path}", file=sys.stderr)


if __name__ == "__main__":
    main()
