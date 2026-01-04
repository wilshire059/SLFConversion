"""
PIE Input Simulation Test
==========================

This script demonstrates how to use the SLF Input Simulator during PIE.

HOW TO USE:
-----------
1. Open the Unreal Editor (not headless mode)
2. Load the L_Demo level
3. Press Play (PIE)
4. Open the console (~) and use these commands:

   SLF.SimAttack          - Simulate attack
   SLF.SimDodge           - Simulate dodge/roll
   SLF.SimKey SpaceBar    - Simulate specific key
   SLF.SimMove 1 0 2      - Move right for 2 seconds
   SLF.SimMove 0 1 1      - Move forward for 1 second
   SLF.Test.Actions       - Run action system test
   SLF.Test.Movement      - Run movement test

AVAILABLE CONSOLE COMMANDS:
---------------------------
  SLF.SimKey <KeyName>        - Simulate a key press
                                Keys: SpaceBar, E, W, A, S, D, LeftMouseButton, etc.

  SLF.SimAttack               - Simulate attack (left mouse)
  SLF.SimDodge                - Simulate dodge (space)
  SLF.SimMove <X> <Y> <Dur>   - Simulate movement
                                X: -1 (left) to 1 (right)
                                Y: -1 (back) to 1 (forward)
                                Dur: duration in seconds

  SLF.Test.Actions            - Run automated action test
  SLF.Test.Movement           - Run automated movement test

BLUEPRINT/C++ ACCESS:
---------------------
  // In C++
  USLFInputSimulator::SimulateKeyPress(WorldContext, EKeys::SpaceBar);
  USLFInputSimulator::SimulateAttack(WorldContext);
  USLFInputSimulator::SimulateMovement(WorldContext, FVector2D(1, 0), 2.0f);

  // In Blueprint
  Call any function from "Input Simulation" category

PROGRAMMATIC PIE (Editor Utility):
----------------------------------
  // Start PIE from C++/Blueprint
  USLFInputSimulatorLibrary::StartPIE();

  // Check if running
  bool bRunning = USLFInputSimulatorLibrary::IsPIERunning();

  // Stop PIE
  USLFInputSimulatorLibrary::StopPIE();
"""

print(__doc__)
