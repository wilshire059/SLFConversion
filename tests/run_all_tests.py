"""
===============================================================================
SOULSLIKE FRAMEWORK - MASTER TEST RUNNER
===============================================================================

Runs ALL test modules and generates comprehensive report.

Run with:
    UnrealEditor-Cmd.exe Project.uproject -run=pythonscript
        -script="tests/run_all_tests.py" -stdout -unattended -nosplash

Test Modules:
    - test_core_systems.py       : C++ class loading, components, base classes
    - test_gameplay_systems.py   : Combat, stats, inventory, equipment, buffs
    - test_ui_widgets.py         : HUD, menus, notifications, data binding
    - test_ai_systems.py         : AI behavior, combat, sensing, BT tasks
    - test_world_actors.py       : Interactables, ladders, NPCs, projectiles
    - test_utility_systems.py    : Save/load, progress, crafting, radar
    - test_animation_systems.py  : ABPs, notifies, animsets, montages
    - test_interfaces.py         : Blueprint interfaces (BPI_*)

Author: Claude Code Migration System
Version: 1.0.0
"""

import unreal
import sys
import os
from datetime import datetime

# Add tests directory to path
tests_dir = os.path.dirname(os.path.abspath(__file__))
if tests_dir not in sys.path:
    sys.path.insert(0, tests_dir)

# Import framework
from slf_test_framework import log, suite, ctx, TestConfig

# Import all test modules
from test_core_systems import run_all_core_tests
from test_gameplay_systems import run_all_gameplay_tests
from test_ui_widgets import run_all_ui_tests
from test_ai_systems import run_all_ai_tests
from test_world_actors import run_all_world_tests
from test_utility_systems import run_all_utility_tests
from test_animation_systems import run_all_animation_tests
from test_interfaces import run_all_interface_tests


