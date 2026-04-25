#pragma once

#include <android/native_window.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <thread>
#include <atomic>

class Overlay {
public:
    static Overlay& getInstance() {
        static Overlay instance;
        return instance;
    }
    
    bool init(ANativeWindow* window);
    void start();
    void stop();
    void render();
    
    // Touch handling for menu toggle (3-finger tap)
    bool handleTouch(int action, float x, float y, int pointer_count);
    
private:
    Overlay() = default;
    ~Overlay();
    Overlay(const Overlay&) = delete;
    Overlay& operator=(const Overlay&) = delete;
    
    // EGL
    EGLDisplay egl_display = EGL_NO_DISPLAY;
    EGLContext egl_context = EGL_NO_CONTEXT;
    EGLSurface egl_surface = EGL_NO_SURFACE;
    ANativeWindow* window = nullptr;
    
    // Thread
    std::atomic<bool> running{false};
    std::thread render_thread;
    
    // Touch state
    struct TouchState {
        bool multi_touch_detected = false;
        int touch_count = 0;
    };
    TouchState touch_state;
    
    bool initEGL();
    void destroyEGL();
    void renderLoop();
};
