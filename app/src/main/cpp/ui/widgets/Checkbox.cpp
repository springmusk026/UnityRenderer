// =============================================================================
// ui/widgets/Checkbox.cpp - Checkbox widget implementation
// =============================================================================

#include "Checkbox.hpp"
#include "../../util/Log.hpp"

namespace renderer::ui::widgets {

static constexpr Color COLOR_BG        = {0.12f, 0.12f, 0.18f, 1.0f};
static constexpr Color COLOR_CHECK     = {1.0f, 0.8f, 0.2f, 1.0f};
static constexpr Color COLOR_TEXT      = {1.0f, 1.0f, 1.0f, 1.0f};

Checkbox::Checkbox(const char* name, bool initialState, int id)
    : m_checked(initialState), m_id(id), m_name(name) {
    m_size = {350, 45};
}

void Checkbox::Create(void* parent) {
    auto& resolver = Il2CppResolver::Get();

    // ---- Container: NewGO → Parent → AddImage → ConfigureRT ----
    auto* go = reinterpret_cast<GameObject*>(
        resolver.NewObject(IMG_CORE, "UnityEngine", "GameObject"));
    if (!go) return;

    auto* str = UnityEngine::il2cpp_string_new("Checkbox");
    if (UnityEngine::il2cpp_runtime_invoke && UnityEngine::methodInfo_GameObject_ctor) {
        void* args[] = { str };
        void* exc = nullptr;
        UnityEngine::il2cpp_runtime_invoke(UnityEngine::methodInfo_GameObject_ctor, go, args, &exc);
    } else if (UnityEngine::GameObject_ctor) {
        UnityEngine::GameObject_ctor(go, str, nullptr);
    }

    if (parent) {
        auto* childT = UnityEngine::GameObject_get_transform(go, nullptr);
        auto* parentT = UnityEngine::GameObject_get_transform(
            reinterpret_cast<GameObject*>(parent), nullptr);
        if (childT && parentT)
            UnityEngine::Transform_SetParent(childT, parentT, false, nullptr);
    }

    // AddImage
    void* imgType = UnityEngine::GetType(UnityEngine::klass_Image);
    if (imgType && UnityEngine::il2cpp_runtime_invoke && UnityEngine::methodInfo_AddComponent) {
        void* args[] = { imgType };
        void* exc = nullptr;
        m_bgImage = reinterpret_cast<Image*>(
            UnityEngine::il2cpp_runtime_invoke(UnityEngine::methodInfo_AddComponent, go, args, &exc));
    }
    if (m_bgImage) {
        UnityEngine::Graphic_set_color(reinterpret_cast<Graphic*>(m_bgImage), COLOR_BG, nullptr);
        if (UnityEngine::Graphic_set_raycastTarget)
            UnityEngine::Graphic_set_raycastTarget(reinterpret_cast<Graphic*>(m_bgImage), true, nullptr);
    }

    // Configure container RT
    auto* rt = reinterpret_cast<RectTransform*>(UnityEngine::GameObject_get_transform(go, nullptr));
    if (rt) {
        UnityEngine::RectTransform_set_anchorMin(rt, {0.5f, 0.5f}, nullptr);
        UnityEngine::RectTransform_set_anchorMax(rt, {0.5f, 0.5f}, nullptr);
        UnityEngine::RectTransform_set_sizeDelta(rt, m_size, nullptr);
        UnityEngine::RectTransform_set_anchoredPosition(rt, m_position, nullptr);
    }
    m_gameObject = go;

    // ---- Checkmark Text (left side) ----
    // Create child GO for checkmark
    auto* checkGO = reinterpret_cast<GameObject*>(
        resolver.NewObject(IMG_CORE, "UnityEngine", "GameObject"));
    if (checkGO) {
        auto* cstr = UnityEngine::il2cpp_string_new("Check");
        if (UnityEngine::il2cpp_runtime_invoke && UnityEngine::methodInfo_GameObject_ctor) {
            void* args[] = { cstr };
            void* exc = nullptr;
            UnityEngine::il2cpp_runtime_invoke(UnityEngine::methodInfo_GameObject_ctor, checkGO, args, &exc);
        } else if (UnityEngine::GameObject_ctor) {
            UnityEngine::GameObject_ctor(checkGO, cstr, nullptr);
        }

        auto* cChildT = UnityEngine::GameObject_get_transform(checkGO, nullptr);
        auto* cParentT = UnityEngine::GameObject_get_transform(go, nullptr);
        if (cChildT && cParentT)
            UnityEngine::Transform_SetParent(cChildT, cParentT, false, nullptr);

        // Add Text to checkmark GO
        void* txtType = UnityEngine::GetType(UnityEngine::klass_Text);
        if (txtType && UnityEngine::il2cpp_runtime_invoke && UnityEngine::methodInfo_AddComponent) {
            void* args[] = { txtType };
            void* exc = nullptr;
            m_checkMark = reinterpret_cast<Text*>(
                UnityEngine::il2cpp_runtime_invoke(UnityEngine::methodInfo_AddComponent, checkGO, args, &exc));
        }
        if (m_checkMark) {
            const char* mark = m_checked ? "[X]" : "[ ]";
            UnityEngine::Text_set_text(m_checkMark, UnityEngine::il2cpp_string_new(mark), nullptr);
            UnityEngine::Text_set_fontSize(m_checkMark, 15, nullptr);
            UnityEngine::Text_set_alignment(m_checkMark, (int)TextAnchor::MiddleCenter, nullptr);
            UnityEngine::Graphic_set_color(reinterpret_cast<Graphic*>(m_checkMark), COLOR_CHECK, nullptr);
            if (UnityEngine::Graphic_set_raycastTarget)
                UnityEngine::Graphic_set_raycastTarget(reinterpret_cast<Graphic*>(m_checkMark), false, nullptr);
            s_allTexts.push_back(m_checkMark);
        }

        // Checkmark RT: left portion
        auto* crt = reinterpret_cast<RectTransform*>(
            UnityEngine::GameObject_get_transform(checkGO, nullptr));
        if (crt) {
            UnityEngine::RectTransform_set_anchorMin(crt, {0, 0}, nullptr);
            UnityEngine::RectTransform_set_anchorMax(crt, {0.15f, 1.0f}, nullptr);
            UnityEngine::RectTransform_set_offsetMin(crt, {5, 0}, nullptr);
            UnityEngine::RectTransform_set_offsetMax(crt, {0, 0}, nullptr);
        }
    }

    // ---- Label Text (right of checkmark) ----
    auto* labelGO = reinterpret_cast<GameObject*>(
        resolver.NewObject(IMG_CORE, "UnityEngine", "GameObject"));
    if (labelGO) {
        auto* lstr = UnityEngine::il2cpp_string_new("Label");
        if (UnityEngine::il2cpp_runtime_invoke && UnityEngine::methodInfo_GameObject_ctor) {
            void* args[] = { lstr };
            void* exc = nullptr;
            UnityEngine::il2cpp_runtime_invoke(UnityEngine::methodInfo_GameObject_ctor, labelGO, args, &exc);
        } else if (UnityEngine::GameObject_ctor) {
            UnityEngine::GameObject_ctor(labelGO, lstr, nullptr);
        }

        auto* lChildT = UnityEngine::GameObject_get_transform(labelGO, nullptr);
        auto* lParentT = UnityEngine::GameObject_get_transform(go, nullptr);
        if (lChildT && lParentT)
            UnityEngine::Transform_SetParent(lChildT, lParentT, false, nullptr);

        // Add Text
        void* txtType = UnityEngine::GetType(UnityEngine::klass_Text);
        if (txtType && UnityEngine::il2cpp_runtime_invoke && UnityEngine::methodInfo_AddComponent) {
            void* args[] = { txtType };
            void* exc = nullptr;
            m_label = reinterpret_cast<Text*>(
                UnityEngine::il2cpp_runtime_invoke(UnityEngine::methodInfo_AddComponent, labelGO, args, &exc));
        }
        if (m_label) {
            UnityEngine::Text_set_text(m_label, UnityEngine::il2cpp_string_new(m_name), nullptr);
            UnityEngine::Text_set_fontSize(m_label, 15, nullptr);
            UnityEngine::Text_set_alignment(m_label, (int)TextAnchor::MiddleLeft, nullptr);
            UnityEngine::Graphic_set_color(reinterpret_cast<Graphic*>(m_label), COLOR_TEXT, nullptr);
            if (UnityEngine::Text_set_horizontalOverflow)
                UnityEngine::Text_set_horizontalOverflow(m_label, (int)HorizontalWrapMode::Overflow, nullptr);
            if (UnityEngine::Graphic_set_raycastTarget)
                UnityEngine::Graphic_set_raycastTarget(reinterpret_cast<Graphic*>(m_label), false, nullptr);
            s_allTexts.push_back(m_label);
        }

        // Label RT: right portion
        auto* lrt = reinterpret_cast<RectTransform*>(
            UnityEngine::GameObject_get_transform(labelGO, nullptr));
        if (lrt) {
            UnityEngine::RectTransform_set_anchorMin(lrt, {0.15f, 0}, nullptr);
            UnityEngine::RectTransform_set_anchorMax(lrt, {1.0f, 1.0f}, nullptr);
            UnityEngine::RectTransform_set_offsetMin(lrt, {5, 0}, nullptr);
            UnityEngine::RectTransform_set_offsetMax(lrt, {0, 0}, nullptr);
        }
    }
}

void Checkbox::OnTouch(int phase, Vec2 localPos) {
    if (phase == (int)TouchPhase::Began) {
        m_checked = !m_checked;
        UpdateVisuals();
    }
}

void Checkbox::AssignFont(void* font) {
    if (font && UnityEngine::Text_set_font) {
        if (m_checkMark) UnityEngine::Text_set_font(m_checkMark, font, nullptr);
        if (m_label) UnityEngine::Text_set_font(m_label, font, nullptr);
    }
}

void Checkbox::UpdateVisuals() {
    if (m_checkMark) {
        const char* mark = m_checked ? "[X]" : "[ ]";
        UnityEngine::Text_set_text(m_checkMark, UnityEngine::il2cpp_string_new(mark), nullptr);
    }
}

} // namespace renderer::ui::widgets
