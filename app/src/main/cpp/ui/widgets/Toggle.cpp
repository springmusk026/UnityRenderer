// =============================================================================
// ui/widgets/Toggle.cpp - Toggle button widget implementation
// =============================================================================

#include "Toggle.hpp"
#include "../../util/Log.hpp"

namespace renderer::ui::widgets {

static constexpr Color COLOR_OFF  = {0.2f, 0.2f, 0.35f, 1.0f};
static constexpr Color COLOR_ON   = {0.2f, 0.7f, 0.2f, 1.0f};
static constexpr Color COLOR_TEXT = {1.0f, 1.0f, 1.0f, 1.0f};

Toggle::Toggle(const char* nameOn, const char* nameOff, bool initialState, int id)
    : m_enabled(initialState), m_id(id), m_nameOn(nameOn), m_nameOff(nameOff) {
    m_size = {350, 45};
}

void Toggle::Create(void* parent) {
    auto& resolver = Il2CppResolver::Get();

    // 1. NewGO
    auto* go = reinterpret_cast<GameObject*>(
        resolver.NewObject(IMG_CORE, "UnityEngine", "GameObject"));
    if (!go) return;

    auto* str = UnityEngine::il2cpp_string_new("Toggle");
    if (UnityEngine::il2cpp_runtime_invoke && UnityEngine::methodInfo_GameObject_ctor) {
        void* args[] = { str };
        void* exc = nullptr;
        UnityEngine::il2cpp_runtime_invoke(UnityEngine::methodInfo_GameObject_ctor, go, args, &exc);
    } else if (UnityEngine::GameObject_ctor) {
        UnityEngine::GameObject_ctor(go, str, nullptr);
    }

    // 2. SetParent
    if (parent) {
        auto* childT = UnityEngine::GameObject_get_transform(go, nullptr);
        auto* parentT = UnityEngine::GameObject_get_transform(
            reinterpret_cast<GameObject*>(parent), nullptr);
        if (childT && parentT)
            UnityEngine::Transform_SetParent(childT, parentT, false, nullptr);
    }

    // 3. AddComponent<Image> - MUST be before RectTransform configuration
    void* imgType = UnityEngine::GetType(UnityEngine::klass_Image);
    if (imgType && UnityEngine::il2cpp_runtime_invoke && UnityEngine::methodInfo_AddComponent) {
        void* args[] = { imgType };
        void* exc = nullptr;
        m_bgImage = reinterpret_cast<Image*>(
            UnityEngine::il2cpp_runtime_invoke(UnityEngine::methodInfo_AddComponent, go, args, &exc));
    }
    if (m_bgImage && UnityEngine::Graphic_set_raycastTarget) {
        UnityEngine::Graphic_set_raycastTarget(reinterpret_cast<Graphic*>(m_bgImage), true, nullptr);
    }

    // 4. Configure RectTransform
    auto* rt = reinterpret_cast<RectTransform*>(
        UnityEngine::GameObject_get_transform(go, nullptr));
    if (rt) {
        UnityEngine::RectTransform_set_anchorMin(rt, {0.5f, 0.5f}, nullptr);
        UnityEngine::RectTransform_set_anchorMax(rt, {0.5f, 0.5f}, nullptr);
        UnityEngine::RectTransform_set_sizeDelta(rt, m_size, nullptr);
        UnityEngine::RectTransform_set_anchoredPosition(rt, m_position, nullptr);
    }

    m_gameObject = go;

    // 5. Child Text (separate GO - one Graphic per GO rule)
    const char* labelText = m_enabled ? m_nameOn : m_nameOff;
    m_label = CreateTextChild(go, labelText, 15, (int)TextAnchor::MiddleCenter, COLOR_TEXT);

    // Set initial visual state
    UpdateVisuals();
}

void Toggle::OnTouch(int phase, Vec2 localPos) {
    if (phase == (int)TouchPhase::Began) {
        m_enabled = !m_enabled;
        UpdateVisuals();
    }
}

void Toggle::AssignFont(void* font) {
    if (m_label && font && UnityEngine::Text_set_font) {
        UnityEngine::Text_set_font(m_label, font, nullptr);
    }
}

void Toggle::UpdateVisuals() {
    if (m_bgImage) {
        Color c = m_enabled ? COLOR_ON : COLOR_OFF;
        UnityEngine::Graphic_set_color(reinterpret_cast<Graphic*>(m_bgImage), c, nullptr);
    }

    if (m_label) {
        const char* text = m_enabled ? m_nameOn : m_nameOff;
        UnityEngine::Text_set_text(m_label, UnityEngine::il2cpp_string_new(text), nullptr);
    }
}

} // namespace renderer::ui::widgets
