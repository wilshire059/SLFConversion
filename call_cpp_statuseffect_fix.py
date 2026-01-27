"""
Call C++ ApplyAllStatusEffectRankInfo to fix poison damage values.

The FInstancedStruct data cannot be created from Python, so we must
call the C++ function which has hardcoded values.

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/call_cpp_statuseffect_fix.py" ^
  -stdout -unattended -nosplash 2>&1
"""

import unreal


def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("Calling C++ ApplyAllStatusEffectRankInfo")
    unreal.log_warning("=" * 70)

    # Get the automation library class
    try:
        # Method 1: Try direct static call
        result = unreal.SLFAutomationLibrary.apply_all_status_effect_rank_info()
        unreal.log_warning(f"Result: {result}")
    except AttributeError as e:
        unreal.log_warning(f"Method 1 failed (expected): {e}")

        # Method 2: Try via class default object
        try:
            lib_class = unreal.load_class(None, "/Script/SLFConversion.SLFAutomationLibrary")
            if lib_class:
                unreal.log_warning(f"Found class: {lib_class}")
                # For static functions, we need to call on the class itself
                result = lib_class.apply_all_status_effect_rank_info()
                unreal.log_warning(f"Result: {result}")
            else:
                unreal.log_warning("Could not find SLFAutomationLibrary class")
        except Exception as e2:
            unreal.log_warning(f"Method 2 failed: {e2}")

            # Method 3: Try via execute_console_command
            try:
                unreal.log_warning("Trying console command approach...")
                # Some UFUNCTION(BlueprintCallable) can be invoked via exec
                unreal.SystemLibrary.execute_console_command(
                    None,
                    "SLF.ApplyAllStatusEffectRankInfo",
                    None
                )
            except Exception as e3:
                unreal.log_warning(f"Method 3 failed: {e3}")

    unreal.log_warning("=" * 70)
    unreal.log_warning("Done!")
    unreal.log_warning("=" * 70)


if __name__ == "__main__":
    main()
