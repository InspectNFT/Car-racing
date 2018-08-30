@echo off
setlocal

rem BAT script that creates the binaries for Carla (carla.org).
rem Run it through a cmd with the x64 Visual C++ Toolset enabled.

set LOCAL_PATH=%~dp0
set "FILE_N=-[%~n0]:"

rem Print batch params (debug purpose)
echo %FILE_N% [Batch params]: %*

rem ============================================================================
rem -- Parse arguments ---------------------------------------------------------
rem ============================================================================

set DOC_STRING=Build LibCarla.
set "USAGE_STRING=Usage: %FILE_N% [-h^|--help] [--build] [--rebuild] [--launch] [--clean]"

set BUILD_CARLAUE4=false
set BUILD_CARLAUE4_EDITOR=false

set LAUNCH_UE4_EDITOR=false
set REMOVE_INTERMEDIATE=false

set UE_VERSION=4.19

:arg-parse
if not "%1"=="" (
    if "%1"=="--rebuild" (
        set REMOVE_INTERMEDIATE=true
        set BUILD_CARLAUE4=true
    )

    if "%1"=="--build" (
        set BUILD_CARLAUE4=true
    )

    if "%1"=="--build-editor" (
        set BUILD_CARLAUE4_EDITOR=true
    )

    if "%1"=="--ue-version" (
        set UE_VERSION=%~2
        shift
    )

    if "%1"=="--launch-editor" (
        set LAUNCH_UE4_EDITOR=true
    )

    if "%1"=="--clean" (
        set REMOVE_INTERMEDIATE=true
    )

    if "%1"=="-h" (
        echo %DOC_STRING%
        echo %USAGE_STRING%
        GOTO :eof
    )

    if "%1"=="--help" (
        echo %DOC_STRING%
        echo %USAGE_STRING%
        GOTO :eof
    )

    shift
    goto :arg-parse
)

if %REMOVE_INTERMEDIATE% == false (
    if %LAUNCH_UE4_EDITOR% == false (
        if %BUILD_CARLAUE4% == false (
            if %BUILD_CARLAUE4_EDITOR% == false (
                echo Nothing selected to be done.
                goto :eof
            )
        )
    )
)

rem Extract Unreal Engine root path
rem
set KEY_NAME="HKEY_LOCAL_MACHINE\SOFTWARE\EpicGames\Unreal Engine\%UE_VERSION%"
set VALUE_NAME=InstalledDirectory

for /f "usebackq tokens=2*" %%A in (`reg query %KEY_NAME% /v %VALUE_NAME% /reg:64`) do set UE4_ROOT=%%B
if not defined UE4_ROOT goto error_unreal_no_found

rem Set the visual studio solution directory
rem
set CARLA_FOLDER=%ROOT_PATH%Unreal\CarlaUE4\
pushd "%CARLA_FOLDER%"

rem Clear binaries and intermediates generated by the build system
rem
if %REMOVE_INTERMEDIATE% == true (
    echo %FILE_N% cleaning "%CARLA_FOLDER%Saved"
    if exist "%CARLA_FOLDER%Saved" rmdir /S /Q "%CARLA_FOLDER%Saved"

    echo %FILE_N% cleaning "%CARLA_FOLDER%Build"
    if exist "%CARLA_FOLDER%Build" rmdir /S /Q "%CARLA_FOLDER%Build"

    echo %FILE_N% cleaning "%CARLA_FOLDER%Binaries"
    if exist "%CARLA_FOLDER%Binaries" rmdir /S /Q "%CARLA_FOLDER%Binaries"

    echo %FILE_N% cleaning "%CARLA_FOLDER%Intermediate"
    if exist "%CARLA_FOLDER%Intermediate" rmdir /S /Q "%CARLA_FOLDER%Intermediate"

    echo %FILE_N% cleaning "%CARLA_FOLDER%Plugins\Carla\Binaries"
    if exist "%CARLA_FOLDER%Plugins\Carla\Binaries" rmdir /S /Q "%CARLA_FOLDER%Plugins\Carla\Binaries"

    echo %FILE_N% cleaning "%CARLA_FOLDER%Plugins\Carla\Intermediate"
    if exist "%CARLA_FOLDER%Plugins\Carla\Intermediate" rmdir /S /Q "%CARLA_FOLDER%Plugins\Carla\Intermediate"

    echo %FILE_N% cleaning "%CARLA_FOLDER%.vs"
    if exist "%CARLA_FOLDER%CarlaUE4.sln" rmdir /S /Q "%CARLA_FOLDER%.vs"

    echo %FILE_N% cleaning "%CARLA_FOLDER%CarlaUE4.sln"
    if exist "%CARLA_FOLDER%CarlaUE4.sln" del /F /Q "%CARLA_FOLDER%CarlaUE4.sln" 1>nul
)

rem Generate Visual Studio solution
rem
set genVsSolution=false

if not exist "%CARLA_FOLDER%CarlaUE4.sln" set genVsSolution=true
if %REMOVE_INTERMEDIATE% == true set genVsSolution=false

if %genVsSolution% == true (
    call "%UE4_ROOT%/Engine/Binaries/DotNET/UnrealBuildTool.exe"  -projectfiles^
        -project="%CARLA_FOLDER%CarlaUE4.uproject"^
        -game -rocket -progress

    call "%UE4_ROOT%/Engine/Binaries/Win64/UE4Editor.exe" "%CARLA_FOLDER%CarlaUE4.uproject"^
        -targetplatform=WindowsNoEditor^
        -run=cook
)

rem Build and launch Carla
rem
if %BUILD_CARLAUE4% == true (
    echo %FILE_N% Builing and starting Carla...

    call MsBuild.exe "%CARLA_FOLDER%CarlaUE4.sln" /m /p:configuration=Development /p:platform=Win64
    if errorlevel 1 goto error_build

    start "" "%CARLA_FOLDER%Binaries\Win64\CarlaUE4.exe"
)

rem Build and launch Carla Editor
rem
if %BUILD_CARLAUE4_EDITOR% == true (
    echo %FILE_N% Builing and starting Carla Editor...

    call MsBuild.exe "%CARLA_FOLDER%CarlaUE4.sln" /m /p:configuration="Development Editor" /p:platform=Win64
    if errorlevel 1 goto error_build

    call "%CARLA_FOLDER%CarlaUE4.uproject"
)

rem Launch Carla Editor
rem
if %LAUNCH_UE4_EDITOR% == true (
    echo %FILE_N% Launching Unreal Editor...
    call "%CARLA_FOLDER%CarlaUE4.uproject"
)

goto good_exit

rem ============================================================================
rem -- Messages and Errors -----------------------------------------------------
rem ============================================================================

:error_build
    echo.
    echo %FILE_N% There was a problem building Carla.
    echo %FILE_N% Please read the screen log for more information.
    goto bad_exit

:error_unreal_no_found
    echo.
    echo %FILE_N% Unreal Engine %UE_VERSION% not detected
    goto bad_exit

:good_exit
    endlocal
    exit /b 0

:bad_exit
    endlocal
    exit /b %errorlevel%
