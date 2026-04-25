#include <jni.h>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <unistd.h>
#include <thread>
#include <string.h>

#include "zygisk.hpp"
#include "il2cpp.h"
#include "hacks.h"
#include "menu.h"
#include "overlay.h"

#define LOG_TAG "ROV_Main"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

using zygisk::Api;
using zygisk::AppSpecializeArgs;

class ROVModule : public zygisk::ModuleBase {
public:
    void onLoad(Api* api, JNIEnv* env) override {
        this->api = api;
        this->env = env;
    }

    void preAppSpecialize(AppSpecializeArgs* args) override {
        const char* pkg = env->GetStringUTFChars(args->nice_name, nullptr);
        if (pkg) {
            is_target = (strstr(pkg, "com.garena.game.kgth") != nullptr);
            env->ReleaseStringUTFChars(args->nice_name, pkg);
        }
        if (!is_target) {
            api->setOption(zygisk::Option::DLCLOSE_MODULE_LIBRARY);
        }
    }

    void postAppSpecialize(const AppSpecializeArgs* args) override {
        if (!is_target) return;
        LOGI("=== ROV Minimap ESP loaded ===");
        std::thread([]() {
            sleep(3);
            if (il2cpp::init()) {
                Hacks::getInstance().init();
            }
        }).detach();
    }

private:
    Api* api = nullptr;
    JNIEnv* env = nullptr;
    bool is_target = false;
};

REGISTER_ZYGISK_MODULE(ROVModule)

extern "C" {
JNIEXPORT void JNICALL
Java_com_rov_minimap_NativeBridge_init(JNIEnv* env, jclass clazz, jobject surface) {
    ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
    if (window) {
        Overlay::getInstance().init(window);
        Overlay::getInstance().start();
    }
}

JNIEXPORT jboolean JNICALL
Java_com_rov_minimap_NativeBridge_handleTouch(JNIEnv* env, jclass clazz,
    jint action, jfloat x, jfloat y, jint pointer_count) {
    return Overlay::getInstance().handleTouch(action, x, y, pointer_count);
}

JNIEXPORT void JNICALL
Java_com_rov_minimap_NativeBridge_shutdown(JNIEnv* env, jclass clazz) {
    Overlay::getInstance().stop();
    Hacks::getInstance().cleanup();
}
} // extern "C"
