#!/bin/bash
set -e

PROJECT=""
for arg in "$@"; do
	PROJECT="$arg"
done

if [[ -z "$PROJECT" ]]; then
    echo "Missing --pj"
    exit 1
fi

./build.sh
mkdir -p ./examples/$PROJECT/build
./build/cmp/aupl --gen-no-bc --gen-cpp -o ./examples/$PROJECT/build ./examples/$PROJECT
./project.sh --pj=$PROJECT
echo -e "compiling shaders..."

find ./shaders -type f ! -name '*.spv' -print0 |
while IFS= read -r -d '' shader; do
	glslc "$shader" -o "$shader.spv"
done

./examples/$PROJECT/build/cpp/aupl