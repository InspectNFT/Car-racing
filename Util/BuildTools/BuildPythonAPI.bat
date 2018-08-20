@echo off
setlocal

rem BAT script that creates the client python api of LibCarla (carla.org).
rem Run it through a cmd with the x64 Visual C++ Toolset enabled.

set LOCAL_PATH=%~dp0
set "FILE_N=-[%~n0]:"

rem ============================================================================
rem -- Parse arguments ---------------------------------------------------------
rem ============================================================================

set DOC_STRING=Build and package CARLA Python API.
set "USAGE_STRING=Usage: %FILE_N% [-h^|--help] [--rebuild]  [--clean]"

set REMOVE_INTERMEDIATE=false
set BUILD_FOR_PYTHON2=false
set BUILD_FOR_PYTHON3=false

:arg-parse
if not "%1"=="" (
    if "%1"=="--rebuild" (
        set REMOVE_INTERMEDIATE=true
        set BUILD_FOR_PYTHON2=true
        set BUILD_FOR_PYTHON3=true
    )

    if "%1"=="--py2" (
        set BUILD_FOR_PYTHON2=true
    )

    if "%1"=="--py3" (
        set BUILD_FOR_PYTHON3=true
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

set PYTHON_LIB_PATH=%ROOT_PATH%PythonAPI\
set PYTHON_LIB_BUILD=%PYTHON_LIB_PATH%build\
set PYTHON_LIB_DEPENDENCIES=%PYTHON_LIB_PATH%dependencies\

if %REMOVE_INTERMEDIATE% == false (
    if %BUILD_FOR_PYTHON3% == false (
        if %BUILD_FOR_PYTHON2% == false (
          echo Nothing selected to be done.
          goto :eof
        )
    )
)

if %REMOVE_INTERMEDIATE% == true (
    echo.

    echo %FILE_N% Cleaning "%PYTHON_LIB_BUILD%"
    if exist "%PYTHON_LIB_BUILD%" rmdir /S /Q "%PYTHON_LIB_BUILD%"

    echo %FILE_N% Cleaning "%PYTHON_LIB_DEPENDENCIES%"
    if exist "%PYTHON_LIB_DEPENDENCIES%" rmdir /S /Q "%PYTHON_LIB_DEPENDENCIES%"

    echo %FILE_N% Cleaning "%PYTHON_LIB_PATH%\dist"
    if exist "%PYTHON_LIB_PATH%dist" rmdir /S /Q "%PYTHON_LIB_PATH%dist"
)

cd "%PYTHON_LIB_PATH%"
if exist "%PYTHON_LIB_PATH%dist" goto already_installed

rem ============================================================================
rem -- Check for py ------------------------------------------------------------
rem ============================================================================

where py 1>nul
if %errorlevel% neq 0 goto error_py

rem Build for Python 2
rem
if %BUILD_FOR_PYTHON2%==true (
    goto py2_not_supported
)

rem Build for Python 2
rem
if %BUILD_FOR_PYTHON3%==true (
    echo Building Python API for Python 3.
    call py -3 setup.py bdist_egg
    if %errorlevel% neq 0 goto error_build_egg
)

goto success

rem ============================================================================
rem -- Messages and Errors -----------------------------------------------------
rem ============================================================================

:success
    echo.
    if %BUILD_FOR_PYTHON3%==true echo %FILE_N% Carla lib for python has been successfully installed in "%PYTHON_LIB_PATH%dist"!
    goto good_exit

:already_installed
    echo.
    echo %FILE_N% already installed in "%PYTHON_LIB_PATH%dist"
    goto good_exit

:py2_not_supported
    echo.
    echo %FILE_N% Python 2 is not currently suported in Windows.
    goto bad_exit

:error_py
    echo.
    echo %FILE_N% [PY ERROR] An error ocurred while executing the py.
    echo %FILE_N% [PY ERROR] Possible causes:
    echo %FILE_N%              - Make sure "py" is installed.
    echo %FILE_N%              - Make sure it is available on your Windows "py".
    goto bad_exit

:error_build_egg
    echo.
    echo %FILE_N% An error occurred while building the egg file.
    goto bad_exit

:good_exit
    endlocal
    exit /b 0

:bad_exit
    endlocal
    exit /b %errorlevel%
