#!/usr/bin/env python3
"""
SoulslikeFramework Migration Healthcheck Script
================================================
Validates C++ migration completeness against Blueprint JSON exports.

Usage: python healthcheck_migration.py [--verbose] [--item NAME]

This script:
1. Reads JSON exports from BlueprintDNA_v2
2. Finds corresponding C++ files (.h and .cpp)
3. Compares content and flags potential issues
4. Generates a report for migration certification

Red Flags Detected:
- Missing C++ files for JSON exports
- Significant line count disparities
- Missing variables, functions, dispatchers, etc.
- Logic items in JSON not found in C++
"""

import os
import re
import json
import glob
import argparse
from pathlib import Path
from dataclasses import dataclass, field
from typing import Dict, List, Set, Tuple, Optional

# Paths
JSON_EXPORT_PATH = r"C:\scripts\SLFConversion\Exports\BlueprintDNA_v2"
CPP_SOURCE_PATH = r"C:\scripts\SLFConversion\Source\SLFConversion"
REPORT_OUTPUT = r"C:\scripts\SLFConversion\HEALTHCHECK_REPORT.md"

# Mapping from Blueprint name to C++ class name patterns
# This helps find the right C++ files
NAME_MAPPINGS = {
    # Components
    "AC_StatManager": ["StatManagerComponent"],
    "AC_ActionManager": ["ActionManagerComponent"],
    "AC_BuffManager": ["BuffManagerComponent"],
    "AC_CombatManager": ["CombatManagerComponent"],
    "AC_EquipmentManager": ["EquipmentManagerComponent"],
    "AC_InputBuffer": ["InputBufferComponent"],
    "AC_InteractionManager": ["InteractionManagerComponent"],
    "AC_InventoryManager": ["InventoryManagerComponent"],
    "AC_StatusEffectManager": ["StatusEffectManagerComponent"],
    "AC_AI_CombatManager": ["AICombatManagerComponent"],
    # Characters
    "B_BaseCharacter": ["SLFBaseCharacter"],
    "B_Soulslike_Character": ["SLFSoulslikeCharacter"],
    "B_Soulslike_Enemy": ["SLFSoulslikeEnemy"],
    "B_Soulslike_Boss": ["SLFSoulslikeBoss"],
    "B_Soulslike_NPC": ["SLFSoulslikeNPC"],
    # Base Classes
    "B_Stat": ["SLFStatBase"],
    "B_Action": ["SLFActionBase"],
    "B_Buff": ["SLFBuffBase"],
    "B_Item": ["SLFItemBase"],
    "B_StatusEffect": ["SLFStatusEffectBase"],
    "B_Interactable": ["SLFInteractableBase"],
    "B_BaseProjectile": ["SLFProjectileBase"],
    # Animation Notifies - use direct names
    "AN_": ["AN_", "SLFAnimNotify"],
    "ANS_": ["ANS_", "SLFAnimNotifyState"],
    # AI Tasks/Services - use direct names
    "BTT_": ["BTT_", "SLFBTTask"],
    "BTS_": ["BTS_", "SLFBTService"],
}


@dataclass
class JsonItem:
    """Represents a parsed JSON export item"""
    name: str
    path: str
    category: str
    line_count: int
    variables: List[str] = field(default_factory=list)
    functions: List[str] = field(default_factory=list)
    event_dispatchers: List[str] = field(default_factory=list)
    interface_functions: List[str] = field(default_factory=list)
    graphs: List[str] = field(default_factory=list)
    macros: List[str] = field(default_factory=list)
    local_variables: List[str] = field(default_factory=list)
    timelines: List[str] = field(default_factory=list)
    components: List[str] = field(default_factory=list)
    structs: List[str] = field(default_factory=list)
    enums: List[str] = field(default_factory=list)
    anim_graphs: List[str] = field(default_factory=list)


