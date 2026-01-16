rmdir build /s /Q
mkdir build
cd build
cmake ..
cd ..
if %ERRORLEVEL% NEQ 0 pause