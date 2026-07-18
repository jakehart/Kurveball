set SCRIPT_PATH=%~dp0
set KURVEBALL_ROOT=%SCRIPT_PATH%.. 
set UNREAL_PLUGIN_OUTPUT_PATH=Examples\UnrealCurveDemo\Plugins\
set UNREAL_PLUGIN_RUNTIME_PATH=%UNREAL_PLUGIN_OUTPUT_PATH%\KurveballPlugin\Source\KurveballPluginRuntime
set UNREAL_RUNTIME_HEADER_PATH=%UNREAL_PLUGIN_RUNTIME_PATH%\Public
set UNREAL_RUNTIME_SOURCE_PATH=%UNREAL_PLUGIN_RUNTIME_PATH%\Private

:: Root project dir
pushd ..

mkdir %UNREAL_PLUGIN_OUTPUT_PATH%
mkdir %UNREAL_RUNTIME_HEADER_PATH%
mkdir %UNREAL_RUNTIME_SOURCE_PATH%

:: Headers and inlines -> Public/
for %%f in (Source\*.h) do (
    if exist "%UNREAL_RUNTIME_HEADER_PATH%\%%~nxf" del "%UNREAL_RUNTIME_HEADER_PATH%\%%~nxf"
    mklink "%UNREAL_RUNTIME_HEADER_PATH%\%%~nxf" "%CD%\%%f"
)

for %%f in (Source\*.inl) do (
    if exist "%UNREAL_RUNTIME_HEADER_PATH%\%%~nxf" del "%UNREAL_RUNTIME_HEADER_PATH%\%%~nxf"
    mklink "%UNREAL_RUNTIME_HEADER_PATH%\%%~nxf" "%CD%\%%f"
)

:: Sources -> Private/
for %%f in (Source\*.cpp) do (
    if exist "%UNREAL_RUNTIME_SOURCE_PATH%\%%~nxf" del "%UNREAL_RUNTIME_SOURCE_PATH%\%%~nxf"
    mklink "%UNREAL_RUNTIME_SOURCE_PATH%\%%~nxf" "%CD%\%%f"
)

:: UnrealWrapper subdirectory
mkdir %UNREAL_RUNTIME_HEADER_PATH%\UnrealWrapper\
mkdir %UNREAL_RUNTIME_SOURCE_PATH%\UnrealWrapper\

for %%f in (Source\UnrealWrapper\*.h) do (
    if exist "%UNREAL_RUNTIME_HEADER_PATH%\UnrealWrapper\%%~nxf" del "%UNREAL_RUNTIME_HEADER_PATH%\UnrealWrapper\%%~nxf"
    mklink "%UNREAL_RUNTIME_HEADER_PATH%\UnrealWrapper\%%~nxf" "%CD%\%%f"
)

for %%f in (Source\UnrealWrapper\*.inl) do (
    if exist "%UNREAL_RUNTIME_HEADER_PATH%\UnrealWrapper\%%~nxf" del "%UNREAL_RUNTIME_HEADER_PATH%\UnrealWrapper\%%~nxf"
    mklink "%UNREAL_RUNTIME_HEADER_PATH%\UnrealWrapper\%%~nxf" "%CD%\%%f"
)

for %%f in (Source\UnrealWrapper\*.cpp) do (
    if exist "%UNREAL_RUNTIME_SOURCE_PATH%\UnrealWrapper\%%~nxf" del "%UNREAL_RUNTIME_SOURCE_PATH%\UnrealWrapper\%%~nxf"
    mklink "%UNREAL_RUNTIME_SOURCE_PATH%\UnrealWrapper\%%~nxf" "%CD%\%%f"
)

:: Return to Scripts directory
popd