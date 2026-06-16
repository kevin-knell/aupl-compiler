#!/bin/bash
set -e

usage() {
cat <<EOF
Usage:
  ./build.sh
  ./build.sh --target=cmp
  ./build.sh --target=vm
  ./build.sh --target=lib

Targets:
  lib   Build library test executable
  vm    Build VM executable
  cmp   Build compiler executable

No target specified => build all.
EOF
}

TARGET="all"

for arg in "$@"; do
    case "$arg" in
        --help)
            usage
            exit 0
            ;;
        --target=cmp)
            TARGET="cmp"
            ;;
        --target=vm)
            TARGET="vm"
            ;;
        --target=lib)
            TARGET="lib"
            ;;
        *)
            echo "Unknown argument: $arg"
            exit 1
            ;;
    esac
done

WARNINGS="
-Wall
-Wextra
-Wconversion
-Werror=return-type
-Wold-style-cast
-Wnon-virtual-dtor
-Wsuggest-override
-Wduplicated-cond
-Wlogical-op
-Wunused-result
-Wunused-value
-Wunused-function
"

BLACKLISTED_WARNINGS=""

INCLUDE_FLAGS="-isystem thirdparty"

while IFS= read -r dir; do
    INCLUDE_FLAGS+=" -I$dir"
done < <(
    find . -type f \( -name '*.h' -o -name '*.hpp' \) \
    -exec dirname {} \; | sort -u
)

COMMON_LINK="
-lncurses
-lglfw
-lvulkan
-ldl
-lpthread
-lX11
-lXxf86vm
-lXrandr
-lXi
-lSDL2
"

build_lib() {
    echo -e "\e[34m[COMPILE]\e[0m lib"

    mkdir -p build/lib

    CPP_FILES=$(find vm/native -name '*.cpp')
    CPP_FILES+=" lib_main.cpp"

    for f in $CPP_FILES; do
        obj="build/lib/$(basename "$f" .cpp).o"

        g++ \
            -c \
            -std=c++23 \
            -O3 \
            -flto=auto \
            -g3 \
            $WARNINGS \
            $BLACKLISTED_WARNINGS \
            $INCLUDE_FLAGS \
            "$f" \
            -o "$obj"
    done

    echo -e "\e[34m[LINK]\e[0m lib"

    g++ \
        -O3 \
        -flto=auto \
        build/lib/*.o \
        -o build/lib/auplib \
        $COMMON_LINK
}

build_vm() {
    build_lib

    echo -e "\e[34m[COMPILE]\e[0m vm"

    mkdir -p build/vm

    CPP_FILES=$(find vm -name '*.cpp')
    CPP_FILES+=" vm_main.cpp"

    for f in $CPP_FILES; do
        obj="build/vm/$(basename "$f" .cpp).o"

        g++ \
            -c \
            -std=c++23 \
            -O3 \
            -flto=auto \
            -g3 \
            -DVM_ONLY \
            $WARNINGS \
            $BLACKLISTED_WARNINGS \
            $INCLUDE_FLAGS \
            "$f" \
            -o "$obj"
    done

    echo -e "\e[34m[LINK]\e[0m vm"

    g++ \
        -O3 \
        -flto=auto \
        build/vm/*.o \
        -o build/vm/aupl-vm \
        $COMMON_LINK
}

build_cmp() {
    build_vm

    echo -e "\e[34m[COMPILE]\e[0m compiler"

    mkdir -p build/cmp

    CPP_FILES=$(find compiler vm -name '*.cpp')
    CPP_FILES+=" cmp_main.cpp"

    for f in $CPP_FILES; do
        obj="build/cmp/$(basename "$f" .cpp).o"

        g++ \
            -c \
            -std=c++23 \
            -O3 \
            -flto=auto \
            -g3 \
            -DCOMPILER \
            $WARNINGS \
            $BLACKLISTED_WARNINGS \
            $INCLUDE_FLAGS \
            "$f" \
            -o "$obj"
    done

    echo -e "\e[34m[LINK]\e[0m compiler"

    g++ \
        -O3 \
        -flto=auto \
        build/cmp/*.o \
        -o build/cmp/aupl \
        $COMMON_LINK
}

case "$TARGET" in
    lib) build_lib ;;
    vm) build_vm ;;
    cmp) build_cmp ;;
    all) build_cmp ;;
esac