"""
Run PIE and capture BT diagnostic output
Uses the existing automation test runner with extended timeout
"""
import subprocess
import time

def main():
    print("=" * 70)
    print("RUNNING PIE BT DIAGNOSTIC")
    print("=" * 70)

    # Use -AutoTest flag which enables the SLFPIETestRunner
    cmd = [
        "C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe",
        "C:/scripts/SLFConversion/SLFConversion.uproject",
        "-game",  # Run as game (PIE-like)
        "-windowed",
        "-resx=800",
        "-resy=600",
        "-log",
        "-stdout",
        "-unattended",
        "-nosplash",
        "-nosound",
        "-AutoTest",  # Enable auto-test mode
        "-ExecCmds=slftests.run_all",  # Run tests after 3 seconds
    ]

    print(f"Command: {' '.join(cmd)}")
    print("")

    # Run with timeout
    try:
        result = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            timeout=60  # 60 second timeout
        )

        # Filter output for BT-related logs
        output_lines = result.stdout.split('\n') if result.stdout else []
        stderr_lines = result.stderr.split('\n') if result.stderr else []

        print("")
        print("=" * 70)
        print("BT-RELATED LOG OUTPUT:")
        print("=" * 70)

        keywords = [
            "BT_", "BTT_", "BTS_",
            "Blackboard", "State=",
            "ActiveNode", "IsRunning",
            "Combat", "Target",
            "AIBehaviorManager", "AIC_",
            "[BB]", "Decorator",
        ]

        for line in output_lines + stderr_lines:
            if any(kw in line for kw in keywords):
                print(line)

    except subprocess.TimeoutExpired:
        print("[TIMEOUT] Process timed out after 60 seconds")
    except Exception as e:
        print(f"[ERROR] {e}")

    print("")
    print("=" * 70)
    print("DONE")
    print("=" * 70)

if __name__ == "__main__":
    main()
