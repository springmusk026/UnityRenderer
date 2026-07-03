// =============================================================================
// ui/Canvas.cpp - Canvas wrapper implementation
// =============================================================================

#include "Canvas.hpp"
#include "../core/Il2CppResolver.hpp"
#include "../util/Log.hpp"

namespace renderer::ui {

Canvas::~Canvas() {
    Destroy();
}

bool Canvas::Create(int sortingOrder, Vec2 referenceResolution) {
    if (m_gameObject) return true; // Already created

    auto& resolver = Il2CppResolver::Get();

    // Create GameObject
    auto* go = reinterpret_cast<GameObject*>(
        resolver.NewObject(IMG_CORE, "UnityEngine", "GameObject"));
    if (!go) {
        LOGE("Canvas::Create: Failed to create GameObject");
        return false;
    }

    // Call .ctor("MenuCanvas")
    auto* nameStr = UnityEngine::il2cpp_string_new("MenuCanvas");
    if (UnityEngine::il2cpp_runtime_invoke && UnityEngine::methodInfo_GameObject_ctor) {
        void* args[] = { nameStr };
        void* exc = nullptr;
        UnityEngine::il2cpp_runtime_invoke(UnityEngine::methodInfo_GameObject_ctor, go, args, &exc);
        if (exc) {
            LOGE("Canvas::Create: GameObject .ctor exception");
            return false;
        }
    } else if (UnityEngine::GameObject_ctor) {
        UnityEngine::GameObject_ctor(go, nameStr, nullptr);
    }

    // AddComponent<Canvas>
    void* canvasType = UnityEngine::GetType(UnityEngine::klass_Canvas);
    core::Canvas* canvas = nullptr;
    if (canvasType && UnityEngine::il2cpp_runtime_invoke && UnityEngine::methodInfo_AddComponent) {
        void* args[] = { canvasType };
        void* exc = nullptr;
        canvas = reinterpret_cast<core::Canvas*>(
            UnityEngine::il2cpp_runtime_invoke(UnityEngine::methodInfo_AddComponent, go, args, &exc));
    }
    if (canvas) {
        UnityEngine::Canvas_set_renderMode(canvas, (int)RenderMode::ScreenSpaceOverlay, nullptr);
        UnityEngine::Canvas_set_sortingOrder(canvas, sortingOrder, nullptr);
    }

    // AddComponent<CanvasScaler>
    void* scalerType = UnityEngine::GetType(UnityEngine::klass_CanvasScaler);
    CanvasScaler* scaler = nullptr;
    if (scalerType && UnityEngine::il2cpp_runtime_invoke && UnityEngine::methodInfo_AddComponent) {
        void* args[] = { scalerType };
        void* exc = nullptr;
        scaler = reinterpret_cast<CanvasScaler*>(
            UnityEngine::il2cpp_runtime_invoke(UnityEngine::methodInfo_AddComponent, go, args, &exc));
    }
    if (scaler) {
        UnityEngine::CanvasScaler_set_uiScaleMode(scaler, (int)CanvasScaleMode::ScaleWithScreenSize, nullptr);
        UnityEngine::CanvasScaler_set_screenMatchMode(scaler, (int)ScreenMatchMode::MatchWidthOrHeight, nullptr);
        UnityEngine::CanvasScaler_set_referenceResolution(scaler, referenceResolution, nullptr);
        UnityEngine::CanvasScaler_set_matchWidthOrHeight(scaler, 0.5f, nullptr);
    }

    // AddComponent<GraphicRaycaster>
    void* raycasterType = UnityEngine::GetType(UnityEngine::klass_GraphicRaycaster);
    if (raycasterType && UnityEngine::il2cpp_runtime_invoke && UnityEngine::methodInfo_AddComponent) {
        void* args[] = { raycasterType };
        void* exc = nullptr;
        UnityEngine::il2cpp_runtime_invoke(UnityEngine::methodInfo_AddComponent, go, args, &exc);
    }

    // DontDestroyOnLoad
    UnityEngine::Object_DontDestroyOnLoad(reinterpret_cast<Object*>(go), nullptr);

    // Activate
    UnityEngine::GameObject_SetActive(go, true, nullptr);

    m_gameObject = go;
    LOGI("Canvas::Create: Created successfully (sortingOrder=%d)", sortingOrder);
    return true;
}

void Canvas::Destroy() {
    if (m_gameObject) {
        UnityEngine::GameObject_SetActive(m_gameObject, false, nullptr);
        m_gameObject = nullptr;
    }
}

} // namespace renderer::ui
