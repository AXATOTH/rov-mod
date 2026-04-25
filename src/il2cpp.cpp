#include "il2cpp.h"
#include <dlfcn.h>
#include <string>
#include <map>
#include <cstring>

namespace il2cpp {
    // Function pointers
    static void* il2cpp_handle = nullptr;
    static Il2CppDomain* (*il2cpp_domain_get_ptr)() = nullptr;
    static Il2CppAssembly** (*il2cpp_domain_get_assemblies_ptr)(Il2CppDomain*, size_t*) = nullptr;
    static Il2CppImage* (*il2cpp_assembly_get_image_ptr)(Il2CppAssembly*) = nullptr;
    static Il2CppClass* (*il2cpp_class_from_name_ptr)(Il2CppImage*, const char*, const char*) = nullptr;
    static MethodInfo* (*il2cpp_class_get_method_from_name_ptr)(Il2CppClass*, const char*, int) = nullptr;
    static FieldInfo* (*il2cpp_class_get_field_from_name_ptr)(Il2CppClass*, const char*) = nullptr;
    static size_t (*il2cpp_field_get_offset_ptr)(FieldInfo*) = nullptr;
    static void (*il2cpp_field_static_get_value_ptr)(FieldInfo*, void*) = nullptr;
    static void (*il2cpp_field_static_set_value_ptr)(FieldInfo*, void*) = nullptr;
    static Il2CppString* (*il2cpp_string_new_ptr)(const char*) = nullptr;
    static char* (*il2cpp_string_to_utf8_ptr)(Il2CppString*) = nullptr;
    static Il2CppArray* (*il2cpp_array_new_ptr)(Il2CppClass*, size_t) = nullptr;
    static Il2CppObject* (*il2cpp_object_new_ptr)(Il2CppClass*) = nullptr;
    static Il2CppThread* (*il2cpp_thread_attach_ptr)(Il2CppDomain*) = nullptr;
    static const char* (*il2cpp_image_get_name_ptr)(Il2CppImage*) = nullptr;
    static Il2CppClass* (*il2cpp_class_get_interfaces_ptr)(Il2CppClass*, void**) = nullptr;
    
    // Cached data
    static std::map<std::string, Il2CppImage*> assembly_images;
    static std::map<std::string, Il2CppClass*> class_cache;
    static bool initialized = false;
    
    // Helper to resolve symbol
    template<typename T>
    T resolve(const char* name) {
        T ptr = reinterpret_cast<T>(dlsym(il2cpp_handle, name));
        if (!ptr) {
            LOGE("Failed to resolve: %s", name);
        }
        return ptr;
    }
    
