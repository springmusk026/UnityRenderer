// =============================================================================
// core/UnityEngine.cpp - Resolve all Unity function pointers and class caches
// =============================================================================

#include "UnityEngine.hpp"
#include "../util/Log.hpp"
#include "../Il2cpp/Obfuscate.h"

#include <xdl.h>

namespace renderer::core {

// Helper macros for concise resolution
#define RESOLVE_METHOD(var, img, ns, cls, name, argc) \
    var = reinterpret_cast<decltype(var)>( \
        Il2CppResolver::Get().ResolveMethod(img, ns, cls, name, argc)); \
    if (!var) LOGE("UnityEngine: Failed to resolve %s::%s::%s", ns, cls, name)

#define RESOLVE_CLASS(var, img, ns, cls) \
    var = Il2CppResolver::Get().ResolveClass(img, ns, cls); \
    if (!var) LOGE("UnityEngine: Failed to resolve class %s::%s", ns, cls)

bool UnityEngine::Init() {
    LOGI("UnityEngine: Initializing...");

    auto& resolver = Il2CppResolver::Get();

    // --- IL2CPP runtime exports (via xdl, not class methods) ---
    void* handle = xdl_open(YK("libil2cpp.so"), 0);
    if (!handle) {
        LOGE("UnityEngine: Failed to open libil2cpp.so");
        return false;
    }

    il2cpp_string_new = reinterpret_cast<il2cpp_string_new_t>(
        xdl_sym(handle, YK("il2cpp_string_new"), nullptr));
    il2cpp_type_get_object = reinterpret_cast<il2cpp_type_get_object_t>(
        xdl_sym(handle, YK("il2cpp_type_get_object"), nullptr));
    il2cpp_class_get_type = reinterpret_cast<il2cpp_class_get_type_t>(
        xdl_sym(handle, YK("il2cpp_class_get_type"), nullptr));
    il2cpp_runtime_invoke = reinterpret_cast<il2cpp_runtime_invoke_t>(
        xdl_sym(handle, YK("il2cpp_runtime_invoke"), nullptr));
    il2cpp_class_get_method_from_name_fn = reinterpret_cast<il2cpp_class_get_method_from_name_t>(
        xdl_sym(handle, YK("il2cpp_class_get_method_from_name"), nullptr));

    if (!il2cpp_string_new || !il2cpp_type_get_object || !il2cpp_class_get_type) {
        LOGE("UnityEngine: Failed to resolve IL2CPP runtime exports");
        return false;
    }

    // --- Resolve classes ---
    RESOLVE_CLASS(klass_GameObject,           IMG_CORE,       "UnityEngine",              "GameObject");
    RESOLVE_CLASS(klass_Transform,            IMG_CORE,       "UnityEngine",              "Transform");
    RESOLVE_CLASS(klass_RectTransform,        IMG_CORE,       "UnityEngine",              "RectTransform");
    RESOLVE_CLASS(klass_Canvas,               IMG_UI_MODULE,  "UnityEngine",              "Canvas");
    RESOLVE_CLASS(klass_CanvasScaler,         IMG_UI,         "UnityEngine.UI",           "CanvasScaler");
    RESOLVE_CLASS(klass_GraphicRaycaster,     IMG_UI,         "UnityEngine.UI",           "GraphicRaycaster");
    RESOLVE_CLASS(klass_Image,                IMG_UI,         "UnityEngine.UI",           "Image");
    RESOLVE_CLASS(klass_RawImage,             IMG_UI,         "UnityEngine.UI",           "RawImage");
    RESOLVE_CLASS(klass_Text,                 IMG_UI,         "UnityEngine.UI",           "Text");
    RESOLVE_CLASS(klass_Button,               IMG_UI,         "UnityEngine.UI",           "Button");
    RESOLVE_CLASS(klass_EventSystem,          IMG_UI,         "UnityEngine.EventSystems", "EventSystem");
    RESOLVE_CLASS(klass_StandaloneInputModule,IMG_UI,         "UnityEngine.EventSystems", "StandaloneInputModule");
    RESOLVE_CLASS(klass_ContentSizeFitter,    IMG_UI,         "UnityEngine.UI",           "ContentSizeFitter");
    RESOLVE_CLASS(klass_VerticalLayoutGroup,  IMG_UI,         "UnityEngine.UI",           "VerticalLayoutGroup");
    RESOLVE_CLASS(klass_HorizontalLayoutGroup,IMG_UI,         "UnityEngine.UI",           "HorizontalLayoutGroup");
    RESOLVE_CLASS(klass_ScrollRect,           IMG_UI,         "UnityEngine.UI",           "ScrollRect");
    RESOLVE_CLASS(klass_Font,                 IMG_TEXTRENDER, "UnityEngine",              "Font");

    // --- GameObject methods ---
    RESOLVE_METHOD(GameObject_ctor,          IMG_CORE, "UnityEngine", "GameObject", ".ctor", 1);
    RESOLVE_METHOD(GameObject_AddComponent,  IMG_CORE, "UnityEngine", "GameObject", "AddComponent", 1);
    RESOLVE_METHOD(GameObject_get_transform, IMG_CORE, "UnityEngine", "GameObject", "get_transform", 0);
    RESOLVE_METHOD(GameObject_SetActive,     IMG_CORE, "UnityEngine", "GameObject", "SetActive", 1);

    // Cache MethodInfo for runtime_invoke
    if (il2cpp_class_get_method_from_name_fn && klass_GameObject) {
        methodInfo_AddComponent = il2cpp_class_get_method_from_name_fn(
            klass_GameObject, "AddComponent", 1);
        methodInfo_GameObject_ctor = il2cpp_class_get_method_from_name_fn(
            klass_GameObject, ".ctor", 1);
    }

    // --- Object ---
    RESOLVE_METHOD(Object_DontDestroyOnLoad, IMG_CORE, "UnityEngine", "Object", "DontDestroyOnLoad", 1);

    // --- Transform ---
    RESOLVE_METHOD(Transform_SetParent, IMG_CORE, "UnityEngine", "Transform", "SetParent", 2);

    // --- RectTransform ---
    RESOLVE_METHOD(RectTransform_set_anchoredPosition, IMG_CORE, "UnityEngine", "RectTransform", "set_anchoredPosition", 1);
    RESOLVE_METHOD(RectTransform_set_sizeDelta,        IMG_CORE, "UnityEngine", "RectTransform", "set_sizeDelta", 1);
    RESOLVE_METHOD(RectTransform_set_anchorMin,        IMG_CORE, "UnityEngine", "RectTransform", "set_anchorMin", 1);
    RESOLVE_METHOD(RectTransform_set_anchorMax,        IMG_CORE, "UnityEngine", "RectTransform", "set_anchorMax", 1);
    RESOLVE_METHOD(RectTransform_set_pivot,            IMG_CORE, "UnityEngine", "RectTransform", "set_pivot", 1);
    RESOLVE_METHOD(RectTransform_set_offsetMin,        IMG_CORE, "UnityEngine", "RectTransform", "set_offsetMin", 1);
    RESOLVE_METHOD(RectTransform_set_offsetMax,        IMG_CORE, "UnityEngine", "RectTransform", "set_offsetMax", 1);
    RESOLVE_METHOD(RectTransform_get_sizeDelta,        IMG_CORE, "UnityEngine", "RectTransform", "get_sizeDelta", 0);

    // --- Canvas ---
    RESOLVE_METHOD(Canvas_set_renderMode,   IMG_UI_MODULE, "UnityEngine", "Canvas", "set_renderMode", 1);
    RESOLVE_METHOD(Canvas_set_sortingOrder, IMG_UI_MODULE, "UnityEngine", "Canvas", "set_sortingOrder", 1);

    // --- CanvasScaler ---
    RESOLVE_METHOD(CanvasScaler_set_uiScaleMode,         IMG_UI, "UnityEngine.UI", "CanvasScaler", "set_uiScaleMode", 1);
    RESOLVE_METHOD(CanvasScaler_set_referenceResolution,  IMG_UI, "UnityEngine.UI", "CanvasScaler", "set_referenceResolution", 1);
    RESOLVE_METHOD(CanvasScaler_set_matchWidthOrHeight,   IMG_UI, "UnityEngine.UI", "CanvasScaler", "set_matchWidthOrHeight", 1);
    RESOLVE_METHOD(CanvasScaler_set_screenMatchMode,      IMG_UI, "UnityEngine.UI", "CanvasScaler", "set_screenMatchMode", 1);

    // --- Graphic / Image ---
    RESOLVE_METHOD(Graphic_set_color,         IMG_UI, "UnityEngine.UI", "Graphic", "set_color", 1);
    RESOLVE_METHOD(Image_set_type,            IMG_UI, "UnityEngine.UI", "Image", "set_type", 1);
    RESOLVE_METHOD(Graphic_set_raycastTarget, IMG_UI, "UnityEngine.UI", "Graphic", "set_raycastTarget", 1);

    // --- Text ---
    RESOLVE_METHOD(Text_set_text,              IMG_UI, "UnityEngine.UI", "Text", "set_text", 1);
    RESOLVE_METHOD(Text_set_fontSize,          IMG_UI, "UnityEngine.UI", "Text", "set_fontSize", 1);
    RESOLVE_METHOD(Text_set_alignment,         IMG_UI, "UnityEngine.UI", "Text", "set_alignment", 1);
    RESOLVE_METHOD(Text_set_horizontalOverflow,IMG_UI, "UnityEngine.UI", "Text", "set_horizontalOverflow", 1);
    RESOLVE_METHOD(Text_set_verticalOverflow,  IMG_UI, "UnityEngine.UI", "Text", "set_verticalOverflow", 1);
    RESOLVE_METHOD(Text_get_font,              IMG_UI, "UnityEngine.UI", "Text", "get_font", 0);
    RESOLVE_METHOD(Text_set_font,              IMG_UI, "UnityEngine.UI", "Text", "set_font", 1);

    // --- Font ---
    RESOLVE_METHOD(Font_CreateDynamicFontFromOSFont, IMG_TEXTRENDER, "UnityEngine", "Font", "CreateDynamicFontFromOSFont", 2);

    // --- Selectable / Button ---
    RESOLVE_METHOD(Selectable_set_interactable,   IMG_UI, "UnityEngine.UI", "Selectable", "set_interactable", 1);
    RESOLVE_METHOD(Selectable_set_targetGraphic,  IMG_UI, "UnityEngine.UI", "Selectable", "set_targetGraphic", 1);
    RESOLVE_METHOD(Button_get_onClick,            IMG_UI, "UnityEngine.UI", "Button", "get_onClick", 0);

    // --- Input ---
    RESOLVE_METHOD(Input_get_touchCount, IMG_INPUT, "UnityEngine", "Input", "get_touchCount", 0);
    RESOLVE_METHOD(Input_GetTouch,       IMG_INPUT, "UnityEngine", "Input", "GetTouch", 1);

    // --- Screen ---
    RESOLVE_METHOD(Screen_get_width,  IMG_CORE, "UnityEngine", "Screen", "get_width", 0);
    RESOLVE_METHOD(Screen_get_height, IMG_CORE, "UnityEngine", "Screen", "get_height", 0);

    // --- EventSystem ---
    RESOLVE_METHOD(EventSystem_get_current, IMG_UI, "UnityEngine.EventSystems", "EventSystem", "get_current", 0);

    LOGI("UnityEngine: Initialization complete");
    return true;
}

#undef RESOLVE_METHOD
#undef RESOLVE_CLASS

} // namespace renderer::core
