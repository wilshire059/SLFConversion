"""
═══════════════════════════════════════════════════════════════════════════════
SOULSLIKE FRAMEWORK - COMPREHENSIVE TEST FRAMEWORK
═══════════════════════════════════════════════════════════════════════════════

A complete headless testing system for validating all SLF systems including:
- Character spawning and movement
- All component managers (player and AI)
- Combat system (attacks, guards, dodges, combos)
- Item system (pickup, inventory, equipment)
- UI/Widget state verification
- Animation triggers
- AI behavior
- Save/Load system
- NPC dialog and vendor systems

Run with:
    UnrealEditor-Cmd.exe Project.uproject -run=pythonscript
        -script="tests/slf_test_framework.py" -stdout -unattended -nosplash

Author: Claude Code Migration System
Version: 1.0.0
"""

import unreal
import sys
import traceback
from datetime import datetime
from enum import Enum
from typing import Optional, List, Dict, Any, Callable, Tuple

# ═══════════════════════════════════════════════════════════════════════════════
# CONFIGURATION
# ═══════════════════════════════════════════════════════════════════════════════

class TestConfig:
    """Global test configuration"""
    VERBOSE = True  # Print detailed debug info
    STOP_ON_FAILURE = False  # Stop test suite on first failure
    LOG_TO_FILE = True  # Also write to log file
    DEFAULT_LEVEL = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
    TIMEOUT_SECONDS = 30


# ═══════════════════════════════════════════════════════════════════════════════
# LOGGING & DEBUG SYSTEM
# ═══════════════════════════════════════════════════════════════════════════════

class LogLevel(Enum):
    DEBUG = 0
    INFO = 1
    WARN = 2
    ERROR = 3
    SUCCESS = 4
    FAIL = 5


class TestLogger:
    """Comprehensive logging with formatting and categorization"""

    COLORS = {
        LogLevel.DEBUG: "",
        LogLevel.INFO: "",
        LogLevel.WARN: "⚠️ ",
        LogLevel.ERROR: "❌ ",
        LogLevel.SUCCESS: "✅ ",
        LogLevel.FAIL: "❌ ",
    }

    def __init__(self, name: str = "SLF-TEST"):
        self.name = name
        self.indent_level = 0
        self.logs = []

    def _format(self, level: LogLevel, msg: str) -> str:
        indent = "  " * self.indent_level
        prefix = self.COLORS.get(level, "")
        timestamp = datetime.now().strftime("%H:%M:%S.%f")[:-3]
        return f"[{self.name}][{timestamp}] {indent}{prefix}{msg}"

    def _log(self, level: LogLevel, msg: str):
        formatted = self._format(level, msg)
        self.logs.append((level, formatted))
        print(formatted)
        unreal.log(formatted)

    def debug(self, msg: str):
        if TestConfig.VERBOSE:
            self._log(LogLevel.DEBUG, f"  {msg}")

    def info(self, msg: str):
        self._log(LogLevel.INFO, msg)

    def warn(self, msg: str):
        self._log(LogLevel.WARN, msg)

    def error(self, msg: str):
        self._log(LogLevel.ERROR, msg)

    def success(self, msg: str):
        self._log(LogLevel.SUCCESS, msg)

    def fail(self, msg: str):
        self._log(LogLevel.FAIL, msg)

    def section(self, title: str):
        self._log(LogLevel.INFO, "")
        self._log(LogLevel.INFO, "═" * 70)
        self._log(LogLevel.INFO, f"  {title}")
        self._log(LogLevel.INFO, "═" * 70)

    def subsection(self, title: str):
        self._log(LogLevel.INFO, "")
        self._log(LogLevel.INFO, f"─── {title} ───")

    def indent(self):
        self.indent_level += 1
        return self

    def dedent(self):
        self.indent_level = max(0, self.indent_level - 1)
        return self

    def __enter__(self):
        self.indent()
        return self

    def __exit__(self, *args):
        self.dedent()


# Global logger instance
log = TestLogger()


# ═══════════════════════════════════════════════════════════════════════════════
# TEST RESULT TRACKING
# ═══════════════════════════════════════════════════════════════════════════════

class TestResult:
    """Individual test result with metadata"""

    def __init__(self, name: str, category: str):
        self.name = name
        self.category = category
        self.passed = False
        self.skipped = False
        self.error_message = ""
        self.debug_info = {}
        self.duration_ms = 0

    def set_passed(self):
        self.passed = True
        return self

    def set_failed(self, message: str):
        self.passed = False
        self.error_message = message
        return self

    def set_skipped(self, reason: str):
        self.skipped = True
        self.error_message = reason
        return self

    def add_debug(self, key: str, value: Any):
        self.debug_info[key] = value
        return self


