#pragma once

// =============================================================================
// input/TouchInput.hpp - Touch polling and coordinate conversion
//
// Must be called from the render thread (Unity's main thread) since
// Input.GetTouch / Input.get_touchCount require that context.
// =============================================================================

#include "../core/Types.hpp"

namespace renderer::input {

using renderer::core::Vec2;
using renderer::core::TouchPhase;

// =============================================================================
// TouchData - simplified touch info for the UI layer
// =============================================================================

struct TouchData {
    Vec2 position;      // Screen-space position
    int phase;          // TouchPhase enum value
    int fingerId;
};

// =============================================================================
// TouchInput - static utility class for touch polling
// =============================================================================

struct TouchInput {
    // Poll current touch state (call once per frame from render thread)
    static void Poll();

    // Number of active touches this frame
    static int GetTouchCount();

    // Get touch data by index (0..GetTouchCount()-1)
    static TouchData GetTouch(int index);

    // Convert screen-space position to canvas-space given reference resolution
    static Vec2 ScreenToCanvas(Vec2 screenPos, float refW, float refH);

private:
    static inline int s_touchCount = 0;
    static inline TouchData s_touches[10] = {};  // Max 10 simultaneous touches
};

} // namespace renderer::input
