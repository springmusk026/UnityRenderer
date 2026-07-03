#pragma once

// =============================================================================
// core/Il2CppResolver.hpp - Class/method resolution with caching (singleton)
//
// Wraps the existing Il2Cpp:: namespace functions with a clean OOP interface
// and adds caching via std::unordered_map.
// =============================================================================

#include "../Il2cpp/Il2CppType.h"
#include <string>
#include <unordered_map>

namespace renderer::core {

class Il2CppResolver {
public:
    static Il2CppResolver& Get();

    // Initialize resolver (calls Il2Cpp::Attach internally)
    int Initialize();

    // Resolve Il2CppClass* by image/namespace/class
    void* ResolveClass(const char* image, const char* ns, const char* cls);

    // Resolve native method pointer
    void* ResolveMethod(const char* image, const char* ns, const char* cls,
                        const char* method, int argc = 0);

    // Get MethodInfo* for use with il2cpp_runtime_invoke
    void* GetMethodInfo(void* klass, const char* name, int argc = 0);

    // Call il2cpp_runtime_invoke
    void* RuntimeInvoke(void* methodInfo, void* obj, void** args);

    // Create new managed object
    void* NewObject(const char* image, const char* ns, const char* cls);

    // Create IL2CPP string
    Il2CppStringx* NewString(const char* str);

    // Get System.Type from Il2CppClass* for AddComponent<T>()
    void* GetType(void* klass);

    Il2CppResolver(const Il2CppResolver&) = delete;
    Il2CppResolver& operator=(const Il2CppResolver&) = delete;

private:
    Il2CppResolver() = default;

    bool initialized_ = false;

    std::unordered_map<std::string, void*> classCache_;
    std::unordered_map<std::string, void*> methodCache_;

    // IL2CPP function pointers
    void* (*fn_runtime_invoke_)(void*, void*, void**, void**) = nullptr;
    Il2CppStringx* (*fn_string_new_)(const char*) = nullptr;
    void* (*fn_class_get_type_)(void*) = nullptr;
    void* (*fn_type_get_object_)(void*) = nullptr;
    void* (*fn_class_get_method_from_name_)(void*, const char*, int) = nullptr;

    static std::string MakeKey(const char* a, const char* b, const char* c,
                               const char* d = nullptr, int argc = -1);
};

} // namespace renderer::core