    bool init() {
        if (initialized) return true;
        
        LOGI("=== Initializing IL2CPP ===");
        
        // Try multiple library names
        const char* lib_names[] = {
            "libil2cpp.so",
            "libil2cpp.aot.so",
            nullptr
        };
        
        for (int i = 0; lib_names[i]; i++) {
            il2cpp_handle = dlopen(lib_names[i], RTLD_LAZY | RTLD_GLOBAL);
            if (il2cpp_handle) {
                LOGI("Loaded: %s", lib_names[i]);
                break;
            }
        }
        
        if (!il2cpp_handle) {
            // Try to find it in loaded libraries
            FILE* maps = fopen("/proc/self/maps", "r");
            if (maps) {
                char line[512];
                while (fgets(line, sizeof(line), maps)) {
                    if (strstr(line, "libil2cpp.so")) {
                        // Extract path
                        char* path = strchr(line, '/');
                        if (path) {
                            path[strcspn(path, "\n")] = 0;
                            LOGI("Found in maps: %s", path);
                            il2cpp_handle = dlopen(path, RTLD_LAZY | RTLD_GLOBAL);
                            if (il2cpp_handle) {
                                LOGI("Loaded from maps!");
                                break;
                            }
                        }
                    }
                }
                fclose(maps);
            }
        }
        
        if (!il2cpp_handle) {
            LOGE("Failed to load libil2cpp.so: %s", dlerror());
            return false;
        }
        
        // Resolve all needed functions
        il2cpp_domain_get_ptr = resolve<Il2CppDomain*(*)()>("il2cpp_domain_get");
        il2cpp_domain_get_assemblies_ptr = resolve<Il2CppAssembly**(*)(Il2CppDomain*, size_t*)>("il2cpp_domain_get_assemblies");
        il2cpp_assembly_get_image_ptr = resolve<Il2CppImage*(*)(Il2CppAssembly*)>("il2cpp_assembly_get_image");
        il2cpp_class_from_name_ptr = resolve<Il2CppClass*(*)(Il2CppImage*, const char*, const char*)>("il2cpp_class_from_name");
        il2cpp_class_get_method_from_name_ptr = resolve<MethodInfo*(*)(Il2CppClass*, const char*, int)>("il2cpp_class_get_method_from_name");
        il2cpp_class_get_field_from_name_ptr = resolve<FieldInfo*(*)(Il2CppClass*, const char*)>("il2cpp_class_get_field_from_name");
        il2cpp_field_get_offset_ptr = resolve<size_t(*)(FieldInfo*)>("il2cpp_field_get_offset");
        il2cpp_string_new_ptr = resolve<Il2CppString*(*)(const char*)>("il2cpp_string_new");
        il2cpp_string_to_utf8_ptr = resolve<char*(*)(Il2CppString*)>("il2cpp_string_to_utf8");
        il2cpp_array_new_ptr = resolve<Il2CppArray*(*)(Il2CppClass*, size_t)>("il2cpp_array_new");
        il2cpp_object_new_ptr = resolve<Il2CppObject*(*)(Il2CppClass*)>("il2cpp_object_new");
        il2cpp_thread_attach_ptr = resolve<Il2CppThread*(*)(Il2CppDomain*)>("il2cpp_thread_attach");
        il2cpp_image_get_name_ptr = resolve<const char*(*)(Il2CppImage*)>("il2cpp_image_get_name");
        
        // Optional
        il2cpp_field_static_get_value_ptr = (decltype(il2cpp_field_static_get_value_ptr))dlsym(il2cpp_handle, "il2cpp_field_static_get_value");
        il2cpp_field_static_set_value_ptr = (decltype(il2cpp_field_static_set_value_ptr))dlsym(il2cpp_handle, "il2cpp_field_static_set_value");
        
        if (!il2cpp_domain_get_ptr) {
            LOGE("Critical function missing!");
            return false;
        }
        
        // Cache assemblies
        Il2CppDomain* domain = il2cpp_domain_get_ptr();
        if (domain) {
            if (il2cpp_thread_attach_ptr) {
                il2cpp_thread_attach_ptr(domain);
            }
            
            size_t count = 0;
            Il2CppAssembly** assemblies = il2cpp_domain_get_assemblies_ptr(domain, &count);
            
            LOGI("Found %zu assemblies", count);
            
            for (size_t i = 0; i < count; i++) {
                Il2CppImage* image = il2cpp_assembly_get_image_ptr(assemblies[i]);
                if (image && il2cpp_image_get_name_ptr) {
                    const char* name = il2cpp_image_get_name_ptr(image);
                    if (name) {
                        assembly_images[name] = image;
                        LOGD("  Assembly: %s", name);
                    }
                }
            }
        }
        
        initialized = true;
        LOGI("=== IL2CPP initialized with %zu assemblies ===", assembly_images.size());
        return true;
    }
    
    bool isInitialized() {
        return initialized;
    }
    
    Il2CppDomain* domain_get() {
        return il2cpp_domain_get_ptr ? il2cpp_domain_get_ptr() : nullptr;
    }
    
    Il2CppThread* thread_attach(Il2CppDomain* domain) {
        return il2cpp_thread_attach_ptr ? il2cpp_thread_attach_ptr(domain) : nullptr;
    }
    
    Il2CppClass* get_class(const char* assembly_name, const char* namespaze, const char* class_name) {
        if (!initialized || !il2cpp_class_from_name_ptr) return nullptr;
        
        // Check cache first
        std::string key = std::string(assembly_name) + "::" + namespaze + "::" + class_name;
        auto cached = class_cache.find(key);
        if (cached != class_cache.end()) {
            return cached->second;
        }
        
        // Search in cached assemblies
        for (auto& [name, image] : assembly_images) {
            if (name.find(assembly_name) != std::string::npos) {
                Il2CppClass* klass = il2cpp_class_from_name_ptr(image, namespaze, class_name);
                if (klass) {
                    class_cache[key] = klass;
                    return klass;
                }
            }
        }
        
        // Try exact match
        auto it = assembly_images.find(assembly_name);
        if (it != assembly_images.end()) {
            Il2CppClass* klass = il2cpp_class_from_name_ptr(it->second, namespaze, class_name);
            if (klass) {
                class_cache[key] = klass;
                return klass;
            }
        }
        
        return nullptr;
    }
    
    MethodInfo* get_method(Il2CppClass* klass, const char* method_name, int param_count) {
        if (!klass || !il2cpp_class_get_method_from_name_ptr) return nullptr;
        return il2cpp_class_get_method_from_name_ptr(klass, method_name, param_count);
    }
    
    void* get_field_ptr(Il2CppObject* obj, Il2CppClass* klass, const char* field_name) {
        if (!klass || !il2cpp_class_get_field_from_name_ptr || !il2cpp_field_get_offset_ptr) {
            return nullptr;
        }
        
        FieldInfo* field = il2cpp_class_get_field_from_name_ptr(klass, field_name);
        if (!field) return nullptr;
        
        size_t offset = il2cpp_field_get_offset_ptr(field);
        return reinterpret_cast<char*>(obj) + offset;
    }
    
