// W_Navigable_InputReader.cpp
// C++ Widget implementation for W_Navigable_InputReader
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session

#include "Widgets/W_Navigable_InputReader.h"
#include "SLFGameTypes.h"
#include "Engine/DataTable.h"

UW_Navigable_InputReader::UW_Navigable_InputReader(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UW_Navigable_InputReader::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_Navigable_InputReader::NativeConstruct"));
}

void UW_Navigable_InputReader::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_Navigable_InputReader::NativeDestruct"));
}

void UW_Navigable_InputReader::CacheWidgetReferences()
{
	// Widget reference caching intentionally empty - dev tool widget
}

void UW_Navigable_InputReader::GetInputIconForKey_Implementation(const FKey& Key, TSoftObjectPtr<UTexture2D>& Icon)
{
	Icon = nullptr;

	if (!Key.IsValid())
	{
		return;
	}

	// Construct texture path directly from key name
	// The data table uses a Blueprint UserDefinedStruct which can't be accessed from C++
	// Instead, we map FKey names to texture names following the pattern:
	// /Game/SoulslikeFramework/Widgets/_Textures/Prompts/KBM/T_Prompt_{KeyMapping}_Key_Dark

	FString KeyName = Key.ToString();
	FString TextureKeyName;

	// Map FKey names to texture key names
	// Single letters (A-Z)
	if (KeyName.Len() == 1 && KeyName[0] >= 'A' && KeyName[0] <= 'Z')
	{
		TextureKeyName = KeyName;
	}
	// Special key mappings
	else if (KeyName == TEXT("Enter"))
	{
		TextureKeyName = TEXT("Enter");
	}
	else if (KeyName == TEXT("Escape"))
	{
		TextureKeyName = TEXT("Esc"); // Shortened in texture name
	}
	else if (KeyName == TEXT("Tab"))
	{
		TextureKeyName = TEXT("Tab");
	}
	else if (KeyName == TEXT("SpaceBar"))
	{
		TextureKeyName = TEXT("Space");
	}
	else if (KeyName == TEXT("BackSpace"))
	{
		TextureKeyName = TEXT("Backspace");
	}
	// Arrow keys
	else if (KeyName == TEXT("Up"))
	{
		TextureKeyName = TEXT("Arrow_Up");
	}
	else if (KeyName == TEXT("Down"))
	{
		TextureKeyName = TEXT("Arrow_Down");
	}
	else if (KeyName == TEXT("Left"))
	{
		TextureKeyName = TEXT("Arrow_Left");
	}
	else if (KeyName == TEXT("Right"))
	{
		TextureKeyName = TEXT("Arrow_Right");
	}
	// Mouse buttons
	else if (KeyName == TEXT("LeftMouseButton"))
	{
		TextureKeyName = TEXT("Mouse_Left");
	}
	else if (KeyName == TEXT("RightMouseButton"))
	{
		TextureKeyName = TEXT("Mouse_Right");
	}
	else if (KeyName == TEXT("MiddleMouseButton"))
	{
		TextureKeyName = TEXT("Mouse_Middle");
	}
	// Modifier keys
	else if (KeyName == TEXT("LeftShift") || KeyName == TEXT("RightShift"))
	{
		TextureKeyName = TEXT("Shift");
	}
	else if (KeyName == TEXT("LeftControl") || KeyName == TEXT("RightControl"))
	{
		TextureKeyName = TEXT("Ctrl");
	}
	else if (KeyName == TEXT("LeftAlt") || KeyName == TEXT("RightAlt"))
	{
		TextureKeyName = TEXT("Alt");
	}
	// Function keys (F1-F12)
	else if (KeyName.StartsWith(TEXT("F")) && KeyName.Len() <= 3)
	{
		TextureKeyName = KeyName; // F1, F2, ... F12
	}
	// Number keys (0-9)
	else if (KeyName.Len() == 1 && KeyName[0] >= '0' && KeyName[0] <= '9')
	{
		TextureKeyName = KeyName;
	}
	// Other common keys
	else if (KeyName == TEXT("Delete"))
	{
		TextureKeyName = TEXT("Del");
	}
	else if (KeyName == TEXT("Insert"))
	{
		TextureKeyName = TEXT("Insert");
	}
	else if (KeyName == TEXT("Home"))
	{
		TextureKeyName = TEXT("Home");
	}
	else if (KeyName == TEXT("End"))
	{
		TextureKeyName = TEXT("End");
	}
	else if (KeyName == TEXT("PageUp"))
	{
		TextureKeyName = TEXT("Page_Up");
	}
	else if (KeyName == TEXT("PageDown"))
	{
		TextureKeyName = TEXT("Page_Down");
	}
	else if (KeyName == TEXT("CapsLock"))
	{
		TextureKeyName = TEXT("Caps_Lock");
	}
	else if (KeyName == TEXT("NumLock"))
	{
		TextureKeyName = TEXT("Num_Lock");
	}
	// Gamepad buttons - use Gamepad folder
	else if (KeyName.StartsWith(TEXT("Gamepad")))
	{
		// Map gamepad keys to PS4 controller textures
		if (KeyName == TEXT("Gamepad_FaceButton_Bottom"))
		{
			Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/SoulslikeFramework/Widgets/_Textures/Prompts/Gamepad/T_Prompt_PS4_Cross.T_Prompt_PS4_Cross")));
			return;
		}
		else if (KeyName == TEXT("Gamepad_FaceButton_Right"))
		{
			Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/SoulslikeFramework/Widgets/_Textures/Prompts/Gamepad/T_Prompt_PS4_Circle.T_Prompt_PS4_Circle")));
			return;
		}
		else if (KeyName == TEXT("Gamepad_FaceButton_Left"))
		{
			Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/SoulslikeFramework/Widgets/_Textures/Prompts/Gamepad/T_Prompt_PS4_Square.T_Prompt_PS4_Square")));
			return;
		}
		else if (KeyName == TEXT("Gamepad_FaceButton_Top"))
		{
			Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/SoulslikeFramework/Widgets/_Textures/Prompts/Gamepad/T_Prompt_PS4_Triangle.T_Prompt_PS4_Triangle")));
			return;
		}
		else if (KeyName == TEXT("Gamepad_LeftShoulder"))
		{
			Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/SoulslikeFramework/Widgets/_Textures/Prompts/Gamepad/T_Prompt_PS4_L1.T_Prompt_PS4_L1")));
			return;
		}
		else if (KeyName == TEXT("Gamepad_RightShoulder"))
		{
			Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/SoulslikeFramework/Widgets/_Textures/Prompts/Gamepad/T_Prompt_PS4_R1.T_Prompt_PS4_R1")));
			return;
		}
		else if (KeyName == TEXT("Gamepad_LeftTrigger"))
		{
			Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/SoulslikeFramework/Widgets/_Textures/Prompts/Gamepad/T_Prompt_PS4_L2.T_Prompt_PS4_L2")));
			return;
		}
		else if (KeyName == TEXT("Gamepad_RightTrigger"))
		{
			Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/SoulslikeFramework/Widgets/_Textures/Prompts/Gamepad/T_Prompt_PS4_R2.T_Prompt_PS4_R2")));
			return;
		}
		else if (KeyName == TEXT("Gamepad_DPad_Up"))
		{
			Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/SoulslikeFramework/Widgets/_Textures/Prompts/Gamepad/T_Prompt_PS4_Dpad_Up.T_Prompt_PS4_Dpad_Up")));
			return;
		}
		else if (KeyName == TEXT("Gamepad_DPad_Down"))
		{
			Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/SoulslikeFramework/Widgets/_Textures/Prompts/Gamepad/T_Prompt_PS4_Dpad_Down.T_Prompt_PS4_Dpad_Down")));
			return;
		}
		else if (KeyName == TEXT("Gamepad_DPad_Left"))
		{
			Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/SoulslikeFramework/Widgets/_Textures/Prompts/Gamepad/T_Prompt_PS4_Dpad_Left.T_Prompt_PS4_Dpad_Left")));
			return;
		}
		else if (KeyName == TEXT("Gamepad_DPad_Right"))
		{
			Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/SoulslikeFramework/Widgets/_Textures/Prompts/Gamepad/T_Prompt_PS4_Dpad_Right.T_Prompt_PS4_Dpad_Right")));
			return;
		}
		else if (KeyName == TEXT("Gamepad_LeftThumbstick"))
		{
			Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/SoulslikeFramework/Widgets/_Textures/Prompts/Gamepad/T_Prompt_PS4_Left_Stick_Click.T_Prompt_PS4_Left_Stick_Click")));
			return;
		}
		else if (KeyName == TEXT("Gamepad_RightThumbstick"))
		{
			Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/SoulslikeFramework/Widgets/_Textures/Prompts/Gamepad/T_Prompt_PS4_Right_Stick_Click.T_Prompt_PS4_Right_Stick_Click")));
			return;
		}
		else if (KeyName == TEXT("Gamepad_Special_Right"))
		{
			Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/SoulslikeFramework/Widgets/_Textures/Prompts/Gamepad/T_Prompt_PS4_Options.T_Prompt_PS4_Options")));
			return;
		}
		else if (KeyName == TEXT("Gamepad_Special_Left"))
		{
			Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TEXT("/Game/SoulslikeFramework/Widgets/_Textures/Prompts/Gamepad/T_Prompt_PS4_Share.T_Prompt_PS4_Share")));
			return;
		}
		// Unknown gamepad key
		return;
	}
	else
	{
		// Unknown key - try using key name directly
		TextureKeyName = KeyName;
	}

	// Construct the full texture path for keyboard/mouse keys
	if (!TextureKeyName.IsEmpty())
	{
		FString TexturePath = FString::Printf(
			TEXT("/Game/SoulslikeFramework/Widgets/_Textures/Prompts/KBM/T_Prompt_%s_Key_Dark.T_Prompt_%s_Key_Dark"),
			*TextureKeyName, *TextureKeyName);

		Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(TexturePath));
	}
}
void UW_Navigable_InputReader::EventInitializeInputDevice_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Navigable_InputReader::EventInitializeInputDevice_Implementation"));
}


void UW_Navigable_InputReader::EventOnHardwareDeviceDetected_Implementation(FPlatformUserId UserId, FInputDeviceId DeviceId)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Navigable_InputReader::EventOnHardwareDeviceDetected_Implementation"));
}
