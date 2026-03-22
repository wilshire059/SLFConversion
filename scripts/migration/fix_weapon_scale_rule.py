#!/usr/bin/env python3
"""Fix ScaleRule from KeepWorld to SnapToTarget to match original Blueprint"""

def main():
    cpp_path = "C:/scripts/slfconversion/Source/SLFConversion/Blueprints/SLFWeaponBase.cpp"

    with open(cpp_path, 'r', encoding='utf-8') as f:
        content = f.read()

    # Replace KeepWorld with SnapToTarget for scale rule
    old_code = '''	// Attach to the socket
	bool bAttached = K2_AttachToComponent(
		MeshComponent,
		SocketName,
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::KeepWorld,
		false
	);'''

    new_code = '''	// Attach to the socket (SnapToTarget for all three rules - matches original Blueprint)
	bool bAttached = K2_AttachToComponent(
		MeshComponent,
		SocketName,
		EAttachmentRule::SnapToTarget,  // Location
		EAttachmentRule::SnapToTarget,  // Rotation
		EAttachmentRule::SnapToTarget,  // Scale (was KeepWorld, now matches Blueprint)
		false
	);'''

    if old_code not in content:
        print("ERROR: Could not find the old code pattern to replace!")
        print("Checking if already fixed...")
        if "EAttachmentRule::SnapToTarget,  // Scale" in content:
            print("Already fixed!")
            return
        return

    content = content.replace(old_code, new_code)

    with open(cpp_path, 'w', encoding='utf-8') as f:
        f.write(content)

    print("Successfully updated ScaleRule from KeepWorld to SnapToTarget")

if __name__ == "__main__":
    main()
