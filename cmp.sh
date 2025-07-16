#!/bin/bash

set -e

# Usage info
if [[ "$1" == "--help" ]]; then
    echo "Usage: ./cmp.sh [--build] [--run] [extra g++ args...]"
    echo "  --build   Only build, do not run."
    echo "  --run     Only run, do not build."
    echo "  --help    Show this help message."
    exit 0
fi

# Blacklist warnings here (space-separated, e.g. -Wno-unused-label -Wno-unused-parameter)
BLACKLISTED_WARNINGS="-Wno-unused-label -Wno-unused-parameter -Wno-switch"

# Find all directories containing header files (.h or .hpp)
INCLUDE_DIRS=$(find . -type f \( -name '*.h' -o -name '*.hpp' \) -exec dirname {} \; | sort -u)
INCLUDE_FLAGS=""
for dir in $INCLUDE_DIRS; do
    INCLUDE_FLAGS+=" -I$dir"
done

# Gather all .cpp files
CPP_FILES=$(find . -name '*.cpp')

# Build only, run only, or both
DO_BUILD=1
DO_RUN=1
for arg in "$@"; do
    [[ "$arg" == "--build" ]] && DO_RUN=0
    [[ "$arg" == "--run" ]] && DO_BUILD=0
done

# Remove --build and --run from args passed to g++
EXTRA_ARGS=()
for arg in "$@"; do
    [[ "$arg" != "--build" && "$arg" != "--run" ]] && EXTRA_ARGS+=("$arg")
done

if [[ $DO_BUILD -eq 1 ]]; then
    echo -e "\e[34m[BUILD]\e[0m Compiling..."
    g++ -std=c++23 -Wall -Wextra -Werror=return-type $BLACKLISTED_WARNINGS $INCLUDE_FLAGS -o main $CPP_FILES "${EXTRA_ARGS[@]}"
    echo -e "\e[34m[BUILD]\e[0m \e[32mDone\e[0m."
fi

if [[ $DO_RUN -eq 1 ]]; then
    echo -e "\e[34m[RUN]\e[0m ./main"
    ./main
fi
