// CS_Boss_Roar.cpp
// C++ implementation for Blueprint CS_Boss_Roar
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/CS_Boss_Roar.json

#include "Blueprints/CS_Boss_Roar.h"

UCS_Boss_Roar::UCS_Boss_Roar()
{
	// Camera shake settings for boss roar
	OscillationDuration = 0.5f;
	OscillationBlendInTime = 0.1f;
	OscillationBlendOutTime = 0.2f;

	// Rotational oscillation (pitch/yaw/roll)
	RotOscillation.Pitch.Amplitude = 1.0f;
	RotOscillation.Pitch.Frequency = 15.0f;
	RotOscillation.Yaw.Amplitude = 1.0f;
	RotOscillation.Yaw.Frequency = 15.0f;
	RotOscillation.Roll.Amplitude = 0.5f;
	RotOscillation.Roll.Frequency = 10.0f;

	// Location oscillation (minimal)
	LocOscillation.X.Amplitude = 0.0f;
	LocOscillation.Y.Amplitude = 0.0f;
	LocOscillation.Z.Amplitude = 0.0f;
}