@dataclass
class CppItem:
    """Represents C++ implementation files"""
    name: str
    header_path: Optional[str] = None
    source_path: Optional[str] = None
    header_lines: int = 0
    source_lines: int = 0
    total_lines: int = 0
    uproperties: List[str] = field(default_factory=list)
    ufunctions: List[str] = field(default_factory=list)
    delegates: List[str] = field(default_factory=list)
    interface_impls: List[str] = field(default_factory=list)
    # NEW: Stub detection fields
    has_todo_stubs: bool = False
    todo_count: int = 0
    stub_functions: List[str] = field(default_factory=list)


@dataclass
class HealthcheckResult:
    """Result of healthcheck for one item"""
    json_item: JsonItem
    cpp_item: Optional[CppItem]
    status: str  # "PASS", "WARN", "FAIL", "MISSING"
    issues: List[str] = field(default_factory=list)
    line_ratio: float = 0.0


def parse_json_export(json_path: str) -> Optional[JsonItem]:
    """Parse a Blueprint JSON export and extract key items"""
    try:
        with open(json_path, 'r', encoding='utf-8', errors='replace') as f:
            content = f.read()
            line_count = content.count('\n')
            data = json.loads(content)
    except (json.JSONDecodeError, IOError) as e:
        return None

    name = Path(json_path).stem
    category = Path(json_path).parent.name

    item = JsonItem(
        name=name,
        path=json_path,
        category=category,
        line_count=line_count
    )

    # Extract variables
    if 'Variables' in data:
        vars_data = data['Variables']
        if isinstance(vars_data, dict) and 'List' in vars_data:
            for var in vars_data['List']:
                if isinstance(var, dict) and 'Name' in var:
                    item.variables.append(var['Name'])
        elif isinstance(vars_data, list):
            for var in vars_data:
                if isinstance(var, dict) and 'Name' in var:
                    item.variables.append(var['Name'])

    # Extract functions
    if 'Functions' in data:
        funcs = data['Functions']
        if isinstance(funcs, list):
            for func in funcs:
                if isinstance(func, dict) and 'Name' in func:
                    item.functions.append(func['Name'])
        elif isinstance(funcs, dict):
            for name in funcs.keys():
                item.functions.append(name)

    # Extract graphs (function implementations)
    if 'Graphs' in data:
        graphs = data['Graphs']
        if isinstance(graphs, dict):
            for graph_name in graphs.keys():
                item.graphs.append(graph_name)
        elif isinstance(graphs, list):
            for graph in graphs:
                if isinstance(graph, dict) and 'Name' in graph:
                    item.graphs.append(graph['Name'])

    # Extract event dispatchers
    if 'EventDispatchers' in data:
        dispatchers = data['EventDispatchers']
        if isinstance(dispatchers, dict) and 'List' in dispatchers:
            for d in dispatchers['List']:
                if isinstance(d, dict) and 'Name' in d:
                    item.event_dispatchers.append(d['Name'])
        elif isinstance(dispatchers, list):
            for d in dispatchers:
                if isinstance(d, dict) and 'Name' in d:
                    item.event_dispatchers.append(d['Name'])

    # Extract interface functions
    if 'Interfaces' in data:
        interfaces = data['Interfaces']
        if isinstance(interfaces, list):
            for iface in interfaces:
                if isinstance(iface, dict):
                    for func_name in iface.get('Functions', []):
                        item.interface_functions.append(func_name)
    if 'ImplementedInterfaces' in data:
        for iface in data['ImplementedInterfaces']:
            if isinstance(iface, dict):
                item.interface_functions.append(iface.get('Name', ''))

    # Extract macros
    if 'MacroGraphs' in data:
        macros = data['MacroGraphs']
        if isinstance(macros, list):
            for macro in macros:
                if isinstance(macro, dict) and 'Name' in macro:
                    item.macros.append(macro['Name'])

    # Extract timelines
    if 'Timelines' in data:
        timelines = data['Timelines']
        if isinstance(timelines, list):
            for tl in timelines:
                if isinstance(tl, dict) and 'Name' in tl:
                    item.timelines.append(tl['Name'])

    # Extract components
    if 'Components' in data:
        components = data['Components']
        if isinstance(components, list):
            for comp in components:
                if isinstance(comp, dict) and 'Name' in comp:
                    item.components.append(comp['Name'])

    # Extract AnimGraph (for AnimBlueprints)
    if 'AnimGraph' in data:
        item.anim_graphs.append('AnimGraph')
    if 'StateMachines' in data:
        for sm in data.get('StateMachines', []):
            if isinstance(sm, dict) and 'Name' in sm:
                item.anim_graphs.append(sm['Name'])

    return item


