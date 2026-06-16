#!/bin/bash
set -e

usage() {
cat <<EOF
Usage:
  ./aupl.sh --pj=<project>
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

if [[ ! -d "examples/$PROJECT" ]]; then
    echo "Project not found: $PROJECT"
    exit 1
fi

mkdir -p "examples/$PROJECT/build/cpp"

./build/cmp/aupl \
    -o "examples/$PROJECT/build/cpp/output" \
    "examples/$PROJECT"