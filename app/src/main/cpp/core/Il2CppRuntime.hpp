#pragma once

// =============================================================================
// core/Il2CppRuntime.hpp - IL2CPP lifecycle management (singleton)
//
// Responsibilities:
//   - Wait for libil2cpp.so to be loaded
//   - Attach the current thread to the IL2CPP domain
//   - Wait for assemblies to be available
//   - Report readiness state
// =============================================================================

#include <atomic>
#include <cstdint>
#include <cstddef>

namespace renderer::core {

class Il2CppRuntime {
public:
    static Il2CppRuntime& Get();

    // Full initialization: waits for library, attaches, waits for assemblies.
    bool Initialize();

    // Individual lifecycle steps (called by Initialize, exposed for flexibility)
    bool WaitForLibrary(int timeoutMs = 30000);
    bool ResolveExports();
    bool Attach();
    bool WaitForAssemblies(int timeoutMs = 30000);

    // State query
    bool IsReady() const { return ready_.load(std::memory_order_acquire); }
    uintptr_t GetBaseAddress() const { return baseAddress_; }

    Il2CppRuntime(const Il2CppRuntime&) = delete;
    Il2CppRuntime& operator=(const Il2CppRuntime&) = delete;

private:
    Il2CppRuntime() = default;

    std::atomic<bool> ready_{false};
    uintptr_t baseAddress_ = 0;
    void* libHandle_ = nullptr;

    // IL2CPP function pointers
    void* (*fn_il2cpp_domain_get_)() = nullptr;
    void* (*fn_il2cpp_thread_attach_)(void* domain) = nullptr;
    void** (*fn_il2cpp_domain_get_assemblies_)(void* domain, size_t* size) = nullptr;
};

} // namespace renderer::core
