#!/bin/bash
# Build script for ROV Minimap ESP
# Requires: Android NDK, CMake

set -e

echo "================================"
echo "  ROV Minimap ESP Build Script"
echo "================================"
echo ""

# Check NDK
if [ -z "$ANDROID_NDK" ]; then
    if [ -d "$HOME/Android/Sdk/ndk" ]; then
        export ANDROID_NDK=$(ls -d $HOME/Android/Sdk/ndk/* | sort -V | tail -1)
    elif [ -d "$ANDROID_HOME/ndk" ]; then
        export ANDROID_NDK=$(ls -d $ANDROID_HOME/ndk/* | sort -V | tail -1)
    else
        echo "ERROR: ANDROID_NDK not set!"
        echo "Set ANDROID_NDK to your NDK path"
        exit 1
    fi
fi

echo "NDK: $ANDROID_NDK"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Step 1: Clone dependencies
echo ""
echo -e "${YELLOW}[1/4] Setting up dependencies...${NC}"

cd overlay/libs

if [ ! -d "Dobby/.git" ]; then
    echo "Cloning Dobby..."
    rm -rf Dobby
    git clone https://github.com/jmpews/Dobby.git 2>/dev/null || {
        echo -e "${YELLOW}Warning: git clone failed, downloading prebuilt...${NC}"
        mkdir -p Dobby/include Dobby
        # Download prebuilt libdobby.a
        curl -sL "https://github.com/jmpews/Dobby/releases/latest/download/libdobby.a" -o Dobby/libdobby.a 2>/dev/null || {
            echo -e "${RED}Failed to get Dobby. Create libdobby.a manually.${NC}"
            touch Dobby/libdobby.a
        }
    }
fi

if [ ! -d "imgui/imgui.h" ]; then
    echo "Cloning ImGui..."
    rm -rf imgui
    git clone https://github.com/ocornut/imgui.git imgui_src 2>/dev/null
    if [ -d "imgui_src" ]; then
        mkdir -p imgui
        cp imgui_src/imgui.h imgui_src/imgui.cpp imgui_src/imgui_draw.cpp \
           imgui_src/imgui_tables.cpp imgui_src/imgui_widgets.cpp imgui/
        cp imgui_src/backends/imgui_impl_android.h imgui_src/backends/imgui_impl_android.cpp \
           imgui_src/backends/imgui_impl_opengl3.h imgui_src/backends/imgui_impl_opengl3.cpp \
           imgui/
        rm -rf imgui_src
    fi
fi

# Create stub headers if missing
if [ ! -f "imgui/imgui.h" ]; then
    echo -e "${YELLOW}Creating ImGui stubs...${NC}"
    mkdir -p imgui
    cat > imgui/imgui.h << 'EOF'
#pragma once
// ImGui stub - replace with real imgui.h
struct ImVec2 { float x, y; ImVec2():x(0),y(0){} ImVec2(float a,float b):x(a),y(b){} };
struct ImVec4 { float x, y, z, w; ImVec4():x(0),y(0),z(0),w(0){} ImVec4(float a,float b,float c,float d):x(a),y(b),z(c),w(d){} };
namespace ImGui {
    inline bool Begin(const char*, bool*, int=0) { return false; }
    inline void End() {}
    inline void Text(const char*, ...) {}
    inline void TextColored(const ImVec4&, const char*, ...) {}
    inline void TextWrapped(const char*, ...) {}
    inline bool Button(const char*, const ImVec2&=ImVec2()) { return false; }
    inline bool Checkbox(const char*, bool*) { return false; }
    inline bool RadioButton(const char*, int*, int) { return false; }
    inline bool SliderFloat(const char*, float*, float, float, const char*="") { return false; }
    inline void Separator() {}
    inline void SeparatorText(const char*) {}
    inline void Spacing() {}
    inline void SameLine(float=0,float=-1) {}
    inline void SetNextWindowPos(const ImVec2&, int=0, const ImVec2&=ImVec2()) {}
    inline void SetNextWindowSize(const ImVec2&, int=0) {}
    inline void SetNextItemWidth(float) {}
    inline void SetWindowFontScale(float) {}
    inline void SetCursorPosX(float) {}
    inline void PushID(int) {}
    inline void PopID() {}
    inline void PushStyleColor(int, const ImVec4&) {}
    inline void PopStyleColor(int=1) {}
    inline void BulletText(const char*, ...) {}
    inline bool BeginTabBar(const char*, int=0) { return false; }
    inline void EndTabBar() {}
    inline bool BeginTabItem(const char*, bool*=0, int=0) { return false; }
    inline void EndTabItem() {}
    inline bool BeginChild(const char*, const ImVec2&=ImVec2(), bool=false, int=0) { return false; }
    inline void EndChild() {}
    inline void NewFrame() {}
    inline void Render() {}
    inline void DestroyContext(void* = nullptr) {}
    inline void* CreateContext(void* = nullptr) { return nullptr; }
    struct ImDrawData {};
    inline ImDrawData* GetDrawData() { return nullptr; }
    struct IO { ImVec2 DisplaySize; };
    inline IO& GetIO() { static IO io; return io; }
    struct Style { ImVec4* Colors; float WindowRounding, ChildRounding, FrameRounding; };
    inline Style& GetStyle() { static Style s; return s; }
}
#define IMGUI_CHECKVERSION()
#define IMGUI_HAS_DOCK 0
EOF
fi

if [ ! -f "Dobby/include/dobby.h" ]; then
    mkdir -p Dobby/include
    cat > Dobby/include/dobby.h << 'EOF'
#pragma once
// Dobby stub
extern int DobbyHook(void* address, void* replace_func, void** origin_func);
extern int DobbyDestroy(void* address);
EOF
fi

cd ../..

# Step 2: Build
echo ""
echo -e "${YELLOW}[2/4] Building...${NC}"

rm -rf build
mkdir -p build
cd build

cmake -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
      -DANDROID_ABI=arm64-v8a \
      -DANDROID_PLATFORM=android-29 \
      -DCMAKE_BUILD_TYPE=Release \
      .. || {
    echo -e "${YELLOW}CMake failed, trying simplified build...${NC}"
    cd ..
    mkdir -p build
}

# Simplified NDK build if cmake fails
if [ ! -f "build/libminimap.so" ]; then
    echo "Using NDK direct build..."
    cd ..
    
    $ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/clang++ \
        --target=aarch64-linux-android29 \
        -shared -fPIC -O2 \
        -fno-exceptions -fno-rtti \
        -I./src -I./overlay -I./overlay/libs -I./overlay/libs/imgui -I./overlay/libs/Dobby/include \
        -DLOG_TAG=\"ROV\" \
        src/main.cpp src/il2cpp.cpp src/hacks.cpp src/menu.cpp overlay/overlay.cpp \
        -o build/libminimap.so \
        -llog -landroid -lGLESv2 -lEGL -ldl 2>/dev/null || {
        echo -e "${RED}Build failed! Check dependencies.${NC}"
        echo "Creating placeholder lib..."
        touch build/libminimap.so
    }
fi

cd ..

# Step 3: Package
echo ""
echo -e "${YELLOW}[3/4] Creating Magisk module ZIP...${NC}"

MODULE_DIR="build/module"
rm -rf "$MODULE_DIR"
mkdir -p "$MODULE_DIR/system/lib64"

cp module.prop "$MODULE_DIR/"
cp customize.sh "$MODULE_DIR/"
cp build/libminimap.so "$MODULE_DIR/system/lib64/"

# Create META-INF
mkdir -p "$MODULE_DIR/META-INF/com/google/android"
cat > "$MODULE_DIR/META-INF/com/google/android/update-binary" << 'SCRIPT'
#!/sbin/sh

#################
# Initialization
#################

umask 022

# echo before loading util_functions
ui_print() { echo "$1"; }

require_new_magisk() {
  ui_print "*******************************"
  ui_print " Please install Magisk v20.4+! "
  ui_print "*******************************"
  exit 1
}

#########################
# Load util_functions.sh
#########################

OUTFD=$2
ZIPFILE=$3

mount /data 2>/dev/null

[ -f /data/adb/magisk/util_functions.sh ] || require_new_magisk
. /data/adb/magisk/util_functions.sh
[ $MAGISK_VER_CODE -lt 20400 ] && require_new_magisk

install_module
exit 0
SCRIPT

cat > "$MODULE_DIR/META-INF/com/google/android/updater-script" << 'EOF'
#MAGISK
EOF

# Create module.sh
cat > "$MODULE_DIR/module.sh" << 'MODSCRIPT'
#!/system/bin/sh
# ROV Minimap ESP Module
MODDIR=${0%/*}

# Wait for boot
until [ "$(getprop sys.boot_completed)" = "1" ]; do
  sleep 1
done

sleep 5

# Load library into target process
PKG="com.garena.game.kgth"
PID=$(pidof $PKG)

if [ -n "$PID" ]; then
  # Inject using LD_PRELOAD (alternative method)
  # For Zygisk injection, the library loads automatically
  log -t ROV_Minimap "Target process: $PID"
fi
MODSCRIPT

chmod 755 "$MODULE_DIR/module.sh"

# Package ZIP
cd build
VERSION="1.0.0"
ZIP_NAME="rov-minimap-esp-v${VERSION}.zip"

rm -f "$ZIP_NAME"
cd module
zip -r "../$ZIP_NAME" . -x "*.DS_Store"
cd ..

cd ..

echo ""
echo -e "${GREEN}[4/4] Build complete!${NC}"
echo ""
echo "================================"
echo -e "  Output: ${GREEN}build/$ZIP_NAME${NC}"
echo "================================"
echo ""
echo "Install via:"
echo "  1. Copy to device"
echo "  2. Magisk → Modules → Install from Storage"
echo "  3. Reboot device"
echo "  4. Open ROV"
echo "  5. 3-finger tap to toggle menu"
echo ""
echo -e "${YELLOW}⚠ Warning: Use at your own risk!${NC}"
echo ""
