cmake_minimum_required(VERSION 3.14)
project(minimap_esp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -fno-exceptions -fno-rtti -fvisibility=hidden")

# Include paths
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/overlay
    ${CMAKE_SOURCE_DIR}/overlay/libs
    ${CMAKE_SOURCE_DIR}/overlay/libs/imgui
)

# ImGui sources
set(IMGUI_SOURCES
    overlay/libs/imgui/imgui.cpp
    overlay/libs/imgui/imgui_draw.cpp
    overlay/libs/imgui/imgui_tables.cpp
    overlay/libs/imgui/imgui_widgets.cpp
)

# Main sources
set(SOURCES
    src/main.cpp
    src/il2cpp.cpp
    src/hacks.cpp
    src/menu.cpp
    overlay/overlay.cpp
    ${IMGUI_SOURCES}
)

add_library(minimap SHARED ${SOURCES})

# Find libraries with fallback or error messaging
find_library(log-lib log)
find_library(android-lib android)
find_library(GLESv2-lib GLESv2)
find_library(EGL-lib EGL)

# Ensure they are found, if not, provide a helpful error message
if(NOT log-lib OR NOT android-lib OR NOT GLESv2-lib OR NOT EGL-lib)
    message(FATAL_ERROR "Could not find Android system libraries. "
            "Please ensure you are using the Android NDK toolchain file (-DCMAKE_TOOLCHAIN_FILE=...)")
endif()

target_link_libraries(minimap
    ${log-lib}
    ${android-lib}
    ${GLESv2-lib}
    ${EGL-lib}
    -ldl
)
