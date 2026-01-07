"""Update SLFSoulslikeCharacter.cpp to use direct property access instead of reflection"""
import re

FILE_PATH = "C:/scripts/SLFConversion/Source/SLFConversion/Blueprints/SLFSoulslikeCharacter.cpp"

# Read the file
with open(FILE_PATH, 'r', encoding='utf-8') as f:
    content = f.read()

# Define the old code pattern to find
OLD_CODE_START = "// Now load meshes from DefaultMeshInfo if available"
OLD_CODE_END = 'UE_LOG(LogTemp, Warning, TEXT("  DefaultMeshInfo is NULL - cannot load default meshes"));'

# Find the section to replace
start_idx = content.find(OLD_CODE_START)
if start_idx == -1:
    print("ERROR: Could not find start marker")
    exit(1)

end_idx = content.find(OLD_CODE_END, start_idx)
if end_idx == -1:
    print("ERROR: Could not find end marker")
    exit(1)

# Include the end line plus closing brace
end_idx = content.find("}", end_idx) + 1

print(f"Found section from {start_idx} to {end_idx}")
print(f"Old code length: {end_idx - start_idx} chars")

# New code
NEW_CODE = '''// Load meshes from DefaultMeshInfo using direct C++ property access (Option B migration)
	// The Blueprint PDA_DefaultMeshData now inherits from UPDA_DefaultMeshData with direct mesh properties
	if (DefaultMeshInfo)
	{
		UE_LOG(LogTemp, Log, TEXT("  Loading meshes from DefaultMeshInfo: %s (Class: %s)"),
			*DefaultMeshInfo->GetName(), *DefaultMeshInfo->GetClass()->GetName());

		// Cast to C++ class to access direct properties (avoids Blueprint struct reflection issues)
		UPDA_DefaultMeshData* MeshDataAsset = Cast<UPDA_DefaultMeshData>(DefaultMeshInfo);
		if (MeshDataAsset)
		{
			UE_LOG(LogTemp, Log, TEXT("    Successfully cast to UPDA_DefaultMeshData"));

			// Load and assign Head mesh
			if (Head && !Head->GetSkeletalMeshAsset())
			{
				if (!MeshDataAsset->HeadMesh.IsNull())
				{
					USkeletalMesh* LoadedMesh = MeshDataAsset->HeadMesh.LoadSynchronous();
					if (LoadedMesh)
					{
						Head->SetSkeletalMesh(LoadedMesh);
						UE_LOG(LogTemp, Log, TEXT("    Loaded Head mesh: %s"), *LoadedMesh->GetName());
					}
				}
			}

			// Load and assign UpperBody mesh
			if (UpperBody && !UpperBody->GetSkeletalMeshAsset())
			{
				if (!MeshDataAsset->UpperBodyMesh.IsNull())
				{
					USkeletalMesh* LoadedMesh = MeshDataAsset->UpperBodyMesh.LoadSynchronous();
					if (LoadedMesh)
					{
						UpperBody->SetSkeletalMesh(LoadedMesh);
						UE_LOG(LogTemp, Log, TEXT("    Loaded UpperBody mesh: %s"), *LoadedMesh->GetName());
					}
				}
			}

			// Load and assign Arms mesh
			if (Arms && !Arms->GetSkeletalMeshAsset())
			{
				if (!MeshDataAsset->ArmsMesh.IsNull())
				{
					USkeletalMesh* LoadedMesh = MeshDataAsset->ArmsMesh.LoadSynchronous();
					if (LoadedMesh)
					{
						Arms->SetSkeletalMesh(LoadedMesh);
						UE_LOG(LogTemp, Log, TEXT("    Loaded Arms mesh: %s"), *LoadedMesh->GetName());
					}
				}
			}

			// Load and assign LowerBody mesh
			if (LowerBody && !LowerBody->GetSkeletalMeshAsset())
			{
				if (!MeshDataAsset->LowerBodyMesh.IsNull())
				{
					USkeletalMesh* LoadedMesh = MeshDataAsset->LowerBodyMesh.LoadSynchronous();
					if (LoadedMesh)
					{
						LowerBody->SetSkeletalMesh(LoadedMesh);
						UE_LOG(LogTemp, Log, TEXT("    Loaded LowerBody mesh: %s"), *LoadedMesh->GetName());
					}
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("    Cast to UPDA_DefaultMeshData failed - class hierarchy may not be correct"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  DefaultMeshInfo is NULL - cannot load default meshes"));
	}'''

# Replace the old code with new code
new_content = content[:start_idx] + NEW_CODE + content[end_idx:]

print(f"New code length: {len(NEW_CODE)} chars")
print(f"Original file: {len(content)} chars")
print(f"New file: {len(new_content)} chars")

# Write back
with open(FILE_PATH, 'w', encoding='utf-8') as f:
    f.write(new_content)

print("SUCCESS: File updated")
