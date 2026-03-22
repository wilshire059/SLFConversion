# Quick script to fix DisplayName meta in C++ headers
import re

files_to_fix = {
    'C:/scripts/SLFConversion/Source/SLFConversion/Components/AC_SaveLoadManager.h': [
        ('UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")\n\tbool AutoSaveNeeded;',
         'UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime", meta=(DisplayName = "AutoSaveNeeded?"))\n\tbool AutoSaveNeeded;'),
        ('UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")\n\tbool CanResave;',
         'UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime", meta=(DisplayName = "CanResave?"))\n\tbool CanResave;'),
    ],
    'C:/scripts/SLFConversion/Source/SLFConversion/Components/AC_InputBuffer.h': [
        ('UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")\n\tbool BufferOpen;',
         'UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime", meta=(DisplayName = "BufferOpen?"))\n\tbool BufferOpen;'),
    ],
    'C:/scripts/SLFConversion/Source/SLFConversion/Components/AC_RadarManager.h': [
        ('UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")\n\tbool ShouldUpdate;',
         'UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime", meta=(DisplayName = "ShouldUpdate?"))\n\tbool ShouldUpdate;'),
    ],
}

for filepath, replacements in files_to_fix.items():
    try:
        with open(filepath, 'r') as f:
            content = f.read()

        for old, new in replacements:
            content = content.replace(old, new)

        with open(filepath, 'w') as f:
            f.write(content)

        print(f"Fixed: {filepath}")
    except Exception as e:
        print(f"Error with {filepath}: {e}")

print("Done!")
