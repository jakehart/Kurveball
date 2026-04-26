rmdir build /s /Q
mkdir build
cd build
cmake ..
if %ERRORLEVEL% NEQ 0 pause
cd ..