def find_cpp_files(item_name: str) -> Tuple[Optional[str], Optional[str]]:
    """Find matching C++ header and source files for a Blueprint"""
    # Try direct name match first
    patterns_to_try = [item_name]

    # Add mapped names
    for prefix, mappings in NAME_MAPPINGS.items():
        if item_name.startswith(prefix) or item_name == prefix.rstrip('_'):
            patterns_to_try.extend(mappings)
            # Also try the suffix part
            suffix = item_name[len(prefix):] if item_name.startswith(prefix) else item_name
            for mapping in mappings:
                if not mapping.endswith('_'):
                    patterns_to_try.append(f"{mapping}{suffix}")

    # Also try SLF prefix variations
    patterns_to_try.append(f"SLF{item_name}")
    patterns_to_try.append(f"U{item_name}")
    patterns_to_try.append(f"A{item_name}")

    header_path = None
    source_path = None

    for pattern in patterns_to_try:
        # Search for header
        headers = glob.glob(os.path.join(CPP_SOURCE_PATH, "**", f"{pattern}.h"), recursive=True)
        if headers:
            header_path = headers[0]

        # Search for source
        sources = glob.glob(os.path.join(CPP_SOURCE_PATH, "**", f"{pattern}.cpp"), recursive=True)
        if sources:
            source_path = sources[0]

        if header_path or source_path:
            break

    return header_path, source_path


