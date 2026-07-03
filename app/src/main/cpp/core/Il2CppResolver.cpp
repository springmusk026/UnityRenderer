// =============================================================================
// core/Il2CppResolver.cpp - Class/method resolution implementation
// =============================================================================

#include "Il2CppResolver.hpp"
#include "../util/Log.hpp"
#include "../Il2cpp/Il2Cpp.h"
#include "../Il2cpp/Obfuscate.h"

#include <xdl.h>

namespace renderer::core {

Il2CppResolver& Il2CppResolver::Get() {
    static Il2CppResolver instance;
    return instance;
}

int Il2CppResolver::Initialize() {
    if (initialized_) return 0;

    // Use existing Il2Cpp::Attach to resolve all core il2cpp exports
    int result = Il2Cpp::Attach(YK("libil2cpp.so"));
    if (result != 0) {
        LOGE("Il2CppResolver: Il2Cpp::Attach failed");
        return -1;
    }

    // Resolve additional exports we need directly
    void* handle = xdl_open(YK("libil2cpp.so"), 0);
    if (!handle) {
        LOGE("Il2CppResolver: xdl_open failed");
        return -1;
    }

    fn_runtime_invoke_ = reinterpret_cast<decltype(fn_runtime_invoke_)>(
        xdl_sym(handle, YK("il2cpp_runtime_invoke"), nullptr));
    fn_string_new_ = reinterpret_cast<decltype(fn_string_new_)>(
        xdl_sym(handle, YK("il2cpp_string_new"), nullptr));
    fn_class_get_type_ = reinterpret_cast<decltype(fn_class_get_type_)>(
        xdl_sym(handle, YK("il2cpp_class_get_type"), nullptr));
    fn_type_get_object_ = reinterpret_cast<decltype(fn_type_get_object_)>(
        xdl_sym(handle, YK("il2cpp_type_get_object"), nullptr));
    fn_class_get_method_from_name_ = reinterpret_cast<decltype(fn_class_get_method_from_name_)>(
        xdl_sym(handle, YK("il2cpp_class_get_method_from_name"), nullptr));

    if (!fn_runtime_invoke_ || !fn_string_new_ ||
        !fn_class_get_type_ || !fn_type_get_object_ ||
        !fn_class_get_method_from_name_) {
        LOGE("Il2CppResolver: Failed to resolve one or more exports");
        return -1;
    }

    initialized_ = true;
    LOGI("Il2CppResolver: Initialized successfully");
    return 0;
}

std::string Il2CppResolver::MakeKey(const char* a, const char* b, const char* c,
                                     const char* d, int argc) {
    std::string key;
    key.reserve(128);
    key += a; key += '|';
    key += b; key += '|';
    key += c;
    if (d) { key += '|'; key += d; }
    if (argc >= 0) { key += '|'; key += std::to_string(argc); }
    return key;
}

void* Il2CppResolver::ResolveClass(const char* image, const char* ns, const char* cls) {
    auto key = MakeKey(image, ns, cls);
    auto it = classCache_.find(key);
    if (it != classCache_.end()) return it->second;

    void* klass = Il2Cpp::GetClass(image, ns, cls);
    if (klass) {
        classCache_[key] = klass;
    } else {
        LOGW("Il2CppResolver: Failed to resolve class %s::%s in %s", ns, cls, image);
    }
    return klass;
}

void* Il2CppResolver::ResolveMethod(const char* image, const char* ns, const char* cls,
                                     const char* method, int argc) {
    auto key = MakeKey(image, ns, cls, method, argc);
    auto it = methodCache_.find(key);
    if (it != methodCache_.end()) return it->second;

    void* ptr = Il2Cpp::GetMethodOffset(image, ns, cls, method, argc);
    if (ptr) {
        methodCache_[key] = ptr;
    } else {
        LOGW("Il2CppResolver: Failed to resolve %s::%s::%s(%d)", ns, cls, method, argc);
    }
    return ptr;
}

void* Il2CppResolver::GetMethodInfo(void* klass, const char* name, int argc) {
    if (!klass || !fn_class_get_method_from_name_) return nullptr;
    return fn_class_get_method_from_name_(klass, name, argc);
}

void* Il2CppResolver::RuntimeInvoke(void* methodInfo, void* obj, void** args) {
    if (!fn_runtime_invoke_ || !methodInfo) return nullptr;
    void* exc = nullptr;
    void* result = fn_runtime_invoke_(methodInfo, obj, args, &exc);
    if (exc) LOGE("Il2CppResolver: RuntimeInvoke threw an exception");
    return result;
}

void* Il2CppResolver::NewObject(const char* image, const char* ns, const char* cls) {
    return Il2Cpp::NewClassObject(image, ns, cls);
}

Il2CppStringx* Il2CppResolver::NewString(const char* str) {
    if (!fn_string_new_ || !str) return nullptr;
    return fn_string_new_(str);
}

void* Il2CppResolver::GetType(void* klass) {
    if (!klass || !fn_class_get_type_ || !fn_type_get_object_) return nullptr;
    void* type = fn_class_get_type_(klass);
    if (!type) return nullptr;
    return fn_type_get_object_(type);
}

} // namespace renderer::core
