"""Add DefaultSceneRoot to B_Interactable"""

# Update header
header_path = "C:/scripts/SLFConversion/Source/SLFConversion/Blueprints/B_Interactable.h"
with open(header_path, 'r', encoding='utf-8') as f:
    content = f.read()

# Add SceneComponent include if not present
if '#include "Components/SceneComponent.h"' not in content:
    content = content.replace(
        '#include "GameFramework/Actor.h"',
        '#include "GameFramework/Actor.h"\n#include "Components/SceneComponent.h"'
    )

# Add DefaultSceneRoot declaration after InteractionText
if 'DefaultSceneRoot' not in content:
    content = content.replace(
        '''UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Config")
	FText InteractionText;''',
        '''UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Config")
	FText InteractionText;

	/** Default scene root for Blueprint components to attach to */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> DefaultSceneRoot;'''
    )

with open(header_path, 'w', encoding='utf-8') as f:
    f.write(content)

print("Header updated")

# Update cpp
cpp_path = "C:/scripts/SLFConversion/Source/SLFConversion/Blueprints/B_Interactable.cpp"
with open(cpp_path, 'r', encoding='utf-8') as f:
    content = f.read()

# Check current constructor content
print(f"Current constructor starts: {content[content.find('AB_Interactable::AB_Interactable()'):content.find('AB_Interactable::AB_Interactable()')+200]}")

# Add DefaultSceneRoot creation in constructor if not present
if 'DefaultSceneRoot = CreateDefaultSubobject' not in content:
    # Find the constructor and add initialization
    old_ctor = '''AB_Interactable::AB_Interactable()
{
}'''
    new_ctor = '''AB_Interactable::AB_Interactable()
{
	// Create DefaultSceneRoot for Blueprint components to attach to
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = DefaultSceneRoot;
}'''
    if old_ctor in content:
        content = content.replace(old_ctor, new_ctor)
        print("Constructor updated")
    else:
        print("Could not find constructor pattern, manual update needed")
        print(f"Full content: {content}")

with open(cpp_path, 'w', encoding='utf-8') as f:
    f.write(content)

print("Done!")
