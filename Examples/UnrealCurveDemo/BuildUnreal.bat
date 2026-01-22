@echo off
setlocal

:: Locate VS2026 installation path
for /f "usebackq tokens=*" %%i in (`^""%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -version "18.0.2" -property installationPath^"`) do (
    set "VS_PATH=%%i"
)

:: Verify path was found
if "%VS_PATH%"=="" (
    echo Visual Studio 2026 not found.
    pause
    exit /b 1
)

set "MSBUILD_PATH=%VS_PATH%\MSBuild\Current\Bin\MSBuild.exe"

:: Execute build for Unreal configuration
"%MSBUILD_PATH%" "CurveDemo.sln" ^
    /p:Configuration="Development Editor" ^
    /p:Platform="Win64" ^
    /t:Build ^
    /m ^
    /v:m
	/p:WarningLevel=0

pause