    bool get_static_field(Il2CppClass* klass, const char* field_name, void* out_value) {
        if (!klass || !il2cpp_class_get_field_from_name_ptr || !il2cpp_field_static_get_value_ptr) {
            return false;
        }
        
        FieldInfo* field = il2cpp_class_get_field_from_name_ptr(klass, field_name);
        if (!field) return false;
        
        il2cpp_field_static_get_value_ptr(field, out_value);
        return true;
    }
    
    bool set_static_field(Il2CppClass* klass, const char* field_name, void* value) {
        if (!klass || !il2cpp_class_get_field_from_name_ptr || !il2cpp_field_static_set_value_ptr) {
            return false;
        }
        
        FieldInfo* field = il2cpp_class_get_field_from_name_ptr(klass, field_name);
        if (!field) return false;
        
        il2cpp_field_static_set_value_ptr(field, value);
        return true;
    }
    
    Il2CppString* string_new(const char* str) {
        return il2cpp_string_new_ptr ? il2cpp_string_new_ptr(str) : nullptr;
    }
    
    std::string string_to_utf8(Il2CppString* str) {
        if (!str || !il2cpp_string_to_utf8_ptr) return "";
        char* utf8 = il2cpp_string_to_utf8_ptr(str);
        if (!utf8) return "";
        std::string result(utf8);
        free(utf8);
        return result;
    }
    
    Il2CppArray* array_new(Il2CppClass* element_class, size_t length) {
        return il2cpp_array_new_ptr ? il2cpp_array_new_ptr(element_class, length) : nullptr;
    }
    
    size_t array_length(Il2CppArray* array) {
        if (!array) return 0;
        // IL2CPP array length is at offset 24 on arm64
        return *reinterpret_cast<size_t*>(reinterpret_cast<char*>(array) + 24);
    }
    
    void* array_addr(Il2CppArray* array, size_t element_size, size_t index) {
        if (!array) return nullptr;
        // IL2CPP array data starts at offset 32 on arm64
        return reinterpret_cast<char*>(array) + 32 + (index * element_size);
    }
    
    Il2CppObject* get_local_player() {
        // GameManager.Instance.LocalPlayer (common pattern in MOBA games)
        Il2CppClass* gm_class = get_class("Assembly-CSharp", "", "GameManager");
        if (!gm_class) {
            gm_class = get_class("Assembly-CSharp", "", "GameplayManager");
        }
        if (!gm_class) return nullptr;
        
        MethodInfo* get_inst = get_method(gm_class, "get_Instance", 0);
        if (!get_inst) get_inst = get_method(gm_class, "get_Instance$", 0);
        if (!get_inst) return nullptr;
        
        // Call static getter
        typedef Il2CppObject* (*StaticGetter)(MethodInfo*);
        auto instance = reinterpret_cast<StaticGetter>(get_inst)(nullptr);
        if (!instance) return nullptr;
        
        MethodInfo* get_local = get_method(gm_class, "get_LocalPlayer", 0);
        if (!get_local) get_local = get_method(gm_class, "GetLocalPlayer", 0);
        if (!get_local) return nullptr;
        
        typedef Il2CppObject* (*Getter)(Il2CppObject*, MethodInfo*);
        return reinterpret_cast<Getter>(get_local)(instance, nullptr);
    }
    
    Il2CppObject* get_game_manager() {
        Il2CppClass* gm_class = get_class("Assembly-CSharp", "", "GameManager");
        if (!gm_class) return nullptr;
        
        MethodInfo* get_inst = get_method(gm_class, "get_Instance", 0);
        if (!get_inst) return nullptr;
        
        typedef Il2CppObject* (*StaticGetter)(MethodInfo*);
        return reinterpret_cast<StaticGetter>(get_inst)(nullptr);
    }
    
    Il2CppObject* get_minimap_controller() {
        const char* class_names[] = {
            "MinimapController", "MinimapManager", "UIMinimap",
            "MiniMapView", "MapController", nullptr
        };
        
        for (int i = 0; class_names[i]; i++) {
            Il2CppClass* klass = get_class("Assembly-CSharp", "", class_names[i]);
            if (!klass) continue;
            
            MethodInfo* method = get_method(klass, "get_Instance", 0);
            if (!method) method = get_method(klass, "get_Singleton", 0);
            if (!method) method = get_method(klass, "Instance", 0);
            
            if (method) {
                typedef Il2CppObject* (*StaticGetter)(MethodInfo*);
                return reinterpret_cast<StaticGetter>(method)(nullptr);
            }
        }
        return nullptr;
    }
    
    bool hook_method(const char* assembly_name, const char* namespaze,
                     const char* class_name, const char* method_name,
                     void* hook_func, void** original_func) {
        // This is a placeholder - actual hooking uses Dobby
        // See hacks.cpp for Dobby-based hooking
        return false;
    }
}
