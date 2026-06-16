PROJECT=""
for arg in "$@"; do
	PROJECT="$arg"
done

if [[ -z "$PROJECT" ]]; then
    echo "Missing --pj"
    exit 1
fi

echo -e "compiling shaders..."
find ./shaders -type f ! -name '*.spv' -print0 |
while IFS= read -r -d '' shader; do
	glslc "$shader" -o "$shader.spv"
done

./build.sh
./project.sh --pj=$PROJECT
./examples/$PROJECT/build/cpp/aupl