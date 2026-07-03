// =============================================================================
// core/Il2CppRuntime.cpp - IL2CPP lifecycle implementation
// =============================================================================

#include "Il2CppRuntime.hpp"
#include "../util/Log.hpp"
#include "../Il2cpp/Obfuscate.h"

#include <xdl.h>

#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <inttypes.h>

namespace renderer::core {

Il2CppRuntime& Il2CppRuntime::Get() {
    static Il2CppRuntime instance;
    return instance;
}

bool Il2CppRuntime::Initialize() {
    if (ready_.load(std::memory_order_acquire)) return true;

    LOGI("Il2CppRuntime: Starting initialization...");

    if (!WaitForLibrary()) {
        LOGE("Il2CppRuntime: Timed out waiting for libil2cpp.so");
        return false;
    }
    if (!ResolveExports()) {
        LOGE("Il2CppRuntime: Failed to resolve IL2CPP exports");
        return false;
    }
    if (!Attach()) {
        LOGE("Il2CppRuntime: Failed to attach thread");
        return false;
    }
    if (!WaitForAssemblies()) {
        LOGE("Il2CppRuntime: Timed out waiting for assemblies");
        return false;
    }

    ready_.store(true, std::memory_order_release);
    LOGI("Il2CppRuntime: Initialization complete (base=0x%" PRIXPTR ")", baseAddress_);
    return true;
}

bool Il2CppRuntime::WaitForLibrary(int timeoutMs) {
    const int sleepMs = 100;
    int elapsed = 0;

    while (elapsed < timeoutMs) {
        char line[512];
        FILE* f = fopen("/proc/self/maps", "r");
        if (f) {
            while (fgets(line, sizeof(line), f)) {
                if (strstr(line, "libil2cpp.so")) {
                    uintptr_t base = 0;
                    sscanf(line, "%" SCNxPTR, &base);
                    baseAddress_ = base;
                    fclose(f);
                    LOGI("Il2CppRuntime: Found libil2cpp.so at 0x%" PRIXPTR, base);
                    return true;
                }
            }
            fclose(f);
        }
        usleep(sleepMs * 1000);
        elapsed += sleepMs;
    }
    return false;
}

bool Il2CppRuntime::ResolveExports() {
    libHandle_ = xdl_open(YK("libil2cpp.so"), 0);
    if (!libHandle_) {
        LOGE("Il2CppRuntime: xdl_open failed");
        return false;
    }

    fn_il2cpp_domain_get_ = reinterpret_cast<void* (*)()>(
        xdl_sym(libHandle_, YK("il2cpp_domain_get"), nullptr));
    fn_il2cpp_thread_attach_ = reinterpret_cast<void* (*)(void*)>(
        xdl_sym(libHandle_, YK("il2cpp_thread_attach"), nullptr));
    fn_il2cpp_domain_get_assemblies_ = reinterpret_cast<void** (*)(void*, size_t*)>(
        xdl_sym(libHandle_, YK("il2cpp_domain_get_assemblies"), nullptr));

    if (!fn_il2cpp_domain_get_ || !fn_il2cpp_thread_attach_ ||
        !fn_il2cpp_domain_get_assemblies_) {
        LOGE("Il2CppRuntime: Failed to resolve one or more exports");
        return false;
    }
    LOGI("Il2CppRuntime: Exports resolved");
    return true;
}

bool Il2CppRuntime::Attach() {
    if (!fn_il2cpp_domain_get_ || !fn_il2cpp_thread_attach_) return false;

    void* domain = fn_il2cpp_domain_get_();
    if (!domain) {
        LOGE("Il2CppRuntime: il2cpp_domain_get returned null");
        return false;
    }
    void* thread = fn_il2cpp_thread_attach_(domain);
    if (!thread) {
        LOGE("Il2CppRuntime: il2cpp_thread_attach returned null");
        return false;
    }
    LOGI("Il2CppRuntime: Thread attached to domain %p", domain);
    return true;
}

bool Il2CppRuntime::WaitForAssemblies(int timeoutMs) {
    if (!fn_il2cpp_domain_get_ || !fn_il2cpp_domain_get_assemblies_) return false;

    const int sleepMs = 200;
    int elapsed = 0;

    while (elapsed < timeoutMs) {
        void* domain = fn_il2cpp_domain_get_();
        if (domain) {
            size_t size = 0;
            void** assemblies = fn_il2cpp_domain_get_assemblies_(domain, &size);
            if (assemblies && size > 0) {
                LOGI("Il2CppRuntime: %zu assemblies loaded", size);
                return true;
            }
        }
        usleep(sleepMs * 1000);
        elapsed += sleepMs;
    }
    return false;
}

} // namespace renderer::core
