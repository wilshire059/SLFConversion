"""
Debug script to discover GameplayTag API.
"""
import unreal

# Check GameplayTag
tag = unreal.GameplayTag()
methods = [x for x in dir(tag) if not x.startswith('_')]
unreal.log_warning(f"GameplayTag methods: {methods}")

# Check GameplayTagLibrary
lib_methods = [x for x in dir(unreal.GameplayTagLibrary) if not x.startswith('_')]
unreal.log_warning(f"GameplayTagLibrary methods: {lib_methods}")

# Try make_literal_gameplay_tag with no args to see signature
try:
    result = unreal.GameplayTagLibrary.make_literal_gameplay_tag()
    unreal.log_warning(f"make_literal_gameplay_tag() returned: {result}")
except Exception as e:
    unreal.log_warning(f"make_literal_gameplay_tag() error: {e}")
