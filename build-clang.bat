rmdir build /s /Q
mkdir build
cd build
cmake -T ClangCL ..
cd ..
if %ERRORLEVEL% NEQ 0 pause