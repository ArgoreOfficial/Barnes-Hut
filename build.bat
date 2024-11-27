@echo off

where /q git
if errorlevel 1 (
    echo Couldn't find git. Make sure it is installed and placed in PATH.
    explorer https://git-scm.com/downloads
    pause
    exit /B
)

if exist .git\ (
  goto :submodule_update
)

:git_init
echo Updating git
git lfs instal
git lfs fetch --all
git lfs pull

:submodule_update
echo Updating submodule
git submodule init
git submodule update

:premake_create 
echo Creating project
cd tools
cd premake
premake5.exe vs2022
pause