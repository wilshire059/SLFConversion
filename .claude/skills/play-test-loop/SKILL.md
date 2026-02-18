---
name: play-test-loop
description: Automated Play-Test Loop for iterative PIE testing with screenshots, log capture, and enemy spawning. Use when testing gameplay changes, animations, or combat features. Triggers on "test", "play test", "PIE test", "validate in game", "screenshot", "visual validation". (project)
---

# Play-Test Loop - Automated PIE Testing

## PURPOSE

Automate the iterative cycle of: Build -> Launch PIE -> Test -> Capture -> Analyze -> Fix -> Repeat.

This skill provides a structured workflow for validating gameplay changes using the C++ test infrastructure in `Source/SLFConversion/Testing/`.

---

## AVAILABLE INFRASTRUCTURE

### C++ Test Classes
| Class | Purpose |
|-------|---------|
| `ASLFTestManager` | Spawn enemies, capture screenshots, export logs, monitor AnimNotifies |
| `USLFPIETestRunner` | GameInstance subsystem for automated PIE tests (Actions, Movement, SaveLoad, NPCDialog) |
| `USLFInputSimulator` | Simulate key presses, attacks, dodges, movement |

### Console Commands (PIE Runtime)
| Command | Purpose |
|---------|---------|
| `SLF.Test.Actions` | Run action system test (attack, dodge, combo) |
| `SLF.Test.Movement` | Run movement test (WASD, sprint) |
| `SLF.Test.SaveLoad` | Run save/load system test |
| `SLF.Test.NPCDialog` | Run NPC interaction and dialog test |
| `SLF.Test.SpawnEnemy <BPPath> [Distance]` | Spawn enemy at distance from player |
| `SLF.Test.Screenshot <Label>` | Capture labeled screenshot |
| `SLF.Test.ExportLog [Categories...]` | Export filtered log to file |
| `SLF.Test.ValidateCharacter <BPPath>` | Validate character Blueprint setup |
| `SLF.SimKey <KeyName>` | Simulate key press |
| `SLF.SimAttack` | Simulate attack input |
| `SLF.SimDodge` | Simulate dodge input |
| `SLF.SimMove <X> <Y> <Duration>` | Simulate movement |
| `SLF.SimCrouch` | Toggle crouch |

### Console Commands (Editor - WITH_EDITOR)
| Command | Purpose |
|---------|---------|
| `SLF.AddSocket <Skeleton> <Name> <Bone>` | Add socket to skeleton |
| `SLF.Reparent <BP> <CppClass>` | Reparent Blueprint |
| `SLF.ClearLogic <BP>` | Clear Blueprint logic |
| `SLF.Save <AssetPath>` | Save asset |

---

## PLAY-TEST WORKFLOW

### Step 1: Build C++
```bash
"C:\Program Files\Epic Games\UE_5.7\Engine\Build\BatchFiles\Build.bat" ^
  SLFConversionEditor Win64 Development ^
  -Project="C:\scripts\SLFConversion\SLFConversion.uproject" ^
  -WaitMutex -FromMsBuild
```
**Verify:** 0 errors, 0 warnings on new code.

### Step 2: Launch PIE with Auto-Test
```bash
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -ExecCmds="SLF.Test.Actions" ^
  -game -log -unattended -nosplash
```

Or for specific tests:
```bash
# Spawn enemy and take screenshots
-ExecCmds="SLF.Test.SpawnEnemy /Game/Blueprints/B_Soulslike_Enemy_Guard 500;SLF.Test.Screenshot Idle"

# Run all automated tests
-SLFAutoTest  # Flag that triggers USLFPIETestRunner auto-test mode
```

### Step 3: Capture Screenshots at Key Moments
Screenshots are saved to `Saved/Automation/` with timestamps.

Key moments to capture:
1. **Initial** - Before any action
2. **Combat** - During attack/dodge
3. **Hit Reaction** - After enemy takes damage
4. **UI State** - With HUD visible (health bars, menus)
5. **Animation** - During specific animation states

### Step 4: Export and Analyze Logs
```bash
-ExecCmds="SLF.Test.ExportLog LogTemp LogSLFTest LogAI LogSkeletalMesh"
```

