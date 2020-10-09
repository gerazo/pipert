#!/bin/sh

GENERATOR="Ninja"
NINJAFLAGS=""
DIR=""
MODE=""
NODOCS=""

help () {
  echo "Options:"
  echo " -h  or  --help         Give this help"
  echo " -G <cmake_generator>   Use CMake generator instead of Ninja"
  echo " -ci                    CI mode: log more, do not stop on errors"
  echo " -dir <build_dir>       Use build_dir folder for this build"
  echo " -mode <mode>           CMake build mode (Debug / Release)"
  echo " -nodocs                Do not generate HTML documentation"
  exit 1
}

generate_docs () {
  if [ "$NODOCS" = "" ] && command -v doxygen; then
    echo "Generating documentation..."
    ninja $NINJAFLAGS docs
    if [ "$?" != "0" ]; then
      echo "Documentation generation failed, exiting."
      exit 7
    fi
  fi
}

generate_coverage () {
  if [ "$COVERAGE" != "OFF" ] && command -v gcovr; then
    echo "Generating coverage report..."
    ninja $NINJAFLAGS coverage
    if [ "$?" != "0" ]; then
      echo "Coverage report generation failed, exiting."
      exit 6
    fi
  fi
  generate_docs
}

run_test () {
  echo "Running tests..."
  ninja $NINJAFLAGS test
  if [ "$?" != "0" ]; then
    echo "Tests failed, exiting."
    exit 5
  fi
  generate_coverage
}

run_build () {
  echo "Running compilation..."
  ninja $NINJAFLAGS
  if [ "$?" != "0" ]; then
    echo "Compilation failed, exiting."
    exit 4
  fi
  run_test
}

run_cmake () {
  DIR="$1"
  MODE="$2"

  COVERAGE="OFF"
  if [ "$MODE" = "Debug" ]; then
    if [ "$NINJAFLAGS" = "" ]; then
      COVERAGE="HTML"
    else
      COVERAGE="ON"
    fi
  fi

  if [ -d "$DIR" ]; then
    echo "\"$DIR\" already exists, skipping CMake run."
    cd "$DIR"
  else
    echo "Running CMake for \"$DIR\" ..."
    mkdir "$DIR"
    cd "$DIR"
    cmake -G "$GENERATOR" -DCMAKE_BUILD_TYPE="$MODE" -DUSE_COVERAGE_ANALYSIS="$COVERAGE" "$DOCOPTS" ..
    if [ "$?" != "0" ]; then
      echo "CMake failed, exiting."
      exit 3
    fi
  fi
  run_build
  cd ..
}



echo "PipeRT build script"

while [ "$1" != "" ]; do
  case "$1" in
    "-h")
      help
      ;;
    "--help")
      help
      ;;
    "-G")
      shift
      GENERATOR="$1"
      echo "Using \"$GENERATOR\" as generator."
      if ! echo "$GENERATOR" | grep inja ; then
        echo "Note, that the automatic build will not be successful without a ninja compatible generator."
      fi
      ;;
    "-ci")
      NINJAFLAGS="-v -k 99"
      ;;
    "-dir")
      shift
      DIR="$1"
      echo "Using \"$DIR\" as build directory."
      ;;
    "-mode")
      shift
      MODE="$1"
      echo "Using \"$MODE\" as build mode."
      ;;
    "-nodocs")
      NODOCS="YEAH"
      echo "Will not generate documentation."
      ;;
    *)
      echo "Unknown parameter \"$1\". Use -h for help."
      exit 2
      ;;
  esac
  shift
done

echo "Running PipeRT build..."

if [ -z "$DIR" ] || [ -z "$MODE" ]; then
  echo "Running standard Debug build into build_debug folder..."
  NODOCS="YEAH"
  run_cmake "build_debug" "Debug"
  echo "Running standard Release build into build_release folder..."
  NODOCS=""
  run_cmake "build_release" "Release"
else
  run_cmake "$DIR" "$MODE"
fi

echo "Build successful."
exit 0
