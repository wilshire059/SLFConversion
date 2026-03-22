"""Add DefaultSceneRoot to SLFBaseCharacter"""

# Update header
header_path = "C:/scripts/SLFConversion/Source/SLFConversion/Blueprints/SLFBaseCharacter.h"
with open(header_path, 'r', encoding='utf-8') as f:
    content = f.read()

# Add SceneComponent include if not present
if '#include "Components/SceneComponent.h"' not in content:
    content = content.replace(
        '#include "Components/TimelineComponent.h"',
        '#include "Components/TimelineComponent.h"\n#include "Components/SceneComponent.h"'
    )

# Add DefaultSceneRoot declaration after CachedExecutionWidget
if 'DefaultSceneRoot' not in content:
    content = content.replace(
        'TObjectPtr<UWidgetComponent> CachedExecutionWidget;',
        '''TObjectPtr<UWidgetComponent> CachedExecutionWidget;

	/** Default scene root for Blueprint components to attach to */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> DefaultSceneRoot;'''
    )

with open(header_path, 'w', encoding='utf-8') as f:
    f.write(content)

print("Header updated")

# Update cpp
cpp_path = "C:/scripts/SLFConversion/Source/SLFConversion/Blueprints/SLFBaseCharacter.cpp"
with open(cpp_path, 'r', encoding='utf-8') as f:
    content = f.read()

# Add DefaultSceneRoot creation in constructor
if 'DefaultSceneRoot = CreateDefaultSubobject' not in content:
    # Find the constructor initialization section and add DefaultSceneRoot creation
    old_init = '''	CachedTL_GenericRotation = nullptr;
	CachedTL_GenericLocation = nullptr;
}'''
    new_init = '''	CachedTL_GenericRotation = nullptr;
	CachedTL_GenericLocation = nullptr;

	// Create DefaultSceneRoot for Blueprint components to attach to
	// This provides the parent component that Blueprint-defined components expect
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	DefaultSceneRoot->SetupAttachment(GetCapsuleComponent());
}'''
    content = content.replace(old_init, new_init)

with open(cpp_path, 'w', encoding='utf-8') as f:
    f.write(content)

print("CPP updated")
print("Done!")
