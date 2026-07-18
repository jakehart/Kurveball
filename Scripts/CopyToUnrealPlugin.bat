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

copy %KURVEBALL_ROOT%\Source\*.h %UNREAL_RUNTIME_HEADER_PATH%
copy %KURVEBALL_ROOT%\Source\*.inl %UNREAL_RUNTIME_HEADER_PATH%
copy %KURVEBALL_ROOT%\Source\*.cpp %UNREAL_RUNTIME_SOURCE_PATH%

:: Unreal-specific wrapper types go in their own subdirectory
mkdir %UNREAL_RUNTIME_HEADER_PATH%\UnrealWrapper\
mkdir %UNREAL_RUNTIME_SOURCE_PATH%\UnrealWrapper\

copy %KURVEBALL_ROOT%\Source\UnrealWrapper\*.h %UNREAL_RUNTIME_HEADER_PATH%\UnrealWrapper\
copy %KURVEBALL_ROOT%\Source\UnrealWrapper\*.inl %UNREAL_RUNTIME_HEADER_PATH%\UnrealWrapper\
copy %KURVEBALL_ROOT%\Source\UnrealWrapper\*.cpp %UNREAL_RUNTIME_SOURCE_PATH%\UnrealWrapper\

:: Return to Scripts directory
popd