class TestCategory:
    """Group of related tests"""

    def __init__(self, name: str, description: str = ""):
        self.name = name
        self.description = description
        self.results: List[TestResult] = []

    def add_result(self, result: TestResult):
        self.results.append(result)

    @property
    def passed_count(self) -> int:
        return sum(1 for r in self.results if r.passed)

    @property
    def failed_count(self) -> int:
        return sum(1 for r in self.results if not r.passed and not r.skipped)

    @property
    def skipped_count(self) -> int:
        return sum(1 for r in self.results if r.skipped)

    @property
    def total_count(self) -> int:
        return len(self.results)

    @property
    def success_rate(self) -> float:
        valid = self.passed_count + self.failed_count
        if valid == 0:
            return 100.0
        return (self.passed_count / valid) * 100


class TestSuite:
    """Complete test suite with all categories"""

    def __init__(self, name: str = "SLF Complete Test Suite"):
        self.name = name
        self.categories: Dict[str, TestCategory] = {}
        self.start_time = None
        self.end_time = None

    def get_or_create_category(self, name: str, description: str = "") -> TestCategory:
        if name not in self.categories:
            self.categories[name] = TestCategory(name, description)
        return self.categories[name]

    def add_result(self, category_name: str, result: TestResult):
        category = self.get_or_create_category(category_name)
        category.add_result(result)

    @property
    def total_passed(self) -> int:
        return sum(c.passed_count for c in self.categories.values())

    @property
    def total_failed(self) -> int:
        return sum(c.failed_count for c in self.categories.values())

    @property
    def total_skipped(self) -> int:
        return sum(c.skipped_count for c in self.categories.values())

    @property
    def total_tests(self) -> int:
        return sum(c.total_count for c in self.categories.values())

    def print_summary(self):
        log.section("FINAL TEST SUMMARY")

        for cat_name, category in self.categories.items():
            status = "✅" if category.failed_count == 0 else "❌"
            log.info(f"{status} {cat_name}: {category.passed_count}/{category.passed_count + category.failed_count} passed ({category.success_rate:.0f}%)")

            # Show failures
            for result in category.results:
                if not result.passed and not result.skipped:
                    log.indent()
                    log.fail(f"{result.name}: {result.error_message}")
                    if result.debug_info:
                        for k, v in result.debug_info.items():
                            log.debug(f"  {k}: {v}")
                    log.dedent()

        log.info("")
        log.info("─" * 70)
        total = self.total_passed + self.total_failed
        rate = (self.total_passed / total * 100) if total > 0 else 0
        log.info(f"TOTAL: {self.total_passed} passed, {self.total_failed} failed, {self.total_skipped} skipped ({rate:.1f}%)")

        if self.total_failed == 0:
            log.success("🎮 ALL TESTS PASSED!")
        else:
            log.fail(f"⚠️ {self.total_failed} TESTS NEED ATTENTION")


# Global test suite
suite = TestSuite()


# ═══════════════════════════════════════════════════════════════════════════════
# HELPER UTILITIES
# ═══════════════════════════════════════════════════════════════════════════════

def safe_call(func: Callable, *args, **kwargs) -> Tuple[bool, Any, str]:
    """Safely call a function and catch exceptions"""
    try:
        result = func(*args, **kwargs)
        return True, result, ""
    except Exception as e:
        return False, None, f"{type(e).__name__}: {str(e)}"


def load_class_safe(class_path: str) -> Optional[type]:
    """Safely load a UClass"""
    try:
        return unreal.load_class(None, class_path)
    except:
        return None


def load_asset_safe(asset_path: str) -> Optional[unreal.Object]:
    """Safely load an asset"""
    try:
        return unreal.load_asset(asset_path)
    except:
        return None


def get_property_safe(obj: unreal.Object, prop_name: str, default=None) -> Any:
    """Safely get a property from an object"""
    try:
        return obj.get_editor_property(prop_name)
    except:
        try:
            return getattr(obj, prop_name, default)
        except:
            return default


def set_property_safe(obj: unreal.Object, prop_name: str, value: Any) -> bool:
    """Safely set a property on an object"""
    try:
        obj.set_editor_property(prop_name, value)
        return True
    except:
        try:
            setattr(obj, prop_name, value)
            return True
        except:
            return False


def has_property(obj: unreal.Object, prop_name: str) -> bool:
    """Check if object has a property"""
    try:
        obj.get_editor_property(prop_name)
        return True
    except:
        return hasattr(obj, prop_name)


def call_function_safe(obj: unreal.Object, func_name: str, *args) -> Tuple[bool, Any]:
    """Safely call a function on an object"""
    try:
        func = getattr(obj, func_name, None)
        if func and callable(func):
            result = func(*args)
            return True, result
        return False, None
    except Exception as e:
        log.debug(f"Function call failed: {func_name} - {e}")
        return False, None


def get_component(actor: unreal.Actor, component_class_name: str) -> Optional[unreal.ActorComponent]:
    """Get a component from an actor by class name"""
    try:
        components = actor.get_components_by_class(unreal.ActorComponent)
        for comp in components:
            if component_class_name in comp.get_class().get_name():
                return comp
        return None
    except:
        return None


