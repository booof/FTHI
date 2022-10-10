@echo off
set arg1=%~1
set original_path=%~dp0
echo %arg1%
cd %arg1%
cmake ../Code
cmake --build .
cd ../Code
copy /y nul "compilation complete"
cd %original_path%
exit 0