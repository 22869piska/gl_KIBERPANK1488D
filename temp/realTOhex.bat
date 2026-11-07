@echo off
setlocal EnableDelayedExpansion
chcp 65001 >nul
title Float/Double <-> Hex (IEEE 754)

:menu
cls
echo ==================================================
echo    Float/Double ^<--^> Hex  (IEEE 754 bits)
echo ==================================================
echo   1 - float  -^> hex      [32-bit, 8 digits]
echo   2 - double -^> hex      [64-bit, 16 digits]
echo   3 - hex    -^> float    [8 hex digits]
echo   4 - hex    -^> double   [16 hex digits]
echo   q - quit
echo.
set /p "m=Choose [1-4/q]: "
if /i "!m!"=="q" goto :eof

if "!m!"=="1" (
    set /p "v=float value: "
    if "!v!"=="" goto menu
    powershell -NoProfile -Command ^
      "$s='!v!'.Replace(',','.'); $v=[float]::Parse($s,[Globalization.CultureInfo]::InvariantCulture); $b=[BitConverter]::GetBytes($v); $u=[BitConverter]::ToUInt32($b,0); Write-Host ('float32 = 0x{0:X8}' -f $u) -ForegroundColor Green"
    echo.
    pause
    goto menu
)

if "!m!"=="2" (
    set /p "v=double value: "
    if "!v!"=="" goto menu
    powershell -NoProfile -Command ^
      "$s='!v!'.Replace(',','.'); $v=[double]::Parse($s,[Globalization.CultureInfo]::InvariantCulture); $b=[BitConverter]::GetBytes($v); $u=[BitConverter]::ToUInt64($b,0); Write-Host ('double64 = 0x{0:X16}' -f $u) -ForegroundColor Green"
    echo.
    pause
    goto menu
)

if "!m!"=="3" (
    set /p "v=hex (8 digits, 0x optional): "
    if "!v!"=="" goto menu
    powershell -NoProfile -Command ^
      "$h='!v!' -replace '^(0[xX])',''; $h=$h.PadLeft(8,'0'); $u=[Convert]::ToUInt32($h,16); $f=[BitConverter]::ToSingle([BitConverter]::GetBytes($u),0); Write-Host ('float  = ' + $f.ToString('R',[Globalization.CultureInfo]::InvariantCulture)) -ForegroundColor Green"
    echo.
    pause
    goto menu
)

if "!m!"=="4" (
    set /p "v=hex (16 digits, 0x optional): "
    if "!v!"=="" goto menu
    powershell -NoProfile -Command ^
      "$h='!v!' -replace '^(0[xX])',''; $h=$h.PadLeft(16,'0'); $u=[Convert]::ToUInt64($h,16); $d=[BitConverter]::ToDouble([BitConverter]::GetBytes($u),0); Write-Host ('double = ' + $d.ToString('R',[Globalization.CultureInfo]::InvariantCulture)) -ForegroundColor Green"
    echo.
    pause
    goto menu
)

goto menu