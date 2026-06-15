#!/bin/bash

set -e

usage() {
cat <<EOF
Usage:
  ./project.sh --pj=<project>
EOF
}

PROJECT=""

for arg in "$@"; do
    case "$arg" in
        --help)
            usage
            exit 0
            ;;
        --pj=*)
            PROJECT="${arg#*=}"
            ;;
        *)
            echo "Unknown argument: $arg"
            exit 1
            ;;
    esac
done

if [[ -z "$PROJECT" ]]; then
    echo "Missing --pj"
    exit 1
fi

./aupl.sh --pj=$PROJECT

CPP_DIR="examples/$PROJECT/build/cpp"

if [[ ! -f "$CPP_DIR/output.cpp" ]]; then
    echo "Missing generated source."
    exit 1
fi

WARNINGS="-Werror=return-type"

INCLUDE_FLAGS="-isystem thirdparty"

while IFS= read -r dir; do
    INCLUDE_FLAGS+=" -I$dir"
done < <(
    find . -type f \( -name '*.h' -o -name '*.hpp' \) \
    -exec dirname {} \; | sort -u
)

CPP_FILES=$(find vm -name '*.cpp')
CPP_FILES+=" $CPP_DIR/output.cpp"

echo -e "\e[34m[COMPILE]\e[0m $PROJECT (C++)"

for f in $CPP_FILES; do
    obj="$CPP_DIR/$(basename "$f" .cpp).o"

    g++ \
        -c \
        -std=c++23 \
        -O3 \
        -DVM_ONLY \
        $WARNINGS \
        $INCLUDE_FLAGS \
        "$f" \
        -o "$obj"
done

echo -e "\e[34m[LINK]\e[0m $PROJECT (C++)"

g++ \
    -O3 \
    "$CPP_DIR"/*.o \
    -o "$CPP_DIR/aupl" \
    -lncurses \
    -lglfw \
    -lvulkan \
    -ldl \
    -lpthread \
    -lX11 \
    -lXxf86vm \
    -lXrandr \
    -lXi \
    -lSDL2

echo -e "\e[36m[BUILT]\e[0m $CPP_DIR/aupl"