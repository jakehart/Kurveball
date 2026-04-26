set UNREAL_PLUGIN_OUTPUT_PATH=Examples\UnrealCurveDemo\Plugins\
set UNREAL_PLUGIN_RUNTIME_PATH=%UNREAL_PLUGIN_OUTPUT_PATH%\KurveballPlugin\Source\KurveballPluginRuntime
set UNREAL_RUNTIME_HEADER_PATH=%UNREAL_PLUGIN_RUNTIME_PATH%\Public
set UNREAL_RUNTIME_SOURCE_PATH=%UNREAL_PLUGIN_RUNTIME_PATH%\Private

:: Generate single-header, pure version of the library (no engine-specific wrappers included)
amalgamate.py --source=. --config=SingleInclude.json

:: Root project dir
pushd ..

mkdir Releases
mkdir Releases\SingleFile\
move Scripts\KurveballSingleFile.* Releases\SingleFile\

mkdir %UNREAL_PLUGIN_OUTPUT_PATH%
mkdir %UNREAL_RUNTIME_HEADER_PATH%
mkdir %UNREAL_RUNTIME_SOURCE_PATH%

copy Source\*.h %UNREAL_RUNTIME_HEADER_PATH%
copy Source\*.inl %UNREAL_RUNTIME_HEADER_PATH%
copy Source\*.cpp %UNREAL_RUNTIME_SOURCE_PATH%

:: Unreal-specific wrapper types go in their own subdirectory
mkdir %UNREAL_RUNTIME_HEADER_PATH%\UnrealWrapper\
mkdir %UNREAL_RUNTIME_SOURCE_PATH%\UnrealWrapper\

copy Source\UnrealWrapper\*.h %UNREAL_RUNTIME_HEADER_PATH%\UnrealWrapper\
copy Source\UnrealWrapper\*.inl %UNREAL_RUNTIME_HEADER_PATH%\UnrealWrapper\
copy Source\UnrealWrapper\*.cpp %UNREAL_RUNTIME_SOURCE_PATH%\UnrealWrapper\

xcopy /Y /E %UNREAL_PLUGIN_OUTPUT_PATH% Releases\

:: Return to Scripts directory
popd