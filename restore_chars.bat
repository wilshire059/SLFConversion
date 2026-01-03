@echo off
echo Restoring character Blueprints from bp_only...

REM Find and copy B_Soulslike_NPC
for /r "C:\scripts\bp_only\Content" %%f in (B_Soulslike_NPC.uasset) do (
    echo Found: %%f
    xcopy "%%f" "C:\scripts\SLFConversion\Content\SoulslikeFramework\Blueprints\Characters\" /Y
)

REM Find and copy B_Soulslike_Enemy_Guard
for /r "C:\scripts\bp_only\Content" %%f in (B_Soulslike_Enemy_Guard.uasset) do (
    echo Found: %%f
    xcopy "%%f" "C:\scripts\SLFConversion\Content\SoulslikeFramework\Blueprints\Characters\" /Y
)

REM Find and copy B_Soulslike_Enemy_Showcase
for /r "C:\scripts\bp_only\Content" %%f in (B_Soulslike_Enemy_Showcase.uasset) do (
    echo Found: %%f
    xcopy "%%f" "C:\scripts\SLFConversion\Content\SoulslikeFramework\Blueprints\Characters\" /Y
)

REM Find and copy B_Soulslike_Boss_Malgareth
for /r "C:\scripts\bp_only\Content" %%f in (B_Soulslike_Boss_Malgareth.uasset) do (
    echo Found: %%f
    xcopy "%%f" "C:\scripts\SLFConversion\Content\SoulslikeFramework\Blueprints\Characters\" /Y
)

REM Find and copy B_Soulslike_NPC_ShowcaseGuide
for /r "C:\scripts\bp_only\Content" %%f in (B_Soulslike_NPC_ShowcaseGuide.uasset) do (
    echo Found: %%f
    xcopy "%%f" "C:\scripts\SLFConversion\Content\SoulslikeFramework\Blueprints\Characters\" /Y
)

REM Find and copy B_Soulslike_NPC_ShowcaseVendor
for /r "C:\scripts\bp_only\Content" %%f in (B_Soulslike_NPC_ShowcaseVendor.uasset) do (
    echo Found: %%f
    xcopy "%%f" "C:\scripts\SLFConversion\Content\SoulslikeFramework\Blueprints\Characters\" /Y
)

echo Done!
