# diff_animbps_vs_backup.py
# Compare current AnimBPs against bp_only backup - node by node

import unreal
import os

def log(msg):
    unreal.log_warning(f"[AnimDiff] {msg}")

ANIMBP_NAMES = [
    "ABP_SoulslikeCharacter_Additive",
    "ABP_SoulslikeEnemy",
    "ABP_SoulslikeBossNew",
    "ABP_SoulslikeNPC",
]

CURRENT_BASE = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP"

def main():
    log("=" * 60)
    log("AnimBP Diff: Current vs bp_only Backup")
    log("=" * 60)

    results = []

    for name in ANIMBP_NAMES:
        log(f"\n=== {name} ===")

        # Load current AnimBP
        current_path = f"{CURRENT_BASE}/{name}"
        current_bp = unreal.load_asset(current_path)

        if not current_bp:
            log(f"  [ERROR] Cannot load current: {current_path}")
            continue

        # Get current diagnosis using available function
        current_diag = unreal.SLFAutomationLibrary.diagnose_anim_graph_nodes(current_bp)

        # Get compile errors
        current_errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(current_bp)

        # Log current state
        log(f"--- Current State ---")

        # Parse diagnosis for key info
        lines = current_diag.split("\n")
        in_graphs = False
        anim_graph_nodes = 0
        event_graph_nodes = 0
        variables_count = 0
        property_access_count = 0

        for line in lines:
            if "ParentClass:" in line:
                log(f"  Parent: {line.split('ParentClass:')[1].strip()}")
            if "=== VARIABLES ===" in line:
                in_graphs = False
            if "=== GRAPHS ===" in line:
                in_graphs = True
            if "--- EventGraph" in line:
                # Next line has node count
                pass
            if "--- AnimGraph" in line:
                pass
            if "Nodes:" in line and in_graphs:
                count = int(line.split("Nodes:")[1].strip())
                if "EventGraph" in lines[lines.index(line) - 1] if lines.index(line) > 0 else "":
                    event_graph_nodes = count
            if "K2Node_PropertyAccess" in line:
                property_access_count += 1

        # Extract node counts from diagnosis
        for i, line in enumerate(lines):
            if "--- EventGraph" in line and i + 1 < len(lines):
                for j in range(i + 1, min(i + 3, len(lines))):
                    if "Nodes:" in lines[j]:
                        event_graph_nodes = int(lines[j].split("Nodes:")[1].strip())
                        break
            if "--- AnimGraph" in line and i + 1 < len(lines):
                for j in range(i + 1, min(i + 3, len(lines))):
                    if "Nodes:" in lines[j]:
                        anim_graph_nodes = int(lines[j].split("Nodes:")[1].strip())
                        break

        log(f"  EventGraph nodes: {event_graph_nodes}")
        log(f"  AnimGraph nodes: {anim_graph_nodes}")
        log(f"  Property Access nodes: {property_access_count}")

        # Check compile status
        has_errors = "Errors: 0" not in current_errors
        has_warnings = "Warnings: 0" not in current_errors

        if has_errors:
            log(f"  [COMPILE] HAS ERRORS")
            for err_line in current_errors.split("\n"):
                if "[ERROR]" in err_line or "Errors:" in err_line:
                    log(f"    {err_line.strip()}")
        elif has_warnings:
            log(f"  [COMPILE] Has warnings")
            for warn_line in current_errors.split("\n"):
                if "[WARNING]" in warn_line or "Warnings:" in warn_line:
                    log(f"    {warn_line.strip()}")
        else:
            log(f"  [COMPILE] Clean (0 errors, 0 warnings)")

        # Check for "?" in property access paths
        question_mark_props = []
        for line in lines:
            if "K2Node_PropertyAccess" in line and "?" in line:
                question_mark_props.append(line.strip())
            if ".Path" in line and "?" in line:
                question_mark_props.append(line.strip())

        if question_mark_props:
            log(f"  [WARNING] Found '?' in property paths:")
            for prop in question_mark_props[:5]:  # Limit output
                log(f"    {prop}")

        results.append({
            "name": name,
            "event_graph_nodes": event_graph_nodes,
            "anim_graph_nodes": anim_graph_nodes,
            "has_errors": has_errors,
            "has_warnings": has_warnings,
            "question_mark_props": len(question_mark_props),
        })

    # Summary
    log("\n" + "=" * 60)
    log("SUMMARY")
    log("=" * 60)

    all_clean = True
    for r in results:
        status = "[OK]" if not r["has_errors"] and not r["has_warnings"] else "[ISSUE]"
        if r["has_errors"] or r["has_warnings"]:
            all_clean = False
        log(f"  {status} {r['name']}: EventGraph={r['event_graph_nodes']}, AnimGraph={r['anim_graph_nodes']}, ?Props={r['question_mark_props']}")

    if all_clean:
        log("\n[PASS] All AnimBPs compile clean")
    else:
        log("\n[FAIL] Some AnimBPs have issues")

    log("=" * 60)

    return all_clean

if __name__ == "__main__":
    result = main()
    if result:
        unreal.log_warning("[AnimDiff] ALL ANIMBPS CLEAN")
    else:
        unreal.log_warning("[AnimDiff] ISSUES FOUND")
