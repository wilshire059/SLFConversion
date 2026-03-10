@echo off
"C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" "C:\scripts\SLFConversion\SLFConversion.uproject" -ExecCmds="SLF.Reparent /Game/EldenRingAnimations/c3100_guard/ABP_c3100_Guard /Script/SLFConversion.ABP_SoulslikeEnemy,SLF.Save /Game/EldenRingAnimations/c3100_guard/ABP_c3100_Guard,quit" -unattended -nosplash -nullrhi