def parse_cpp_files(name: str, header_path: Optional[str], source_path: Optional[str]) -> CppItem:
    """Parse C++ files and extract declarations"""
    item = CppItem(name=name, header_path=header_path, source_path=source_path)

    header_content = ""
    source_content = ""

    if header_path and os.path.exists(header_path):
        with open(header_path, 'r', encoding='utf-8', errors='replace') as f:
            header_content = f.read()
            item.header_lines = header_content.count('\n')

    if source_path and os.path.exists(source_path):
        with open(source_path, 'r', encoding='utf-8', errors='replace') as f:
            source_content = f.read()
            item.source_lines = source_content.count('\n')

    # NEW: Detect stub implementations by looking for ANY TODO comments
    item.has_todo_stubs = False
    item.todo_count = 0
    item.stub_functions = []

    # Match ANY TODO comment - comprehensive detection
    todo_pattern = r'//\s*TODO'
    matches = re.findall(todo_pattern, source_content, re.IGNORECASE)
    item.todo_count = len(matches)

    if item.todo_count > 0:
        item.has_todo_stubs = True

    # Find functions that are likely stubs (only have Super:: call and/or logging)
    # Pattern: function body with only Super call, return, and maybe UE_LOG
    stub_pattern = r'void\s+\w+::(\w+)\s*\([^)]*\)[^{]*\{([^}]{0,500})\}'
    for match in re.finditer(stub_pattern, source_content):
        func_name = match.group(1)
        func_body = match.group(2)
        # Check if body is basically empty (only Super::, return, UE_LOG, or TODO)
        stripped = func_body.strip()
        lines = [l.strip() for l in stripped.split('\n') if l.strip() and not l.strip().startswith('//')]
        # If function only has Super call, early return check, or logging - it's a stub
        non_stub_indicators = ['Execute_', 'IBPI_', 'GetClass()->', 'if (', 'for (', 'while (']
        has_real_logic = any(indicator in func_body for indicator in non_stub_indicators)
        if not has_real_logic and len(lines) <= 5:
            if 'TODO' in func_body or (len(lines) <= 2 and 'Super::' in func_body):
                item.stub_functions.append(func_name)

    item.total_lines = item.header_lines + item.source_lines
    full_content = header_content + "\n" + source_content

    # Extract UPROPERTY declarations
    # Handle nested parentheses in meta specifiers like: Category = "Config|Collision (Adjust to Your Needs)"
    # Use a simpler approach: find UPROPERTY, then look for the variable declaration on the next line
    uproperty_pattern = r'UPROPERTY\s*\([^;]*?\)\s*\n?\s*(\w+[\w\s\*<>,]*)\s+(\w+)\s*[;=]'
    for match in re.finditer(uproperty_pattern, header_content):
        item.uproperties.append(match.group(2))

    # Also try a more relaxed pattern for multi-line declarations
    # Look for lines that start with known types and follow UPROPERTY
    lines = header_content.split('\n')
    in_uproperty = False
    for i, line in enumerate(lines):
        if 'UPROPERTY' in line:
            in_uproperty = True
        elif in_uproperty:
            # Try to extract variable name from this line
            type_var_match = re.match(r'\s*(?:class\s+|struct\s+)?(\w+[\w:]*(?:<[^>]+>)?)\s*[\*&]?\s*(\w+)\s*[;=]', line)
            if type_var_match:
                var_name = type_var_match.group(2)
                if var_name and var_name not in item.uproperties:
                    item.uproperties.append(var_name)
            in_uproperty = False

    # Extract UFUNCTION declarations
    ufunction_pattern = r'UFUNCTION\s*\([^)]*\)\s*\n?\s*(?:virtual\s+)?(\w+[\w\s\*<>,]*)\s+(\w+)\s*\('
    for match in re.finditer(ufunction_pattern, full_content):
        item.ufunctions.append(match.group(2))

    # Extract delegate declarations
    delegate_pattern = r'DECLARE_DYNAMIC_MULTICAST_DELEGATE[^;]*\(\s*(\w+)'
    for match in re.finditer(delegate_pattern, full_content):
        item.delegates.append(match.group(1))

    # Extract interface implementations (_Implementation suffix)
    impl_pattern = r'(\w+)_Implementation\s*\('
    for match in re.finditer(impl_pattern, source_content):
        item.interface_impls.append(match.group(1))

    return item


def normalize_bp_name(name: str) -> List[str]:
    """
    Convert Blueprint variable name to possible C++ equivalents.
    Blueprint uses names like "BarAtZero?", "Is Dead?", "10MWall", "Direction(Angle)"
    C++ uses bBarAtZero, bIsDead, TenMWall or _10MWall, Direction, etc.
    Returns a list of possible C++ names (lowercase).
    """
    variants = []

    # Handle trailing parenthetical descriptions like "Direction(Angle)" -> "Direction"
    import re
    stripped_parens = re.sub(r'\([^)]*\)$', '', name).strip()
    if stripped_parens:
        variants.append(stripped_parens.lower())

    # Strip special characters and normalize
    clean = name.replace('?', '').replace(' ', '').replace('-', '').replace('(', '').replace(')', '')
    variants.append(clean.lower())

    # Also try hyphen-to-underscore (Sub-title -> Sub_title)
    underscore_variant = name.replace('-', '_').replace('?', '').replace(' ', '').replace('(', '').replace(')', '')
    variants.append(underscore_variant.lower())

    # Boolean prefix variant (BarAtZero? -> bBarAtZero)
    if name.endswith('?') or name.lower().startswith('is') or name.lower().startswith('has'):
        variants.append(f"b{clean}".lower())
        # Without leading "Is" for bools like "Is Dead?" -> bDead
        if clean.lower().startswith('is'):
            variants.append(f"b{clean[2:]}".lower())

    # Handle spaces (e.g., "Title Text" -> "TitleText" or "titletext")
    no_space = name.replace(' ', '')
    variants.append(no_space.lower())
    variants.append(f"b{no_space}".lower())

    # Handle numbers at start (e.g., "10MWall" -> "Wall10M" or "_10MWall")
    if name and name[0].isdigit():
        # Try underscore prefix
        variants.append(f"_{clean}".lower())
        # Try moving numbers to end
        import re
        match = re.match(r'^(\d+[A-Za-z]*)(.+)$', clean)
        if match:
            variants.append(f"{match.group(2)}{match.group(1)}".lower())

    # Handle underscore variants (ISM_ Curve -> ISM_Curve or ismcurve)
    underscore_clean = name.replace(' ', '')
    variants.append(underscore_clean.lower())

    # Also add just the base part after underscore
    if '_' in name:
        parts = name.split('_')
        for part in parts:
            if part.strip():
                variants.append(part.strip().lower())

    return list(set(variants))  # Remove duplicates


