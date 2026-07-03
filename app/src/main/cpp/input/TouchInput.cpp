// =============================================================================
// input/TouchInput.cpp - Touch polling implementation
// =============================================================================

#include "TouchInput.hpp"
#include "../core/UnityEngine.hpp"
#include "../util/Log.hpp"

namespace renderer::input {

using namespace renderer::core;

void TouchInput::Poll() {
    s_touchCount = 0;

    if (!UnityEngine::Input_get_touchCount) return;

    int count = UnityEngine::Input_get_touchCount(nullptr);
    if (count <= 0) return;
    if (count > 10) count = 10;  // Clamp to our buffer size

    if (!UnityEngine::Input_GetTouch) return;

    for (int i = 0; i < count; i++) {
        UnityTouch touch = UnityEngine::Input_GetTouch(i, nullptr);
        s_touches[i].position = touch.position;
        s_touches[i].phase = touch.phase;
        s_touches[i].fingerId = touch.fingerId;
    }

    s_touchCount = count;
}

int TouchInput::GetTouchCount() {
    return s_touchCount;
}

TouchData TouchInput::GetTouch(int index) {
    if (index < 0 || index >= s_touchCount) {
        return {{0, 0}, static_cast<int>(TouchPhase::Canceled), -1};
    }
    return s_touches[index];
}

Vec2 TouchInput::ScreenToCanvas(Vec2 screenPos, float refW, float refH) {
    // Get current screen dimensions
    float screenW = 1080.0f;
    float screenH = 1920.0f;

    if (UnityEngine::Screen_get_width) {
        int w = UnityEngine::Screen_get_width(nullptr);
        if (w > 0) screenW = static_cast<float>(w);
    }
    if (UnityEngine::Screen_get_height) {
        int h = UnityEngine::Screen_get_height(nullptr);
        if (h > 0) screenH = static_cast<float>(h);
    }

    // Convert screen position to canvas coordinates (center-origin, Y-up)
    // This matches Unity's ScreenSpaceOverlay Canvas with anchor (0.5, 0.5)
    // Unity screen origin is bottom-left, positive Y goes up
    float cx = (screenPos.x - screenW * 0.5f) * (refW / screenW);
    float cy = (screenPos.y - screenH * 0.5f) * (refH / screenH);

    return {cx, cy};
}

} // namespace renderer::input
