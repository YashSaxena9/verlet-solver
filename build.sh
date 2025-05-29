#!/bin/bash

# === Configuration ===
CXX=clang++
CXXFLAGS="-std=c++17 -O2 -Wall -Wextra"
INCLUDE_DIRS=(
    "src"
    "src/deps/raylib/include"
)
SRC_DIRS=(
    "src"
    "src/Engine"
    "src/utils"
)
RAYLIB_LIB="src/deps/raylib/lib/libraylib.a"
OUT_DIR="bin"
OUT_BIN="$OUT_DIR/app"

# === Platform-specific flags ===
# PLATFORM_LIBS="-lGL -lm -lpthread -ldl -lrt -lX11" # Linux
PLATFORM_LIBS="-framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo" # macOS

# === Build process ===
echo "[+] Building..."

# Gather all .cpp source files
SRC_FILES=$(
    for dir in "${SRC_DIRS[@]}"; do
        find "$dir" -name '*.cpp'
    done | sort -u
)

# Construct include flags
INCLUDE_FLAGS=""
for dir in "${INCLUDE_DIRS[@]}"; do
    INCLUDE_FLAGS+=" -I$dir"
done

# Create output directory if needed
mkdir -p "$OUT_DIR"

# Compile
echo "$SRC_FILES"
$CXX \
    $CXXFLAGS \
    $INCLUDE_FLAGS \
    $SRC_FILES \
    $RAYLIB_LIB \
    $PLATFORM_LIBS \
    -o "$OUT_BIN"

# Done
if [ $? -eq 0 ]; then
    echo "[✓] Build succeeded: $OUT_BIN"
else
    echo "[✗] Build failed."
fi
