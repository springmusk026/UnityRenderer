#pragma once

// =============================================================================
// ui/Canvas.hpp - Canvas wrapper (Canvas + CanvasScaler + GraphicRaycaster)
// =============================================================================

#include "../core/Types.hpp"
#include "../core/UnityEngine.hpp"

namespace renderer::ui {

using namespace renderer::core;

class Canvas {
public:
    Canvas() = default;
    ~Canvas();

    // Create the Canvas hierarchy: GO → Canvas → CanvasScaler → GraphicRaycaster
    // DontDestroyOnLoad is applied automatically.
    bool Create(int sortingOrder = 100, Vec2 referenceResolution = {1080, 1920});

    // Destroy the canvas (hides GO; GC handles memory)
    void Destroy();

    GameObject* GetGameObject() const { return m_gameObject; }

private:
    GameObject* m_gameObject = nullptr;
};

} // namespace renderer::ui
