#!/bin/bash
set -e

usage() {
cat <<EOF
Usage:
  ./run.sh --target=lib
  ./run.sh --target=vm
  ./run.sh --target=cmp
EOF
}

TARGET=""

for arg in "$@"; do
    case "$arg" in
        --help)
            usage
            exit 0
            ;;
        --target=lib)
            TARGET="lib"
            ;;
        --target=vm)
            TARGET="vm"
            ;;
        --target=cmp)
            TARGET="cmp"
            ;;
        *)
            echo "Unknown argument: $arg"
            exit 1
            ;;
    esac
done

case "$TARGET" in
    lib)
        ./build/lib/auplib
        ;;
    vm)
        ./build/vm/aupl-vm
        ;;
    cmp)
        ./build/cmp/aupl
        ;;
    *)
        echo "Missing --target"
        exit 1
        ;;
esac