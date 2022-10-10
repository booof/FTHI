@echo off
set arg1=%~1
set original_path=%~dp0
echo %arg1%
cd %arg1%
del ..\LastBuildOutput.txt
cmake --build . >> ../LastBuildOutput.txt 2>&1
cd ../Code
copy /y nul "compilation complete"
cd %original_path%
exit 0