def check_item(json_item: JsonItem, cpp_item: Optional[CppItem]) -> HealthcheckResult:
    """Check migration completeness for an item"""
    result = HealthcheckResult(json_item=json_item, cpp_item=cpp_item, status="PASS")

    if cpp_item is None or (cpp_item.header_path is None and cpp_item.source_path is None):
        result.status = "MISSING"
        result.issues.append(f"No C++ files found for {json_item.name}")
        return result

    # Calculate line ratio (rough indicator)
    if json_item.line_count > 0:
        result.line_ratio = cpp_item.total_lines / json_item.line_count

    # Check for significant line count disparity
    # JSON is VERY verbose with node positions, pins, connections, etc.
    # Only flag as warning if ratio is extremely low (< 0.5%) for large files
    # This is just a heuristic - actual validation is done via missing items
    if result.line_ratio < 0.005 and json_item.line_count > 5000:
        result.issues.append(f"LOW LINE RATIO: {result.line_ratio:.2%} (JSON: {json_item.line_count}, C++: {cpp_item.total_lines})")

    # Check for missing variables
    cpp_props_lower = {p.lower() for p in cpp_item.uproperties}
    for var in json_item.variables:
        # Get all possible C++ name variants
        variants = normalize_bp_name(var)
        # Check if any variant exists in C++ properties
        found = any(v in cpp_props_lower for v in variants)
        if not found:
            result.issues.append(f"MISSING VARIABLE: {var}")

    # Check for missing functions
    cpp_funcs_lower = {f.lower() for f in cpp_item.ufunctions}
    cpp_impls_lower = {f.lower() for f in cpp_item.interface_impls}
    all_cpp_funcs = cpp_funcs_lower | cpp_impls_lower

    for func in json_item.functions:
        func_lower = func.lower()
        if func_lower not in all_cpp_funcs and f"{func_lower}_implementation" not in cpp_impls_lower:
            # Skip common UE functions that don't need explicit declaration
            if func_lower not in ['beginplay', 'tick', 'endplay', 'construct']:
                result.issues.append(f"MISSING FUNCTION: {func}")

    # Check for missing event dispatchers
    cpp_delegates_lower = {d.lower() for d in cpp_item.delegates}
    for dispatcher in json_item.event_dispatchers:
        dispatcher_lower = dispatcher.lower()
        # Check various naming conventions
        found = any(dispatcher_lower in d for d in cpp_delegates_lower)
        if not found:
            result.issues.append(f"MISSING DISPATCHER: {dispatcher}")

    # Check for missing interface functions
    for iface_func in json_item.interface_functions:
        if iface_func and iface_func.lower() not in cpp_impls_lower:
            result.issues.append(f"MISSING INTERFACE FUNC: {iface_func}")

    # Check for missing graphs (these should have _Implementation or be in source)
    for graph in json_item.graphs:
        graph_lower = graph.lower()
        if graph_lower not in ['eventgraph', 'constructionscript']:
            if graph_lower not in all_cpp_funcs and f"{graph_lower}_implementation" not in cpp_impls_lower:
                result.issues.append(f"MISSING GRAPH: {graph}")

    # NEW: Check for stub implementations (TODO comments)
    if cpp_item.has_todo_stubs:
        result.issues.append(f"STUB IMPLEMENTATION: {cpp_item.todo_count} TODO comments found")
        for stub_func in cpp_item.stub_functions[:5]:  # Limit to 5
            result.issues.append(f"STUB FUNCTION: {stub_func}()")

    # Determine overall status
    critical_issues = [i for i in result.issues if 'MISSING' in i]
    stub_issues = [i for i in result.issues if 'STUB' in i]

    # STUBS are now FAIL status - they need to be migrated
    if len(stub_issues) > 0:
        result.status = "FAIL"  # Changed from WARN - stubs must be fixed
    elif len(critical_issues) > 5:
        result.status = "FAIL"
    elif len(critical_issues) > 0:
        result.status = "WARN"
    elif 'LOW LINE RATIO' in str(result.issues):
        result.status = "WARN"

    return result


