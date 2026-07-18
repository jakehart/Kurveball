:: Root project dir
pushd ..

:: Generate single-header, pure version of the library (no engine-specific wrappers included)
amalgamate.py --source=. --config=Scripts\SingleInclude.json

mkdir Releases
mkdir Releases\SingleFile\
move Scripts\KurveballSingleFile.* Releases\SingleFile\