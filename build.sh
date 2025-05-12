SOURCE_DIR=$(pwd)
BUILD_DIR="$SOURCE_DIR/build"

PRESET="default"
if [ $# -gt 0 ]; then
  PRESET="$1"
fi

if [ ! -d "$BUILD_DIR" ]
then
	mkdir "$BUILD_DIR"
	cmake -S "$SOURCE_DIR" -B "$BUILD_DIR" --preset "$PRESET"
fi

cmake --build "$BUILD_DIR"

