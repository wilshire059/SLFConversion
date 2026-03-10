# run_apply_flask_data.py
# Call C++ SLFAutomationLibrary::ApplyFlaskData() to restore flask data

import unreal

def run_apply_flask_data():
    """Call the C++ automation function to restore flask data"""

    unreal.log("=== Calling USLFAutomationLibrary::ApplyFlaskData() ===")

    result = unreal.SLFAutomationLibrary.apply_flask_data()
    unreal.log(f"Result: {result}")

    # Also write to a file for easy access
    with open("C:/scripts/SLFConversion/flask_data_result.txt", "w") as f:
        f.write(f"ApplyFlaskData Result:\n{result}\n")

    return result

if __name__ == "__main__":
    result = run_apply_flask_data()
