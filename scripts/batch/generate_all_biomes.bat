@echo off
echo ============================================
echo   Generating ALL 5 Biome Levels
echo ============================================
echo.

set UE="C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe"
set PROJ="C:\scripts\SLFConversion\SLFConversion.uproject"

echo [1/5] Stone Cave (default)...
%UE% %PROJ% -run=AutoBuildDungeon -Biome=stone -Seed=42 -unattended -nosplash

echo.
echo [2/5] Lava Cavern...
%UE% %PROJ% -run=AutoBuildDungeon -Biome=lava -Seed=42 -unattended -nosplash

echo.
echo [3/5] Ice Grotto...
%UE% %PROJ% -run=AutoBuildDungeon -Biome=ice -Seed=42 -unattended -nosplash

echo.
echo [4/5] Fungal Depths...
%UE% %PROJ% -run=AutoBuildDungeon -Biome=fungal -Seed=42 -unattended -nosplash

echo.
echo [5/5] The Abyss...
%UE% %PROJ% -run=AutoBuildDungeon -Biome=abyss -Seed=42 -unattended -nosplash

echo.
echo ============================================
echo   ALL 5 BIOMES GENERATED
echo ============================================
echo   L_CaveTest     - Stone Cave (120x120)
echo   L_LavaCavern   - Lava Cavern (80x80)
echo   L_IceGrotto    - Ice Grotto (100x100)
echo   L_FungalDepths - Fungal Depths (140x140)
echo   L_Abyss        - The Abyss (60x60)
echo ============================================
pause