def get_all_components(actor: unreal.Actor) -> List[unreal.ActorComponent]:
    """Get all components from an actor"""
    try:
        return list(actor.get_components_by_class(unreal.ActorComponent))
    except:
        return []


def describe_object(obj: unreal.Object, max_props: int = 10) -> Dict[str, Any]:
    """Get a debug description of an object"""
    info = {
        "class": obj.get_class().get_name() if obj else "None",
        "name": obj.get_name() if obj else "None",
    }

    # Try to get some common properties
    common_props = ["health", "stamina", "is_valid", "owner", "instigator"]
    for prop in common_props[:max_props]:
        val = get_property_safe(obj, prop)
        if val is not None:
            info[prop] = val

    return info


# ═══════════════════════════════════════════════════════════════════════════════
# TEST DECORATORS
# ═══════════════════════════════════════════════════════════════════════════════

def test(category: str, name: str = None):
    """Decorator to mark a function as a test"""
    def decorator(func):
        test_name = name or func.__name__

        def wrapper(*args, **kwargs):
            result = TestResult(test_name, category)
            log.info(f"Testing: {test_name}")
            log.indent()

            try:
                success, msg, debug = func(*args, **kwargs)
                if success:
                    result.set_passed()
                    log.success(f"PASSED: {msg or test_name}")
                else:
                    result.set_failed(msg or "Test failed")
                    log.fail(f"FAILED: {msg}")

                if debug:
                    result.debug_info = debug
                    for k, v in debug.items():
                        log.debug(f"{k}: {v}")

            except Exception as e:
                result.set_failed(f"Exception: {e}")
                log.error(f"EXCEPTION: {e}")
                log.debug(traceback.format_exc())

            log.dedent()
            suite.add_result(category, result)
            return result

        wrapper._is_test = True
        wrapper._category = category
        wrapper._name = test_name
        return wrapper
    return decorator


def skip_if(condition: bool, reason: str = "Condition not met"):
    """Skip test if condition is true"""
    def decorator(func):
        def wrapper(*args, **kwargs):
            if condition:
                result = TestResult(func.__name__, getattr(func, '_category', 'Unknown'))
                result.set_skipped(reason)
                log.info(f"SKIPPED: {func.__name__} - {reason}")
                suite.add_result(getattr(func, '_category', 'Unknown'), result)
                return result
            return func(*args, **kwargs)
        return wrapper
    return decorator


# ═══════════════════════════════════════════════════════════════════════════════
# ASSET & WORLD CONTEXT
# ═══════════════════════════════════════════════════════════════════════════════

class GameContext:
    """Holds references to game objects for testing"""

    def __init__(self):
        self.world = None
        self.game_mode = None
        self.player_controller = None
        self.player_character = None
        self.player_state = None
        self.game_state = None
        self.hud_widget = None

        # Component references
        self.stat_manager = None
        self.action_manager = None
        self.buff_manager = None
        self.combat_manager = None
        self.equipment_manager = None
        self.inventory_manager = None
        self.input_buffer = None
        self.interaction_manager = None
        self.status_effect_manager = None
        self.save_load_manager = None
        self.progress_manager = None

        # Test actors
        self.test_enemy = None
        self.test_item = None
        self.test_weapon = None

    def initialize(self) -> bool:
        """Initialize game context - call after world is loaded"""
        log.subsection("Initializing Game Context")

        # Get world
        try:
            self.world = unreal.EditorLevelLibrary.get_editor_world()
            if not self.world:
                # Try game world
                self.world = unreal.GameplayStatics.get_game_instance(None)
            log.debug(f"World: {self.world}")
        except Exception as e:
            log.warn(f"Could not get world: {e}")

        return True

    def get_component_from_character(self, component_name: str):
        """Get a component from the player character"""
        if not self.player_character:
            return None
        return get_component(self.player_character, component_name)

    def debug_dump(self):
        """Dump all context for debugging"""
        log.subsection("Game Context Debug Dump")
        log.debug(f"World: {self.world}")
        log.debug(f"GameMode: {self.game_mode}")
        log.debug(f"PlayerController: {self.player_controller}")
        log.debug(f"PlayerCharacter: {self.player_character}")
        log.debug(f"GameState: {self.game_state}")
        log.debug(f"PlayerState: {self.player_state}")

        if self.player_character:
            log.debug("Character Components:")
            for comp in get_all_components(self.player_character):
                log.debug(f"  - {comp.get_class().get_name()}")


# Global game context
ctx = GameContext()


# ═══════════════════════════════════════════════════════════════════════════════
# EXPORT TO SEPARATE TEST MODULES
# ═══════════════════════════════════════════════════════════════════════════════

# Export all utilities for use by test modules
__all__ = [
    'log', 'suite', 'ctx',
    'TestConfig', 'TestResult', 'TestCategory', 'TestSuite', 'GameContext',
    'test', 'skip_if',
    'safe_call', 'load_class_safe', 'load_asset_safe',
    'get_property_safe', 'set_property_safe', 'has_property',
    'call_function_safe', 'get_component', 'get_all_components',
    'describe_object',
]
