import re

with open("Source/SLFConversion/Animation/ABP_SoulslikeCharacter_Additive.cpp", "r") as f:
    content = f.read()

# Replace the first call logging
old = '''	// ALWAYS log first call to confirm C++ AnimInstance is active
	static bool bFirstCall = true;
	if (bFirstCall)
	{
		UE_LOG(LogTemp, Error, TEXT("=== [ANIM_CPP] NativeUpdateAnimation FIRST CALL - C++ AnimInstance is active! ==="));
		bFirstCall = false;
	}'''

new = '''	// ═══════════════════════════════════════════════════════════════════════════
	// VERBOSE DEBUG: Log FIRST call to confirm C++ AnimInstance is running
	// ═══════════════════════════════════════════════════════════════════════════
	static bool bFirstCall = true;
	if (bFirstCall)
	{
		bFirstCall = false;
		UE_LOG(LogTemp, Error, TEXT(""));
		UE_LOG(LogTemp, Error, TEXT("======================================================================"));
		UE_LOG(LogTemp, Error, TEXT("  [ANIM_CPP] C++ ANIMINSTANCE IS NOW ACTIVE!"));
		UE_LOG(LogTemp, Error, TEXT("  NativeUpdateAnimation() is being called every frame."));
		UE_LOG(LogTemp, Error, TEXT("  If you see this, the AnimBP is correctly using C++."));
		UE_LOG(LogTemp, Error, TEXT("======================================================================"));
		UE_LOG(LogTemp, Error, TEXT(""));
	}'''

if old in content:
    content = content.replace(old, new)
    print("Updated first call logging")
else:
    print("First call pattern not found")

# Replace periodic logging
old2 = '''	// DEBUG: Log animation state periodically (every 60 frames = ~1 second at 60fps)
	static int32 DebugLogCounter = 0;
	if (++DebugLogCounter % 60 == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ANIM_CPP] Speed=%.1f Falling=%d Blocking=%d RightOverlay=%d EquipMgr=%s"),
			Speed,
			bIsFalling ? 1 : 0,
			bIsBlocking ? 1 : 0,
			(int32)RightHandOverlayState,
			EquipmentManager ? TEXT("YES") : TEXT("NO"));
	}'''

new2 = '''	// ═══════════════════════════════════════════════════════════════════════════
	// DEBUG: Log state every second (60 frames at 60fps)
	// ═══════════════════════════════════════════════════════════════════════════
	static int32 DebugCounter = 0;
	if (++DebugCounter % 60 == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ANIM_CPP] Speed=%.1f Dir=%.1f Accel=%d Fall=%d Crouch=%d Block=%d Rest=%d ROverlay=%d"),
			Speed,
			Direction,
			bIsAccelerating ? 1 : 0,
			bIsFalling ? 1 : 0,
			IsCrouched ? 1 : 0,
			bIsBlocking ? 1 : 0,
			IsResting ? 1 : 0,
			(int32)RightHandOverlayState);
	}'''

if old2 in content:
    content = content.replace(old2, new2)
    print("Updated periodic logging")
else:
    print("Periodic logging pattern not found")

with open("Source/SLFConversion/Animation/ABP_SoulslikeCharacter_Additive.cpp", "w") as f:
    f.write(content)

print("Done!")
