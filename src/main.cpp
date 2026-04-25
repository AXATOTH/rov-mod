#include <jni.h>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <unistd.h>
#include <thread>
#include <string.h>

#include "il2cpp.h"
#include "hacks.h"
#include "menu.h"
#include "overlay.h"

#define LOG_TAG "ROV_Main"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// ============================================================
// JNI Entry Points - Called from Java overlay service
// ============================================================

extern "C" {

// Initialize overlay with Surface
JNIEXPORT void JNICALL
Java_com_rov_minimap_NativeBridge_init(JNIEnv* env, jclass clazz, jobject surface) {
    ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
    if (window) {
        Overlay::getInstance().init(window);
        Overlay::getInstance().start();
        LOGI("Overlay started");
    } else {
        LOGE("Failed to get ANativeWindow");
    }
}

// Handle touch events - returns true if consumed
JNIEXPORT jboolean JNICALL
Java_com_rov_minimap_NativeBridge_handleTouch(JNIEnv* env, jclass clazz,
                                               jint action, jfloat x, jfloat y,
                                               jint pointer_count) {
    bool consumed = Overlay::getInstance().handleTouch(action, x, y, pointer_count);
    return consumed ? JNI_TRUE : JNI_FALSE;
}

// Cleanup
JNIEXPORT void JNICALL
Java_com_rov_minimap_NativeBridge_shutdown(JNIEnv* env, jclass clazz) {
    Overlay::getInstance().stop();
    Hacks::getInstance().cleanup();
    LOGI("Shutdown complete");
}

// Get ESP status
JNIEXPORT jboolean JNICALL
Java_com_rov_minimap_NativeBridge_isEspActive(JNIEnv* env, jclass clazz) {
    return Hacks::getInstance().settings.enabled ? JNI_TRUE : JNI_FALSE;
}

// Get enemy count
JNIEXPORT jint JNICALL
Java_com_rov_minimap_NativeBridge_getEnemyCount(JNIEnv* env, jclass clazz) {
    return Hacks::getInstance().getEnemyCount();
}

} // extern "C"

// ============================================================
// Module Entry Point
// ============================================================

// Called when the shared library is loaded
__attribute__((constructor))
void on_load() {
    LOGI("=== ROV Minimap ESP v1.0.0 loaded ===");
    LOGI("Target: com.garena.game.kgth");
    
    // Initialize IL2CPP and hacks in background
    std::thread([]() {
        // Wait for game process to stabilize
        sleep(3);
        
        LOGI("Initializing IL2CPP...");
        if (il2cpp::init()) {
            LOGI("IL2CPP initialized, starting hacks...");
            Hacks::getInstance().init();
        } else {
            LOGE("IL2CPP init failed, retrying in 5s...");
            sleep(5);
            if (il2cpp::init()) {
                Hacks::getInstance().init();
            }
        }
    }).detach();
}

// Called when unloaded
__attribute__((destructor))
void on_unload() {
    LOGI("=== ROV Minimap ESP unloaded ===");
    Hacks::getInstance().cleanup();
}