def generate_report(results: List[HealthcheckResult], output_path: str) -> str:
    """Generate a markdown report of healthcheck results"""
    lines = [
        "# SoulslikeFramework Migration Healthcheck Report",
        "",
        f"Generated: {Path(output_path).stat().st_mtime if os.path.exists(output_path) else 'N/A'}",
        "",
        "## Summary",
        "",
    ]

    # Count by status
    status_counts = {"PASS": 0, "WARN": 0, "FAIL": 0, "MISSING": 0}
    for r in results:
        status_counts[r.status] += 1

    total = len(results)
    lines.append(f"| Status | Count | Percentage |")
    lines.append(f"|--------|-------|------------|")
    for status, count in status_counts.items():
        pct = (count / total * 100) if total > 0 else 0
        emoji = {"PASS": "✅", "WARN": "⚠️", "FAIL": "❌", "MISSING": "🔴"}[status]
        lines.append(f"| {emoji} {status} | {count} | {pct:.1f}% |")
    lines.append("")

    # Critical failures
    failures = [r for r in results if r.status in ["FAIL", "MISSING"]]
    if failures:
        lines.append("## Critical Issues (FAIL/MISSING)")
        lines.append("")
        for r in failures[:50]:  # Limit to 50
            lines.append(f"### {r.json_item.name} [{r.status}]")
            lines.append(f"- Category: {r.json_item.category}")
            lines.append(f"- JSON Lines: {r.json_item.line_count}")
            if r.cpp_item:
                lines.append(f"- C++ Lines: {r.cpp_item.total_lines}")
            lines.append(f"- Issues:")
            for issue in r.issues[:10]:  # Limit issues shown
                lines.append(f"  - {issue}")
            lines.append("")

    # Warnings
    warnings = [r for r in results if r.status == "WARN"]
    if warnings:
        lines.append("## Warnings")
        lines.append("")
        for r in warnings[:30]:  # Limit to 30
            lines.append(f"### {r.json_item.name}")
            lines.append(f"- Line Ratio: {r.line_ratio:.2%}")
            for issue in r.issues[:5]:
                lines.append(f"  - {issue}")
            lines.append("")

    # Detailed item list
    lines.append("## All Items by Category")
    lines.append("")

    by_category = {}
    for r in results:
        cat = r.json_item.category
        if cat not in by_category:
            by_category[cat] = []
        by_category[cat].append(r)

    for category, items in sorted(by_category.items()):
        lines.append(f"### {category}")
        lines.append("")
        lines.append("| Item | Status | JSON Lines | C++ Lines | Ratio | Issues |")
        lines.append("|------|--------|------------|-----------|-------|--------|")
        for r in sorted(items, key=lambda x: x.json_item.name):
            cpp_lines = r.cpp_item.total_lines if r.cpp_item else 0
            issue_count = len(r.issues)
            emoji = {"PASS": "✅", "WARN": "⚠️", "FAIL": "❌", "MISSING": "🔴"}[r.status]
            lines.append(f"| {r.json_item.name} | {emoji} | {r.json_item.line_count} | {cpp_lines} | {r.line_ratio:.1%} | {issue_count} |")
        lines.append("")

    report = "\n".join(lines)

    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(report)

    return report


