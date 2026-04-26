rmdir /s /q ..\build

pushd ..\
rd /s /q Releases
popd

pushd ..\Examples\UnrealCurveDemo

rd /s /q Binaries
rd /s /q Intermediate
rd /s /q DerivedDataCache
rd /s /q Saved
rd /s /q cmake-build-debug
rd /s /q .vs
del CurveDemo.sln
del .vsconfig

rd /s /q Plugins\KurveballPlugin\Binaries
rd /s /q Plugins\KurveballPlugin\Intermediate

popd