Log files are saved to `Saved/Automation/TestLog_*.txt` with:
- Test log entries
- AnimNotify summary (which notifies fired, counts)
- Enemy state validation (mesh, AnimBP, montages)

### Step 5: Validate Results
Check for:
- [ ] Enemy spawned correctly (mesh visible, AnimBP assigned)
- [ ] Animations playing (idle, locomotion, attack montages)
- [ ] AnimNotifies firing (weapon traces, hit impacts, sound)
- [ ] No crashes or errors in filtered log
- [ ] Screenshots match expected visual state

### Step 6: Iterate on Failures
If validation fails:
1. Add targeted `UE_LOG(LogSLFTest, ...)` for the failing system
2. Rebuild C++
3. Re-run the specific test
4. Capture new screenshots/logs for comparison

---

## COMMON TEST SCENARIOS

### Enemy Guard Validation
```bash
SLF.Test.SpawnEnemy /Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard 500
SLF.Test.Screenshot Guard_Idle
SLF.SimAttack
SLF.Test.Screenshot Guard_Combat
SLF.Test.ExportLog LogTemp LogSLFTest LogAI
```

### Boss Validation (c4810 Erdtree Avatar)
```bash
SLF.Test.SpawnEnemy /Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss_Malgareth 800
SLF.Test.Screenshot Boss_Spawn
SLF.Test.ValidateCharacter /Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss_Malgareth
```

### Animation Validation
```bash
# Check AnimBP setup
SLF.Test.ValidateCharacter /Game/Path/To/Character
# Spawn and observe
SLF.Test.SpawnEnemy /Game/Path/To/Character 500
SLF.Test.Screenshot Anim_Idle
SLF.SimAttack
SLF.Test.Screenshot Anim_Attack
SLF.Test.ExportLog LogAnimation LogSkeletalMesh
```

### Widget Validation
```bash
# Open menu and screenshot
SLF.SimKey Escape
SLF.Test.Screenshot Menu_Open
SLF.Test.ExportLog LogTemp LogSlate
```

---

## ADDING NEW TESTS

### To SLFTestManager (Actor-based, per-scenario)
Add a new `RunTest()` case in `SLFTestManager.cpp`:
```cpp
void ASLFTestManager::RunTest(const FString& TestName)
{
    // ...existing cases...
    if (TestName == TEXT("MyNewTest"))
    {
        SpawnTestEnemy(TestEnemyClass, FVector(500.f, 0.f, 0.f));
        CaptureScreenshot(TEXT("Initial"));
        // Validate state after delay
        GetWorld()->GetTimerManager().SetTimer(TestTimerHandle, [this]()
        {
            FString State = ValidateEnemyState(TEXT("MyExpectedState"));
            CaptureScreenshot(TEXT("After"));
            ExportFilteredLog();
        }, 3.0f, false);
    }
}
```

### To SLFPIETestRunner (Subsystem-based, always available)
Add a new `Run*Test()` method in `SLFPIETestRunner.h/.cpp` and register a console command.

### To SLFAutomationLibrary (Editor-only validation)
Add a static validation function that can run without PIE.

---

## NEVER USE PYTHON API FOR TESTING

All test infrastructure is in C++. Never use Unreal Python API for:
- Asset validation (use `SLF.Test.ValidateCharacter`)
- Screenshot capture (use `SLF.Test.Screenshot`)
- Input simulation (use `SLF.Sim*` commands)
- Log export (use `SLF.Test.ExportLog`)

---

## FILES

| File | Purpose |
|------|---------|
| `Source/SLFConversion/Testing/SLFTestManager.h/cpp` | Test manager actor |
| `Source/SLFConversion/Testing/SLFPIETestRunner.h/cpp` | PIE test subsystem |
| `Source/SLFConversion/Testing/SLFInputSimulator.h/cpp` | Input simulation utilities |
| `Source/SLFConversion/SLFConversion.cpp` | Console command registration |
| `Source/SLFConversion/SLFAutomationLibrary.h/cpp` | Validation/screenshot functions |
