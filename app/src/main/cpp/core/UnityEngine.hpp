#pragma once

// =============================================================================
// core/UnityEngine.hpp - Static class with all typed Unity function pointers
//
// Header-heavy design: typedefs, inline pointers, class caches all here.
// Init() implementation in UnityEngine.cpp resolves everything via Il2CppResolver.
// =============================================================================

#include "Types.hpp"
#include "Il2CppResolver.hpp"

namespace renderer::core {

// =============================================================================
// Assembly image name constants
// =============================================================================

inline constexpr const char* IMG_CORE       = "UnityEngine.CoreModule.dll";
inline constexpr const char* IMG_UI_MODULE  = "UnityEngine.UIModule.dll";
inline constexpr const char* IMG_UI         = "UnityEngine.UI.dll";
inline constexpr const char* IMG_TEXTRENDER = "UnityEngine.TextRenderingModule.dll";
inline constexpr const char* IMG_INPUT      = "UnityEngine.InputLegacyModule.dll";

// =============================================================================
// UnityEngine static class
// =============================================================================

struct UnityEngine {
    // Initialize all function pointers and class caches.
    // Call once after Il2CppResolver is initialized and assemblies are loaded.
    static bool Init();

    // =========================================================================
    // Class cache pointers (resolved once during Init)
    // =========================================================================
    static inline void* klass_GameObject = nullptr;
    static inline void* klass_Transform = nullptr;
    static inline void* klass_RectTransform = nullptr;
    static inline void* klass_Canvas = nullptr;
    static inline void* klass_CanvasScaler = nullptr;
    static inline void* klass_GraphicRaycaster = nullptr;
    static inline void* klass_Image = nullptr;
    static inline void* klass_RawImage = nullptr;
    static inline void* klass_Text = nullptr;
    static inline void* klass_Button = nullptr;
    static inline void* klass_EventSystem = nullptr;
    static inline void* klass_StandaloneInputModule = nullptr;
    static inline void* klass_ContentSizeFitter = nullptr;
    static inline void* klass_VerticalLayoutGroup = nullptr;
    static inline void* klass_HorizontalLayoutGroup = nullptr;
    static inline void* klass_ScrollRect = nullptr;
    static inline void* klass_Font = nullptr;

    // =========================================================================
    // Cached MethodInfo pointers (for il2cpp_runtime_invoke)
    // =========================================================================
    static inline void* methodInfo_AddComponent = nullptr;
    static inline void* methodInfo_GameObject_ctor = nullptr;

    // =========================================================================
    // IL2CPP runtime exports
    // =========================================================================
    using il2cpp_string_new_t = Il2CppStringx* (*)(const char*);
    static inline il2cpp_string_new_t il2cpp_string_new = nullptr;

    using il2cpp_runtime_invoke_t = void* (*)(void*, void*, void**, void**);
    static inline il2cpp_runtime_invoke_t il2cpp_runtime_invoke = nullptr;

    using il2cpp_type_get_object_t = void* (*)(void*);
    static inline il2cpp_type_get_object_t il2cpp_type_get_object = nullptr;

    using il2cpp_class_get_type_t = void* (*)(void*);
    static inline il2cpp_class_get_type_t il2cpp_class_get_type = nullptr;

    using il2cpp_class_get_method_from_name_t = void* (*)(void*, const char*, int);
    static inline il2cpp_class_get_method_from_name_t il2cpp_class_get_method_from_name_fn = nullptr;

    // =========================================================================
    // Helper: Get System.Type from Il2CppClass*
    // =========================================================================
    static inline void* GetType(void* klass) {
        if (!klass || !il2cpp_class_get_type || !il2cpp_type_get_object) return nullptr;
        void* type = il2cpp_class_get_type(klass);
        return type ? il2cpp_type_get_object(type) : nullptr;
    }

