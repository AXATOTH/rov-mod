#include "overlay.h"
#include "menu.h"
#include "imgui.h"
#include "imgui_impl_android.h"
#include "imgui_impl_opengl3.h"
#include <android/log.h>
#include <chrono>

#define LOG_TAG "Overlay"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

bool Overlay::init(ANativeWindow* window) {
    this->window = window;
    
    if (!initEGL()) {
        LOGE("Failed to init EGL");
        return false;
    }
    
    // ImGui init
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;  // No imgui.ini
    
    // ImGui backends
    ImGui_ImplAndroid_Init(window);
    ImGui_ImplOpenGL3_Init("#version 100");
    
    // Init mod menu
    ModMenu::getInstance().init();
    
    LOGI("Overlay initialized");
    return true;
}

void Overlay::start() {
    if (running) return;
    running = true;
    render_thread = std::thread(&Overlay::renderLoop, this);
    LOGI("Render thread started");
}

void Overlay::stop() {
    running = false;
    if (render_thread.joinable()) {
        render_thread.join();
    }
    destroyEGL();
}

void Overlay::renderLoop() {
    LOGI("Render loop started");
    
    while (running) {
        render();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));  // ~60 FPS
    }
    
    LOGI("Render loop ended");
}

void Overlay::render() {
    if (egl_display == EGL_NO_DISPLAY || egl_surface == EGL_NO_SURFACE) return;
    
    // ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame();
    ImGui::NewFrame();
    
    // Render menu if visible
    ModMenu::getInstance().render();
    
    // Render
    ImGui::Render();
    
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    eglSwapBuffers(egl_display, egl_surface);
}

bool Overlay::initEGL() {
    egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (egl_display == EGL_NO_DISPLAY) {
        LOGE("No EGL display");
        return false;
    }
    
    EGLint major, minor;
    if (!eglInitialize(egl_display, &major, &minor)) {
        LOGE("EGL init failed");
        return false;
    }
    
    EGLint config_attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_NONE
    };
    
    EGLConfig config;
    EGLint num_configs;
    if (!eglChooseConfig(egl_display, config_attribs, &config, 1, &num_configs)) {
        LOGE("EGL config failed");
        return false;
    }
    
    EGLint ctx_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    egl_context = eglCreateContext(egl_display, config, EGL_NO_CONTEXT, ctx_attribs);
    if (egl_context == EGL_NO_CONTEXT) {
        LOGE("EGL context failed");
        return false;
    }
    
    egl_surface = eglCreateWindowSurface(egl_display, config, window, nullptr);
    if (egl_surface == EGL_NO_SURFACE) {
        LOGE("EGL surface failed");
        return false;
    }
    
    if (!eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context)) {
        LOGE("EGL make current failed");
        return false;
    }
    
    LOGI("EGL initialized (v%d.%d)", major, minor);
    return true;
}

void Overlay::destroyEGL() {
    if (egl_display != EGL_NO_DISPLAY) {
        eglMakeCurrent(egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (egl_surface != EGL_NO_SURFACE) eglDestroySurface(egl_display, egl_surface);
        if (egl_context != EGL_NO_CONTEXT) eglDestroyContext(egl_display, egl_context);
        eglTerminate(egl_display);
    }
    egl_display = EGL_NO_DISPLAY;
    egl_surface = EGL_NO_SURFACE;
    egl_context = EGL_NO_CONTEXT;
}

bool Overlay::handleTouch(int action, float x, float y, int pointer_count) {
    // Detect 3-finger tap to toggle menu
    // ACTION_DOWN = 0, ACTION_UP = 1, ACTION_MOVE = 2, ACTION_POINTER_UP = 6
    
    if (action == 0) {  // ACTION_DOWN
        touch_state.touch_count = pointer_count;
        if (pointer_count >= 3) {
            touch_state.multi_touch_detected = true;
        }
    }
    else if (action == 1 || action == 6) {  // ACTION_UP or ACTION_POINTER_UP
        if (touch_state.multi_touch_detected && touch_state.touch_count >= 3) {
            ModMenu::getInstance().toggle();
            touch_state.multi_touch_detected = false;
            return true;
        }
    }
    else if (action == 2) {  // ACTION_MOVE
        touch_state.touch_count = pointer_count;
    }
    
    // Pass to ImGui if menu is visible
    if (ModMenu::getInstance().isVisible()) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddMousePosEvent(x, y);
        if (action == 0)       // ACTION_DOWN
            io.AddMouseButtonEvent(0, true);
        else if (action == 1)  // ACTION_UP
            io.AddMouseButtonEvent(0, false);
        return true;
    }
    
    return false;
}

Overlay::~Overlay() {
    stop();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplAndroid_Shutdown();
    ImGui::DestroyContext();
}
