// =============================================================================
// ui/Widget.cpp - Abstract base class implementation
// =============================================================================

#include "Widget.hpp"
#include "../util/Log.hpp"
#include "../core/Il2CppResolver.hpp"

namespace renderer::ui {

// Destructor: hide the GO (IL2CPP GC manages actual memory)
Widget::~Widget() {
    if (m_gameObject) {
        UnityEngine::GameObject_SetActive(m_gameObject, false, nullptr);
    }
}

void Widget::SetVisible(bool visible) {
    m_visible = visible;
    if (m_gameObject) {
        UnityEngine::GameObject_SetActive(m_gameObject, visible, nullptr);
    }
}

// =============================================================================
// Static: CreateContainerGO
// Order: NewGO → SetParent → AddImage → Configure RectTransform
// =============================================================================
GameObject* Widget::CreateContainerGO(const char* name, void* parent,
                                       Vec2 size, Vec2 position) {
    // 1. Create new GameObject
    auto* go = reinterpret_cast<GameObject*>(
        Il2CppResolver::Get().NewObject(IMG_CORE, "UnityEngine", "GameObject"));
    if (!go) {
        LOGE("Widget::CreateContainerGO: NewObject failed for '%s'", name);
        return nullptr;
    }

    // Call .ctor(name)
    auto* str = UnityEngine::il2cpp_string_new(name);
    if (UnityEngine::il2cpp_runtime_invoke && UnityEngine::methodInfo_GameObject_ctor) {
        void* args[] = { str };
        void* exc = nullptr;
        UnityEngine::il2cpp_runtime_invoke(UnityEngine::methodInfo_GameObject_ctor, go, args, &exc);
        if (exc) {
            LOGE("Widget::CreateContainerGO: ctor exception for '%s'", name);
            return nullptr;
        }
    } else if (UnityEngine::GameObject_ctor) {
        UnityEngine::GameObject_ctor(go, str, nullptr);
    }

    // 2. SetParent
    if (parent) {
        auto* childT = UnityEngine::GameObject_get_transform(go, nullptr);
        auto* parentT = UnityEngine::GameObject_get_transform(
            reinterpret_cast<GameObject*>(parent), nullptr);
        if (childT && parentT) {
            UnityEngine::Transform_SetParent(childT, parentT, false, nullptr);
        }
    }

    // 3. AddComponent<Image> - MUST be before RectTransform configuration
    void* type = UnityEngine::GetType(UnityEngine::klass_Image);
    if (type && UnityEngine::il2cpp_runtime_invoke && UnityEngine::methodInfo_AddComponent) {
        void* args[] = { type };
        void* exc = nullptr;
        auto* img = reinterpret_cast<Image*>(
            UnityEngine::il2cpp_runtime_invoke(UnityEngine::methodInfo_AddComponent, go, args, &exc));
        if (img) {
            // Default: semi-transparent panel color, raycast enabled
            UnityEngine::Graphic_set_color(reinterpret_cast<Graphic*>(img),
                                           {0.12f, 0.12f, 0.18f, 1.0f}, nullptr);
            if (UnityEngine::Graphic_set_raycastTarget) {
                UnityEngine::Graphic_set_raycastTarget(
                    reinterpret_cast<Graphic*>(img), true, nullptr);
            }
        }
    }

    // 4. Configure RectTransform (Image creation auto-promotes Transform → RectTransform)
    auto* rt = reinterpret_cast<RectTransform*>(
        UnityEngine::GameObject_get_transform(go, nullptr));
    if (rt) {
        UnityEngine::RectTransform_set_anchorMin(rt, {0.5f, 0.5f}, nullptr);
        UnityEngine::RectTransform_set_anchorMax(rt, {0.5f, 0.5f}, nullptr);
        UnityEngine::RectTransform_set_sizeDelta(rt, size, nullptr);
        UnityEngine::RectTransform_set_anchoredPosition(rt, position, nullptr);
    }

    return go;
}

// =============================================================================
// Static: CreateTextChild
// Creates a child GO with ONLY a Text component (no Image - one Graphic rule)
// =============================================================================
Text* Widget::CreateTextChild(void* parent, const char* text, int fontSize,
                              int alignment, Color color) {
    // Create child GO
    auto* go = reinterpret_cast<GameObject*>(
        Il2CppResolver::Get().NewObject(IMG_CORE, "UnityEngine", "GameObject"));
    if (!go) {
        LOGE("Widget::CreateTextChild: NewObject failed");
        return nullptr;
    }

    // Call .ctor
    auto* str = UnityEngine::il2cpp_string_new("Text");
    if (UnityEngine::il2cpp_runtime_invoke && UnityEngine::methodInfo_GameObject_ctor) {
        void* args[] = { str };
        void* exc = nullptr;
        UnityEngine::il2cpp_runtime_invoke(UnityEngine::methodInfo_GameObject_ctor, go, args, &exc);
    } else if (UnityEngine::GameObject_ctor) {
        UnityEngine::GameObject_ctor(go, str, nullptr);
    }

    // SetParent
    if (parent) {
        auto* childT = UnityEngine::GameObject_get_transform(go, nullptr);
        auto* parentT = UnityEngine::GameObject_get_transform(
            reinterpret_cast<GameObject*>(parent), nullptr);
        if (childT && parentT) {
            UnityEngine::Transform_SetParent(childT, parentT, false, nullptr);
        }
    }

    // Add Text component (NO Image on this GO)
    void* type = UnityEngine::GetType(UnityEngine::klass_Text);
    if (!type) {
        LOGE("Widget::CreateTextChild: GetType(Text) failed");
        return nullptr;
    }

    Text* textComp = nullptr;
    if (UnityEngine::il2cpp_runtime_invoke && UnityEngine::methodInfo_AddComponent) {
        void* args[] = { type };
        void* exc = nullptr;
        textComp = reinterpret_cast<Text*>(
            UnityEngine::il2cpp_runtime_invoke(UnityEngine::methodInfo_AddComponent, go, args, &exc));
    }

    if (!textComp) {
        LOGE("Widget::CreateTextChild: AddComponent<Text> failed");
        return nullptr;
    }

    // Configure Text
    UnityEngine::Text_set_text(textComp, UnityEngine::il2cpp_string_new(text), nullptr);
    UnityEngine::Text_set_fontSize(textComp, fontSize, nullptr);
    UnityEngine::Text_set_alignment(textComp, alignment, nullptr);
    UnityEngine::Graphic_set_color(reinterpret_cast<Graphic*>(textComp), color, nullptr);

    if (UnityEngine::Text_set_horizontalOverflow) {
        UnityEngine::Text_set_horizontalOverflow(textComp, (int)HorizontalWrapMode::Overflow, nullptr);
    }
    if (UnityEngine::Text_set_verticalOverflow) {
        UnityEngine::Text_set_verticalOverflow(textComp, (int)VerticalWrapMode::Overflow, nullptr);
    }
    if (UnityEngine::Graphic_set_raycastTarget) {
        UnityEngine::Graphic_set_raycastTarget(reinterpret_cast<Graphic*>(textComp), false, nullptr);
    }

    // Configure RectTransform: stretch-fill parent by default
    auto* rt = reinterpret_cast<RectTransform*>(
        UnityEngine::GameObject_get_transform(go, nullptr));
    if (rt) {
        UnityEngine::RectTransform_set_anchorMin(rt, {0, 0}, nullptr);
        UnityEngine::RectTransform_set_anchorMax(rt, {1, 1}, nullptr);
        UnityEngine::RectTransform_set_offsetMin(rt, {10, 0}, nullptr);
        UnityEngine::RectTransform_set_offsetMax(rt, {0, 0}, nullptr);
    }

    // Register for deferred font assignment
    s_allTexts.push_back(textComp);

    return textComp;
}

} // namespace renderer::ui