    // =========================================================================
    // GameObject
    // =========================================================================
    using GameObject_ctor_t = void (*)(GameObject*, Il2CppStringx*, void*);
    static inline GameObject_ctor_t GameObject_ctor = nullptr;

    using GameObject_AddComponent_t = Component* (*)(GameObject*, void*, void*);
    static inline GameObject_AddComponent_t GameObject_AddComponent = nullptr;

    using GameObject_get_transform_t = Transform* (*)(GameObject*, void*);
    static inline GameObject_get_transform_t GameObject_get_transform = nullptr;

    using GameObject_SetActive_t = void (*)(GameObject*, bool, void*);
    static inline GameObject_SetActive_t GameObject_SetActive = nullptr;

    // =========================================================================
    // Object
    // =========================================================================
    using Object_DontDestroyOnLoad_t = void (*)(Object*, void*);
    static inline Object_DontDestroyOnLoad_t Object_DontDestroyOnLoad = nullptr;

    // =========================================================================
    // Transform
    // =========================================================================
    using Transform_SetParent_t = void (*)(Transform*, Transform*, bool, void*);
    static inline Transform_SetParent_t Transform_SetParent = nullptr;

    // =========================================================================
    // RectTransform
    // =========================================================================
    using RectTransform_set_anchoredPosition_t = void (*)(RectTransform*, Vec2, void*);
    static inline RectTransform_set_anchoredPosition_t RectTransform_set_anchoredPosition = nullptr;

    using RectTransform_set_sizeDelta_t = void (*)(RectTransform*, Vec2, void*);
    static inline RectTransform_set_sizeDelta_t RectTransform_set_sizeDelta = nullptr;

    using RectTransform_set_anchorMin_t = void (*)(RectTransform*, Vec2, void*);
    static inline RectTransform_set_anchorMin_t RectTransform_set_anchorMin = nullptr;

    using RectTransform_set_anchorMax_t = void (*)(RectTransform*, Vec2, void*);
    static inline RectTransform_set_anchorMax_t RectTransform_set_anchorMax = nullptr;

    using RectTransform_set_pivot_t = void (*)(RectTransform*, Vec2, void*);
    static inline RectTransform_set_pivot_t RectTransform_set_pivot = nullptr;

    using RectTransform_set_offsetMin_t = void (*)(RectTransform*, Vec2, void*);
    static inline RectTransform_set_offsetMin_t RectTransform_set_offsetMin = nullptr;

    using RectTransform_set_offsetMax_t = void (*)(RectTransform*, Vec2, void*);
    static inline RectTransform_set_offsetMax_t RectTransform_set_offsetMax = nullptr;

    using RectTransform_get_sizeDelta_t = Vec2 (*)(RectTransform*, void*);
    static inline RectTransform_get_sizeDelta_t RectTransform_get_sizeDelta = nullptr;

    // =========================================================================
    // Canvas
    // =========================================================================
    using Canvas_set_renderMode_t = void (*)(Canvas*, int, void*);
    static inline Canvas_set_renderMode_t Canvas_set_renderMode = nullptr;

    using Canvas_set_sortingOrder_t = void (*)(Canvas*, int, void*);
    static inline Canvas_set_sortingOrder_t Canvas_set_sortingOrder = nullptr;

    // =========================================================================
    // CanvasScaler
    // =========================================================================
    using CanvasScaler_set_uiScaleMode_t = void (*)(CanvasScaler*, int, void*);
    static inline CanvasScaler_set_uiScaleMode_t CanvasScaler_set_uiScaleMode = nullptr;

    using CanvasScaler_set_referenceResolution_t = void (*)(CanvasScaler*, Vec2, void*);
    static inline CanvasScaler_set_referenceResolution_t CanvasScaler_set_referenceResolution = nullptr;

    using CanvasScaler_set_matchWidthOrHeight_t = void (*)(CanvasScaler*, float, void*);
    static inline CanvasScaler_set_matchWidthOrHeight_t CanvasScaler_set_matchWidthOrHeight = nullptr;

