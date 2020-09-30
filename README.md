# PipeRT
Real-Time meets High-Delay: A hybrid scheduling and dataflow framework for DSP applications

## Features
 - High performance
   - Multithreaded scheduler with worker pool
   - In-place running in memory, allocations are only made in the setup phase, not in the running phase
   - Uses mostly non-blocking algorithms avoiding OS locks
 - Easy to use
   - No need for user to have own mutexes or any kind of synchronization primitives
   - Nice, compact OOP API with templates to support own data types
   - Cross-platform C++11 code tested on gcc and clang
 - Versatile
   - Configurable buffers to support low- and high delay units
   - Supports stateless and stateful (accumulate) tasks
   - Supports user controlled threads besides own thread pool to support UI, I/O or special thread affinity
   - Supports circles (feedback) in the pipeline chain
   - Supports multiple entry and exit (even reentry) points for distributed workflows
 - Measurable (coming soon)
   - Supports measurement-driven development via its own, minimal impact autoprofiling system
   - Has a measurement API which exposes autoprofiling capabilities of the whole pipeline
   - Pluggable measurements: The user can fit any logging mechanism to the profiler

## Build

You need the following packages installed on your system to run the build:
 - `git` for downloading this repo
 - `cmake` and `ninja-build` for running the build
 - `gcc`, `g++` for compiling with GCC or
 - `clang`, `clang++` and `llvm` for compiling with Clang
Optionally:
 - `clang-format` is used to keep our coding conventions

How to start:
 1. Download this repo
 2. Run `./run.sh`
 3. Find the compiled binaries in the generated build* folders
 4. Go into build_debug and use `ninja` and `ninja test` from there during development

Have fun!
