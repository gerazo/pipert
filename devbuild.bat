@echo OFF
SETLOCAL EnableDelayedExpansion

set GENERATOR=Ninja
set NINJAFLAGS=
set DIR=
set MODE=
set NODOCS=
set COVERAGE=OFF


:while
if NOT "%~1" == "" (
  if "%~1" == "-h" (
    call :Help
    goto :endscript
  ) 
  if "%~1" == "--help" (
    call :Help
    goto :endscript
  )
  if "%~1" == "-G" (
    shift
    call set GENERATOR=%%~1
    echo Using !GENERATOR! as generator.
    if  x!GENERATOR:inja^=!==x!GENERATOR! (
      echo Note, that the automatic build will not be successful without a ninja compatible generator.
    )
    goto :break
  )
  if "%~1" == "-ci" (
    set NINJAFLAGS=-v -k 99
    goto :break
  )
  if "%~1" == "-dir" (
    shift
    call set DIR=%%~1
    echo Using !DIR! as build directory.
    goto :break
  )
  if "%~1" == "-mode" (
    shift
    call set MODE=%%~1
    echo Using !MODE! as build mode.
    goto :break
  )
  if "%~1" == "-nodocs" (
    set NODOCS=YEAH
    echo Will not generate documentation.
    goto :break
  )
  echo Unknown parameter %~1. Use -h for help.
  EXIT /B 1

  :break
  shift
  goto :while
)

echo Running PipeRT build...
set result=false
if "%DIR%" == "" set result=true
if "%MODE%" == "" set result=true
if %result% == true (
  echo Running standard Debug build into build_debug folder...
  set NODOCS=YEAH
  call :RunCMake build_debug , Debug
  echo Running standard Release build into build_release folder...
  set NODOCS=
  call :RunCMake build_release , Release
) else (
  call :RunCMake %DIR% , %MODE%
)
echo Build successful.
:endscript
EXIT /B 0


:RunCMake
set DIR=%~1
set MODE=%~2
set COVERAGE=OFF
if "%MODE%" == "Debug" (
  if "%NINJAFLAGS%" == "" (
    set COVERAGE=HTML
  ) else (
    set COVERAGE=ON
  )
)
set CMAKE_OK=cmake.ok
if exist %DIR%\%CMAKE_OK% (
  echo %DIR% already exists, skipping CMake run.
  cd %DIR%
) else (
  if exist %DIR% (
    echo Detected previous failed CMake run, removing...
    rmdir /S /Q %DIR%
  )
  echo Running CMake for %DIR%...
  mkdir %DIR%
  cd %DIR%
  cmake -G %GENERATOR% -DCMAKE_BUILD_TYPE=%MODE% -DUSE_COVERAGE_ANALYSIS=%COVERAGE% ..
  if ERRORLEVEL 1 (
    echo CMake failed, exiting.
    EXIT /B 1
  )
  echo. 2>%CMAKE_OK%
)
call :RunBuild
cd ..
EXIT /B 0

:RunBuild
echo Running compilation...
ninja %NINJAFLAGS%
if ERRORLEVEL 1 (
  echo Compilation failed, exiting.
  EXIT /B 1
)
call :RunTest
EXIT /B 0

:RunTest
echo Running tests...
set CTEST_OUTPUT_ON_FAILURE=1
ninja %NINJAFLAGS% test
if ERRORLEVEL 1 (
  echo Tests failed, exiting.
  EXIT /B 1
)
call :GenerateCoverage
EXIT /B 0

:GenerateCoverage
for %%X in (gcovr.exe) do (set CFOUND=%%~$PATH:X)
if defined CFOUND (
  if NOT %COVERAGE% == OFF (
    echo Generating coverage report...
    ninja %NINJAFLAGS% coverage
    if ERRORLEVEL 1 (
      echo Coverage report generation failed, exiting.
      EXIT /B 1
    )
  )
)
call :GenerateDocs
EXIT /B 0

:GenerateDocs
for %%X in (doxygen.exe) do (set DFOUND=%%~$PATH:X)
if defined DFOUND (
  if "%NODOCS%" == "" (
    echo Generating documentation...
    ninja %NINJAFLAGS% docs
    if ERRORLEVEL 1 (
      echo Documentation generation failed, exiting.
      EXIT /B 1
    )
  )
)
EXIT /B 0

:Help 
echo Options: 
echo -h  or  --help         Give this help 
echo -G ^<cmake_generator^>   Use CMake generator instead of Ninja 
echo -ci                    CI mode: log more, do not stop on errors 
echo -dir ^<build_dir^>       Use build_dir folder for this build 
echo -mode ^<mode^>           CMake build mode (Debug / Release) 
echo -nodocs                Do not generate HTML documentation 
EXIT /B 1