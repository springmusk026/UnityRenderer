#pragma once

// =============================================================================
// core/Types.hpp - All IL2CPP/Unity struct definitions and enums
//
// Single canonical header for type definitions in the new OOP layer.
// Re-exports Il2CppType.h structs and adds Unity object layouts + enumerations.
// =============================================================================

#include "../Il2cpp/Il2CppType.h"
#include <cstdint>
#include <cmath>

namespace renderer::core {

// =============================================================================
// Value type aliases
// =============================================================================

using Vec2 = Il2CppVector2;
using Vec3 = Il2CppVector3;
using Color = Il2CppColor;
using Rect = Il2CppRect;
using Quaternion = Il2CppQuaternion;

// =============================================================================
// Enumerations
// =============================================================================

enum class RenderMode : int {
    ScreenSpaceOverlay = 0,
    ScreenSpaceCamera = 1,
    WorldSpace = 2
};

enum class CanvasScaleMode : int {
    ConstantPixelSize = 0,
    ScaleWithScreenSize = 1,
    ConstantPhysicalSize = 2
};

enum class ScreenMatchMode : int {
    MatchWidthOrHeight = 0,
    Expand = 1,
    Shrink = 2
};

enum class ImageType : int {
    Simple = 0,
    Sliced = 1,
    Tiled = 2,
    Filled = 3
};

enum class TextAnchor : int {
    UpperLeft = 0,
    UpperCenter = 1,
    UpperRight = 2,
    MiddleLeft = 3,
    MiddleCenter = 4,
    MiddleRight = 5,
    LowerLeft = 6,
    LowerCenter = 7,
    LowerRight = 8
};

enum class HorizontalWrapMode : int {
    Wrap = 0,
    Overflow = 1
};

enum class VerticalWrapMode : int {
    Truncate = 0,
    Overflow = 1
};

enum class FontStyle : int {
    Normal = 0,
    Bold = 1,
    Italic = 2,
    BoldAndItalic = 3
};

enum class ContentSizeFitMode : int {
    Unconstrained = 0,
    MinSize = 1,
    PreferredSize = 2
};

enum class ScrollbarVisibility : int {
    Permanent = 0,
    AutoHide = 1,
    AutoHideAndExpandViewport = 2
};

enum class TouchPhase : int {
    Began = 0,
    Moved = 1,
    Stationary = 2,
    Ended = 3,
    Canceled = 4
};

// =============================================================================
// Unity object hierarchy (in-memory IL2CPP layouts)
// =============================================================================

struct Object : Il2CppObjectx {
    void* m_CachedPtr;
};

struct Component : Object {};
struct Behaviour : Component {};
struct MonoBehaviour : Behaviour {};
struct UIBehaviour : MonoBehaviour {};

struct Transform : Component {};
struct RectTransform : Transform {};
struct GameObject : Object {};

struct Canvas : Behaviour {};
struct CanvasScaler : UIBehaviour {};
struct GraphicRaycaster : Component {};
struct EventSystem : UIBehaviour {};
struct StandaloneInputModule : MonoBehaviour {};

struct Graphic : UIBehaviour {};
struct MaskableGraphic : Graphic {};
struct Image : MaskableGraphic {};
struct RawImage : MaskableGraphic {};
struct Text : MaskableGraphic {};

struct Selectable : UIBehaviour {};
struct Button : Selectable {};

struct ContentSizeFitter : UIBehaviour {};
struct VerticalLayoutGroup : MonoBehaviour {};
struct HorizontalLayoutGroup : MonoBehaviour {};
struct LayoutElement : UIBehaviour {};
struct ScrollRect : UIBehaviour {};

// =============================================================================
// Touch struct (matches UnityEngine.Touch layout in IL2CPP)
// =============================================================================

struct UnityTouch {
    int fingerId;
    Vec2 position;
    Vec2 rawPosition;
    Vec2 deltaPosition;
    float deltaTime;
    int tapCount;
    int phase;
    int type;
    float pressure;
    float maximumPossiblePressure;
    float radius;
    float radiusVariance;
    float altitudeAngle;
    float azimuthAngle;
};

} // namespace renderer::core