    using CanvasScaler_set_screenMatchMode_t = void (*)(CanvasScaler*, int, void*);
    static inline CanvasScaler_set_screenMatchMode_t CanvasScaler_set_screenMatchMode = nullptr;

    // =========================================================================
    // Graphic / Image
    // =========================================================================
    using Graphic_set_color_t = void (*)(Graphic*, Color, void*);
    static inline Graphic_set_color_t Graphic_set_color = nullptr;

    using Image_set_type_t = void (*)(Image*, int, void*);
    static inline Image_set_type_t Image_set_type = nullptr;

    using Graphic_set_raycastTarget_t = void (*)(Graphic*, bool, void*);
    static inline Graphic_set_raycastTarget_t Graphic_set_raycastTarget = nullptr;

    // =========================================================================
    // Text
    // =========================================================================
    using Text_set_text_t = void (*)(Text*, Il2CppStringx*, void*);
    static inline Text_set_text_t Text_set_text = nullptr;

    using Text_set_fontSize_t = void (*)(Text*, int, void*);
    static inline Text_set_fontSize_t Text_set_fontSize = nullptr;

    using Text_set_alignment_t = void (*)(Text*, int, void*);
    static inline Text_set_alignment_t Text_set_alignment = nullptr;

    using Text_set_horizontalOverflow_t = void (*)(Text*, int, void*);
    static inline Text_set_horizontalOverflow_t Text_set_horizontalOverflow = nullptr;

    using Text_set_verticalOverflow_t = void (*)(Text*, int, void*);
    static inline Text_set_verticalOverflow_t Text_set_verticalOverflow = nullptr;

    using Text_get_font_t = void* (*)(Text*, void*);
    static inline Text_get_font_t Text_get_font = nullptr;

    using Text_set_font_t = void (*)(Text*, void*, void*);
    static inline Text_set_font_t Text_set_font = nullptr;

    // =========================================================================
    // Font
    // =========================================================================
    using Font_CreateDynamicFontFromOSFont_t = void* (*)(Il2CppStringx*, int, void*);
    static inline Font_CreateDynamicFontFromOSFont_t Font_CreateDynamicFontFromOSFont = nullptr;

    // =========================================================================
    // Selectable / Button
    // =========================================================================
    using Selectable_set_interactable_t = void (*)(Selectable*, bool, void*);
    static inline Selectable_set_interactable_t Selectable_set_interactable = nullptr;

    using Selectable_set_targetGraphic_t = void (*)(Selectable*, Graphic*, void*);
    static inline Selectable_set_targetGraphic_t Selectable_set_targetGraphic = nullptr;

    using Button_get_onClick_t = void* (*)(Button*, void*);
    static inline Button_get_onClick_t Button_get_onClick = nullptr;

    // =========================================================================
    // Input
    // =========================================================================
    using Input_get_touchCount_t = int (*)(void*);
    static inline Input_get_touchCount_t Input_get_touchCount = nullptr;

    using Input_GetTouch_t = UnityTouch (*)(int, void*);
    static inline Input_GetTouch_t Input_GetTouch = nullptr;

    // =========================================================================
    // Screen
    // =========================================================================
    using Screen_get_width_t = int (*)(void*);
    static inline Screen_get_width_t Screen_get_width = nullptr;

    using Screen_get_height_t = int (*)(void*);
    static inline Screen_get_height_t Screen_get_height = nullptr;

    // =========================================================================
    // EventSystem
    // =========================================================================
    using EventSystem_get_current_t = void* (*)(void*);
    static inline EventSystem_get_current_t EventSystem_get_current = nullptr;

    // =========================================================================
    // UnityEvent
    // =========================================================================
    using UnityEvent_AddListener_t = void (*)(void*, void*, void*);
    static inline UnityEvent_AddListener_t UnityEvent_AddListener = nullptr;
};

} // namespace renderer::core