def print_banner():
    """Print test suite banner"""
    log.info("")
    log.info("╔══════════════════════════════════════════════════════════════════════╗")
    log.info("║                                                                      ║")
    log.info("║         SOULSLIKE FRAMEWORK - COMPREHENSIVE TEST SUITE               ║")
    log.info("║                                                                      ║")
    log.info("║    Blueprint to C++ Migration Validation                             ║")
    log.info("║                                                                      ║")
    log.info("╚══════════════════════════════════════════════════════════════════════╝")
    log.info("")
    log.info(f"Started: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    log.info(f"Config: VERBOSE={TestConfig.VERBOSE}, STOP_ON_FAILURE={TestConfig.STOP_ON_FAILURE}")
    log.info("")


def print_test_plan():
    """Print what tests will be run"""
    log.section("TEST PLAN")
    modules = [
        ("Core Systems", "C++ classes, components, base types"),
        ("Gameplay Systems", "Combat, stats, inventory, equipment, buffs"),
        ("UI & Widgets", "HUD, menus, notifications, data binding"),
        ("AI Systems", "Behavior, combat, sensing, BT tasks"),
        ("World Actors", "Interactables, ladders, NPCs, projectiles"),
        ("Utility Systems", "Save/load, progress, crafting, radar"),
        ("Animation Systems", "ABPs, notifies, animsets, montages"),
        ("Interfaces", "Blueprint interfaces (BPI_*)"),
    ]

    for i, (name, desc) in enumerate(modules, 1):
        log.info(f"  {i}. {name}")
        log.info(f"     └─ {desc}")
    log.info("")


def initialize_context():
    """Initialize game context for tests"""
    log.subsection("Initializing Test Context")

    try:
        ctx.initialize()
        log.success("Context initialized")
        return True
    except Exception as e:
        log.error(f"Context initialization failed: {e}")
        return False


def run_all_tests():
    """Run complete test suite"""

    print_banner()
    print_test_plan()

    # Initialize context
    if not initialize_context():
        log.warn("Continuing without full context")

    start_time = datetime.now()

    # ═══════════════════════════════════════════════════════════════════
    # RUN ALL TEST MODULES
    # ═══════════════════════════════════════════════════════════════════

    try:
        # 1. Core Systems
        log.info("")
        log.info("▶ Running Core Systems Tests...")
        run_all_core_tests()

        # 2. Gameplay Systems
        log.info("")
        log.info("▶ Running Gameplay Systems Tests...")
        run_all_gameplay_tests()

        # 3. UI & Widgets
        log.info("")
        log.info("▶ Running UI & Widget Tests...")
        run_all_ui_tests()

        # 4. AI Systems
        log.info("")
        log.info("▶ Running AI Systems Tests...")
        run_all_ai_tests()

        # 5. World Actors
        log.info("")
        log.info("▶ Running World Actor Tests...")
        run_all_world_tests()

        # 6. Utility Systems
        log.info("")
        log.info("▶ Running Utility Systems Tests...")
        run_all_utility_tests()

        # 7. Animation Systems
        log.info("")
        log.info("▶ Running Animation Systems Tests...")
        run_all_animation_tests()

        # 8. Interfaces
        log.info("")
        log.info("▶ Running Interface Tests...")
        run_all_interface_tests()

    except Exception as e:
        log.error(f"Test execution error: {e}")
        import traceback
        log.error(traceback.format_exc())

    # ═══════════════════════════════════════════════════════════════════
    # GENERATE REPORT
    # ═══════════════════════════════════════════════════════════════════

    end_time = datetime.now()
    duration = (end_time - start_time).total_seconds()

    # Print summary
    suite.print_summary()

    # Print timing
    log.info("")
    log.info(f"Total Duration: {duration:.2f} seconds")
    log.info(f"Completed: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")

    # Generate detailed report
    generate_detailed_report(duration)

    # Return exit code
    return 0 if suite.total_failed == 0 else 1


def generate_detailed_report(duration):
    """Generate detailed test report"""
    log.info("")
    log.section("DETAILED CATEGORY BREAKDOWN")

    for cat_name, category in sorted(suite.categories.items()):
        total = category.total_count
        passed = category.passed_count
        failed = category.failed_count
        skipped = category.skipped_count
        rate = category.success_rate

        status = "✅" if failed == 0 else "⚠️" if failed < passed else "❌"

        log.info(f"\n{status} {cat_name}")
        log.info(f"   Total: {total} | Passed: {passed} | Failed: {failed} | Skipped: {skipped} | Rate: {rate:.0f}%")

        # List failures
        for result in category.results:
            if not result.passed and not result.skipped:
                log.info(f"   ├─ ❌ {result.name}")
                if result.error_message:
                    log.info(f"   │     └─ {result.error_message}")

        # List skipped
        for result in category.results:
            if result.skipped:
                log.info(f"   ├─ ⏭️  {result.name}")
                if result.error_message:
                    log.info(f"   │     └─ {result.error_message}")

    # Final statistics
    log.info("")
    log.section("FINAL STATISTICS")

    total = suite.total_passed + suite.total_failed
    rate = (suite.total_passed / total * 100) if total > 0 else 0

    log.info(f"Categories Tested: {len(suite.categories)}")
    log.info(f"Total Tests: {suite.total_tests}")
    log.info(f"Passed: {suite.total_passed}")
    log.info(f"Failed: {suite.total_failed}")
    log.info(f"Skipped: {suite.total_skipped}")
    log.info(f"Success Rate: {rate:.1f}%")
    log.info(f"Duration: {duration:.2f}s")

    if suite.total_failed == 0:
        log.info("")
        log.success("═══════════════════════════════════════════════════════════════")
        log.success("  🎮 ALL TESTS PASSED! FRAMEWORK VALIDATION SUCCESSFUL!")
        log.success("═══════════════════════════════════════════════════════════════")
    else:
        log.info("")
        log.warn("═══════════════════════════════════════════════════════════════")
        log.warn(f"  ⚠️ {suite.total_failed} TESTS REQUIRE ATTENTION")
        log.warn("═══════════════════════════════════════════════════════════════")

        # Prioritized action items
        log.info("")
        log.info("PRIORITY ACTION ITEMS:")
        priority_categories = [
            "Game Framework", "Player Components", "AI Components",
            "Combat System", "Widget Classes"
        ]

        action_num = 1
        for cat_name in priority_categories:
            if cat_name in suite.categories:
                category = suite.categories[cat_name]
                if category.failed_count > 0:
                    for result in category.results:
                        if not result.passed and not result.skipped:
                            log.info(f"  {action_num}. [{cat_name}] {result.name}")
                            log.info(f"     └─ {result.error_message}")
                            action_num += 1


def run_quick_tests():
    """Run subset of critical tests only"""
    log.info("")
    log.info("╔══════════════════════════════════════════════════════════════════════╗")
    log.info("║                    QUICK TEST MODE                                   ║")
    log.info("╚══════════════════════════════════════════════════════════════════════╝")
    log.info("")

    # Only run core and gameplay tests
    run_all_core_tests()
    run_all_gameplay_tests()

    suite.print_summary()


def run_category(category_name):
    """Run tests for a specific category"""
    category_map = {
        "core": run_all_core_tests,
        "gameplay": run_all_gameplay_tests,
        "ui": run_all_ui_tests,
        "ai": run_all_ai_tests,
        "world": run_all_world_tests,
        "utility": run_all_utility_tests,
        "animation": run_all_animation_tests,
        "interfaces": run_all_interface_tests,
    }

    if category_name.lower() in category_map:
        log.info(f"Running {category_name} tests only...")
        category_map[category_name.lower()]()
        suite.print_summary()
    else:
        log.error(f"Unknown category: {category_name}")
        log.info(f"Available: {', '.join(category_map.keys())}")


# ═══════════════════════════════════════════════════════════════════════════════
# MAIN ENTRY POINT
# ═══════════════════════════════════════════════════════════════════════════════

if __name__ == "__main__":
    # Check for command line args
    if len(sys.argv) > 1:
        arg = sys.argv[1].lower()
        if arg == "quick":
            run_quick_tests()
        elif arg in ["core", "gameplay", "ui", "ai", "world", "utility", "animation", "interfaces"]:
            run_category(arg)
        else:
            log.warn(f"Unknown argument: {arg}")
            log.info("Usage: run_all_tests.py [quick|core|gameplay|ui|ai|world|utility|animation]")
    else:
        # Run full test suite
        exit_code = run_all_tests()
        # Note: Can't call sys.exit() in Unreal Python, just log result
        if exit_code != 0:
            log.warn(f"Tests completed with failures (exit code: {exit_code})")
