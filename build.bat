@echo off
echo Building SDRuno Cosmo Plugin...
echo.

if "%1"=="clean" (
    echo Cleaning build directories...
    rmdir /s /q Debug 2>nul
    rmdir /s /q Release 2>nul
    rmdir /s /q x64 2>nul
    rmdir /s /q .vs 2>nul
    echo Clean completed.
    goto :end
)

if not exist "C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe" (
    echo Visual Studio not found. Please install Visual Studio 2019 or later.
    exit /b 1
)

for /f "usebackq tokens=*" %%i in (`"C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe" -latest -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
    set InstallDir=%%i
)

if not exist "%InstallDir%\Common7\Tools\VsDevCmd.bat" (
    echo Visual Studio build tools not found.
    exit /b 1
)

call "%InstallDir%\Common7\Tools\VsDevCmd.bat"

echo Building for x64 Release...
msbuild SDRunoPlugin_Cosmo.sln /p:Configuration=Release /p:Platform=x64 /m

if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    exit /b 1
)

echo.
echo Build completed successfully!
echo Output: x64\Release\SDRunoPlugin_Cosmo.dll
echo.
echo To install:
echo 1. Copy the DLL to your SDRuno plugins directory
echo 2. Restart SDRuno
echo 3. Find the plugin in the Plugins menu

:end