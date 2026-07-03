// =============================================================================
// entry/Entry.cpp - JNI entry point and initialization orchestration
//
// Educational project: demonstrates Unity IL2CPP runtime interaction from
// native C++ on Android, including UI creation and touch input handling.
// =============================================================================

#include <jni.h>
#include <dlfcn.h>
#include <unistd.h>
#include <pthread.h>
#include <atomic>

#include <dobby.h>
#include <xdl.h>

#include "../core/Il2CppRuntime.hpp"
#include "../core/Il2CppResolver.hpp"
#include "../core/UnityEngine.hpp"
#include "../ui/Menu.hpp"
#include "../input/TouchInput.hpp"
#include "../util/Log.hpp"
#include "../Il2cpp/Il2Cpp.h"
#include "../Il2cpp/Obfuscate.h"

using namespace renderer::core;
using namespace renderer::ui;
using namespace renderer::input;

// =============================================================================
// Module state
// =============================================================================
static void* s_unityLibHandle = nullptr;
static std::atomic<bool> s_menuCreated{false};
static std::atomic<bool> s_fontAssigned{false};

// =============================================================================
// Hook state
// =============================================================================
static void (*s_orig_SendWillRenderCanvases)(void* method) = nullptr;

// =============================================================================
// Hook: Canvas::SendWillRenderCanvases
// - First frame after menu created: create font, assign to all widgets
// - Every frame: poll touch input and update menu
// =============================================================================
static void hook_SendWillRenderCanvases(void* method) {
    if (s_menuCreated.load(std::memory_order_acquire)) {
        // Assign font once on first render frame
        if (!s_fontAssigned.load(std::memory_order_acquire)) {
            if (UnityEngine::Font_CreateDynamicFontFromOSFont && UnityEngine::il2cpp_string_new) {
                auto* fontName = UnityEngine::il2cpp_string_new("Arial");
                void* font = UnityEngine::Font_CreateDynamicFontFromOSFont(fontName, 18, nullptr);
                if (font) {
                    Menu::Get().AssignFonts(font);
                    LOGI("Entry: Font assigned to all widgets");
                }
                s_fontAssigned.store(true, std::memory_order_release);
            }
        }

        // Poll touch and update menu every frame
        TouchInput::Poll();

        int count = TouchInput::GetTouchCount();
        if (count > 0) {
            TouchData touch = TouchInput::GetTouch(0);
            Vec2 canvasPos = TouchInput::ScreenToCanvas(touch.position, 1080.0f, 1920.0f);
            Menu::Get().Update(canvasPos, touch.phase, true);
        } else {
            // No touch - pass released state
            Menu::Get().Update({0, 0}, static_cast<int>(TouchPhase::Ended), false);
        }
    }

    // Call original
    if (s_orig_SendWillRenderCanvases) {
        s_orig_SendWillRenderCanvases(method);
    }
}

// =============================================================================
// Background init thread
// =============================================================================
static void* initThread(void* /*arg*/) {
    LOGI("Entry: Init thread started");

    // Step 1: Wait for libil2cpp.so to be loaded into memory
    if (!Il2CppRuntime::Get().WaitForLibrary()) {
        LOGE("Entry: Timeout waiting for libil2cpp.so");
        return nullptr;
    }

    // Step 2: Wait a moment for IL2CPP to initialize its exports
    usleep(1000000); // 1 second

    // Step 3: Resolve IL2CPP exports (il2cpp_domain_get, etc.)
    // This uses the existing Il2Cpp::Attach which resolves function pointers
    if (Il2Cpp::Attach("libil2cpp.so") != 0) {
        // Retry a few times - IL2CPP may not have initialized exports yet
        int retries = 0;
        while (Il2Cpp::Attach("libil2cpp.so") != 0 && retries < 20) {
            usleep(500000);
            retries++;
        }
        if (retries >= 20) {
            LOGE("Entry: Il2Cpp::Attach failed after retries");
            return nullptr;
        }
    }
    LOGI("Entry: Il2Cpp exports resolved");

    // Step 3.5: Resolve Il2CppRuntime's internal exports (for WaitForAssemblies and Attach)
    Il2CppRuntime::Get().ResolveExports();

    // Step 4: Wait for assemblies to be loaded
    if (!Il2CppRuntime::Get().WaitForAssemblies()) {
        LOGE("Entry: Timeout waiting for assemblies");
        return nullptr;
    }

    // Step 5: NOW attach this thread to the IL2CPP domain
    // (domain is valid only after assemblies are loaded)
    if (!Il2CppRuntime::Get().Attach()) {
        LOGE("Entry: Failed to attach thread to IL2CPP domain");
        return nullptr;
    }

    // Step 5: Wait for Unity scene to be ready
    LOGI("Entry: Waiting 3s for scene loading...");
    usleep(3000000);

    // Step 6: Initialize UnityEngine (resolve all function pointers)
    if (!UnityEngine::Init()) {
        LOGE("Entry: UnityEngine::Init() failed");
        return nullptr;
    }
    LOGI("Entry: UnityEngine initialized");

    // Step 7: Build the menu (hook-free, from this attached thread)
    {
        auto& menu = Menu::Get();

        // Add demo widgets before building
        menu.AddCategory("-- Display --");
        menu.AddToggle("Overlay [ON]", "Overlay [OFF]", false, 1);
        menu.AddToggle("Dark Mode [ON]", "Dark Mode [OFF]", false, 2);
        menu.AddSeekBar("Opacity", 0, 100, 75, 3);
        menu.AddCategory("-- Settings --");
        menu.AddCheckbox("Notifications", false, 4);
        menu.AddCheckbox("Auto-Save", false, 5);
        menu.AddToggle("Debug [ON]", "Debug [OFF]", false, 6);

        // Now build (creates all GameObjects)
        menu.Build();
    }
    s_menuCreated.store(true, std::memory_order_release);
    LOGI("Entry: Menu built successfully");

    // Step 8: Install minimal Dobby hook on Canvas::SendWillRenderCanvases
    void* hookTarget = Il2Cpp::GetMethodOffset(
        YK("UnityEngine.UIModule.dll"),
        YK("UnityEngine"),
        YK("Canvas"),
        YK("SendWillRenderCanvases"), 0);

    if (hookTarget) {
        int ret = DobbyHook(hookTarget,
                            reinterpret_cast<void*>(hook_SendWillRenderCanvases),
                            reinterpret_cast<void**>(&s_orig_SendWillRenderCanvases));
        if (ret == 0) {
            LOGI("Entry: Hook installed on SendWillRenderCanvases");
        } else {
            LOGE("Entry: DobbyHook failed (ret=%d)", ret);
        }
    } else {
        LOGE("Entry: Could not find SendWillRenderCanvases method");
    }

    LOGI("Entry: Initialization complete");
    return nullptr;
}

