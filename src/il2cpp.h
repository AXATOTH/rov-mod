#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <functional>
#include <android/log.h>

#define LOG_TAG "ROV_Minimap"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

// IL2CPP Types
typedef void Il2CppObject;
typedef void Il2CppDomain;
typedef void Il2CppAssembly;
typedef void Il2CppImage;
typedef void Il2CppClass;
typedef void MethodInfo;
typedef void Il2CppArray;
typedef void Il2CppString;
typedef void Il2CppThread;
typedef void FieldInfo;

// Unity Types
struct Vector3 {
    float x, y, z;
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
};

struct Vector2 {
    float x, y;
    Vector2() : x(0), y(0) {}
    Vector2(float x, float y) : x(x), y(y) {}
};

namespace il2cpp {
    // Initialization
    bool init();
    bool isInitialized();
    
    // Domain
    Il2CppDomain* domain_get();
    Il2CppThread* thread_attach(Il2CppDomain* domain);
    
    // Class operations
    Il2CppClass* get_class(const char* assembly_name, const char* namespaze, const char* class_name);
    MethodInfo* get_method(Il2CppClass* klass, const char* method_name, int param_count = -1);
    
    // Field operations
    void* get_field_ptr(Il2CppObject* obj, Il2CppClass* klass, const char* field_name);
    
    template<typename T>
    T get_field(Il2CppObject* obj, Il2CppClass* klass, const char* field_name) {
        void* ptr = get_field_ptr(obj, klass, field_name);
        if (!ptr) return T{};
        return *static_cast<T*>(ptr);
    }
    
    template<typename T>
    void set_field(Il2CppObject* obj, Il2CppClass* klass, const char* field_name, T value) {
        void* ptr = get_field_ptr(obj, klass, field_name);
        if (ptr) *static_cast<T*>(ptr) = value;
    }
    
    // Static field operations
    bool get_static_field(Il2CppClass* klass, const char* field_name, void* out_value);
    bool set_static_field(Il2CppClass* klass, const char* field_name, void* value);
    
    // String operations
    Il2CppString* string_new(const char* str);
    std::string string_to_utf8(Il2CppString* str);
    
    // Array operations
    Il2CppArray* array_new(Il2CppClass* element_class, size_t length);
    size_t array_length(Il2CppArray* array);
    void* array_addr(Il2CppArray* array, size_t element_size, size_t index);
    
    // Method invocation - generic
    typedef void* (*InvokeFunc)(Il2CppObject*, ...);
    
    // Game-specific helpers
    Il2CppObject* get_local_player();
    Il2CppObject* get_game_manager();
    Il2CppObject* get_minimap_controller();
    
    // Hooking
    bool hook_method(const char* assembly_name, const char* namespaze,
                     const char* class_name, const char* method_name,
                     void* hook_func, void** original_func);
}