def run_healthcheck(verbose: bool = False, single_item: Optional[str] = None) -> List[HealthcheckResult]:
    """Run the full healthcheck"""
    results = []

    # Find all JSON exports
    json_files = glob.glob(os.path.join(JSON_EXPORT_PATH, "**", "*.json"), recursive=True)

    # Filter to logic-bearing items (Components, Blueprints, Animation, AI, etc.)
    logic_categories = ['Component', 'Blueprint', 'Animation', 'AI', 'AnimBlueprint',
                       'Interface', 'GameFramework', 'WidgetBlueprint']

    for json_path in json_files:
        if '_manifest.json' in json_path:
            continue

        category = Path(json_path).parent.name
        item_name = Path(json_path).stem

        # Filter by single item if specified
        if single_item and item_name != single_item:
            continue

        # Skip non-logic categories unless checking a specific item
        if not single_item and category not in logic_categories:
            # But include items that start with known prefixes
            if not any(item_name.startswith(p) for p in ['AC_', 'AN_', 'ANS_', 'BTT_', 'BTS_', 'B_', 'BPI_', 'W_', 'ABP_']):
                continue

        if verbose:
            print(f"Checking: {item_name}")

        # Parse JSON
        json_item = parse_json_export(json_path)
        if not json_item:
            continue

        # Find and parse C++ files
        header_path, source_path = find_cpp_files(item_name)
        cpp_item = None
        if header_path or source_path:
            cpp_item = parse_cpp_files(item_name, header_path, source_path)

        # Run healthcheck
        result = check_item(json_item, cpp_item)
        results.append(result)

        if verbose and result.issues:
            print(f"  Status: {result.status}")
            for issue in result.issues[:3]:
                print(f"    - {issue}")

    return results


def main():
    parser = argparse.ArgumentParser(description='SoulslikeFramework Migration Healthcheck')
    parser.add_argument('--verbose', '-v', action='store_true', help='Verbose output')
    parser.add_argument('--item', '-i', type=str, help='Check single item by name')
    parser.add_argument('--no-report', action='store_true', help='Skip report generation')
    args = parser.parse_args()

    print("=" * 60)
    print("SoulslikeFramework Migration Healthcheck")
    print("=" * 60)
    print(f"JSON Source: {JSON_EXPORT_PATH}")
    print(f"C++ Source: {CPP_SOURCE_PATH}")
    print()

    results = run_healthcheck(verbose=args.verbose, single_item=args.item)

    # Summary
    status_counts = {"PASS": 0, "WARN": 0, "FAIL": 0, "MISSING": 0}
    for r in results:
        status_counts[r.status] += 1

    print()
    print("=" * 60)
    print("SUMMARY")
    print("=" * 60)
    for status, count in status_counts.items():
        print(f"  {status}: {count}")
    print(f"  TOTAL: {len(results)}")

    if not args.no_report:
        print()
        print(f"Generating report: {REPORT_OUTPUT}")
        generate_report(results, REPORT_OUTPUT)
        print("Report generated!")

    # Return exit code based on results
    if status_counts["FAIL"] > 0 or status_counts["MISSING"] > 10:
        return 1
    return 0


if __name__ == "__main__":
    exit(main())
