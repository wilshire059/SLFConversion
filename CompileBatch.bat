@echo off
"C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" SLFConversionEditor Development Win64 -project="C:\scripts\SLFConversion\SLFConversion.uproject" -WaitMutex -MaxParallelActions=1
if %ERRORLEVEL% NEQ 0 (
  echo Build Failed
  exit /b %ERRORLEVEL%
)
echo Build Successful
