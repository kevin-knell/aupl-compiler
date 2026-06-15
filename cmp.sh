PROJECT=""
for arg in "$@"; do
	PROJECT="$arg"
done

if [[ -z "$PROJECT" ]]; then
    echo "Missing --pj"
    exit 1
fi

./build.sh
./project.sh --pj=$PROJECT
./examples/$PROJECT/build/cpp/aupl