// =============================================================================
// JNI native methods
// =============================================================================
extern "C" {

typedef jint (*JNI_OnLoad_t)(JavaVM* vm, void* reserved);

static jboolean JNICALL native_load(JNIEnv* env, jobject /*thiz*/, jstring path) {
    const char* libraryPath = env->GetStringUTFChars(path, nullptr);
    if (!libraryPath) return JNI_FALSE;

    char fullPath[1024];
    snprintf(fullPath, sizeof(fullPath), "%s/libunity.so", libraryPath);
    env->ReleaseStringUTFChars(path, libraryPath);

    // dlopen libunity.so
    s_unityLibHandle = dlopen(fullPath, RTLD_LAZY | RTLD_LOCAL);
    if (!s_unityLibHandle) {
        LOGE("Entry: Failed to load libunity.so: %s", dlerror());
        return JNI_FALSE;
    }

    // Call libunity's JNI_OnLoad
    auto unityJniOnLoad = reinterpret_cast<JNI_OnLoad_t>(
        dlsym(s_unityLibHandle, "JNI_OnLoad"));
    if (!unityJniOnLoad) {
        LOGE("Entry: libunity.so has no JNI_OnLoad");
        dlclose(s_unityLibHandle);
        s_unityLibHandle = nullptr;
        return JNI_FALSE;
    }

    JavaVM* vm = nullptr;
    if (env->GetJavaVM(&vm) != JNI_OK) {
        dlclose(s_unityLibHandle);
        s_unityLibHandle = nullptr;
        return JNI_FALSE;
    }

    jint result = unityJniOnLoad(vm, nullptr);
    if (result < JNI_VERSION_1_6) {
        LOGE("Entry: libunity.so JNI_OnLoad returned invalid version");
        dlclose(s_unityLibHandle);
        s_unityLibHandle = nullptr;
        return JNI_FALSE;
    }

    LOGI("Entry: libunity.so loaded successfully");

    // Start initialization thread
    pthread_t thread;
    pthread_create(&thread, nullptr, initThread, nullptr);
    pthread_detach(thread);

    return JNI_TRUE;
}

static jboolean JNICALL native_unload(JNIEnv* /*env*/, jclass /*clazz*/) {
    if (s_menuCreated.load(std::memory_order_acquire)) {
        // Menu::Get().Destroy() would hide/deactivate all GOs
        s_menuCreated.store(false, std::memory_order_release);
        s_fontAssigned.store(false, std::memory_order_release);
        LOGI("Entry: Menu destroyed");
    }
    if (s_unityLibHandle) {
        dlclose(s_unityLibHandle);
        s_unityLibHandle = nullptr;
    }
    return JNI_TRUE;
}

// =============================================================================
// JNI_OnLoad - register native methods for com.unity3d.player.NativeLoader
// =============================================================================
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* /*reserved*/) {
    JNIEnv* env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }

    jclass clazz = env->FindClass("com/unity3d/player/NativeLoader");
    if (!clazz) {
        LOGE("Entry: Failed to find NativeLoader class");
        return JNI_ERR;
    }

    static const JNINativeMethod methods[] = {
        {"load",   "(Ljava/lang/String;)Z", reinterpret_cast<void*>(native_load)},
        {"unload", "()Z",                   reinterpret_cast<void*>(native_unload)}
    };

    jint ret = env->RegisterNatives(clazz, methods,
                                    static_cast<jint>(sizeof(methods) / sizeof(methods[0])));
    if (ret != JNI_OK) {
        LOGE("Entry: RegisterNatives failed");
        return JNI_ERR;
    }

    LOGI("Entry: JNI_OnLoad - natives registered");
    return JNI_VERSION_1_6;
}

} // extern "C"
