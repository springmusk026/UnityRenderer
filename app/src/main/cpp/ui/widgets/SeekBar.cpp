// =============================================================================
// ui/widgets/SeekBar.cpp - Slider widget implementation
// =============================================================================

#include "SeekBar.hpp"
#include "../../util/Log.hpp"
#include <cstdio>

namespace renderer::ui::widgets {

static constexpr Color COLOR_BG       = {0.12f, 0.12f, 0.18f, 1.0f};
static constexpr Color COLOR_TRACK    = {0.15f, 0.15f, 0.25f, 1.0f};
static constexpr Color COLOR_FILL     = {0.3f, 0.6f, 1.0f, 1.0f};
static constexpr Color COLOR_TEXT     = {1.0f, 1.0f, 1.0f, 1.0f};

SeekBar::SeekBar(const char* name, int minVal, int maxVal, int currentVal, int id)
    : m_id(id), m_name(name), m_min(minVal), m_max(maxVal), m_current(currentVal) {
    m_size = {350, 60};
}

void SeekBar::Create(void* parent) {
    auto& resolver = Il2CppResolver::Get();

    // ---- Container: NewGO → Parent → AddImage → ConfigureRT ----
    auto* go = reinterpret_cast<GameObject*>(
        resolver.NewObject(IMG_CORE, "UnityEngine", "GameObject"));
    if (!go) return;

    auto* str = UnityEngine::il2cpp_string_new("SeekBar");
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

    // AddImage to container
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

    // ---- Label Text (top-left area) ----
    m_label = CreateTextChild(go, m_name, 14, (int)TextAnchor::MiddleLeft, COLOR_TEXT);

    // ---- Value Text (top-right area) ----
    char valStr[32];
    snprintf(valStr, sizeof(valStr), "%d", m_current);
    m_valueText = CreateTextChild(go, valStr, 14, (int)TextAnchor::MiddleRight, COLOR_FILL);

    // ---- Track (bottom area - background bar) ----
    // Track is an Image child GO
    auto* trackGO = reinterpret_cast<GameObject*>(
        resolver.NewObject(IMG_CORE, "UnityEngine", "GameObject"));
    if (trackGO) {
        auto* tstr = UnityEngine::il2cpp_string_new("Track");
        if (UnityEngine::il2cpp_runtime_invoke && UnityEngine::methodInfo_GameObject_ctor) {
            void* args[] = { tstr };
            void* exc = nullptr;
            UnityEngine::il2cpp_runtime_invoke(UnityEngine::methodInfo_GameObject_ctor, trackGO, args, &exc);
        } else if (UnityEngine::GameObject_ctor) {
            UnityEngine::GameObject_ctor(trackGO, tstr, nullptr);
        }

        auto* tChildT = UnityEngine::GameObject_get_transform(trackGO, nullptr);
        auto* tParentT = UnityEngine::GameObject_get_transform(go, nullptr);
        if (tChildT && tParentT)
            UnityEngine::Transform_SetParent(tChildT, tParentT, false, nullptr);

        // Add Image to track
        if (imgType && UnityEngine::il2cpp_runtime_invoke && UnityEngine::methodInfo_AddComponent) {
            void* args[] = { imgType };
            void* exc = nullptr;
            auto* trackImg = reinterpret_cast<Image*>(
                UnityEngine::il2cpp_runtime_invoke(UnityEngine::methodInfo_AddComponent, trackGO, args, &exc));
            if (trackImg) {
                UnityEngine::Graphic_set_color(reinterpret_cast<Graphic*>(trackImg), COLOR_TRACK, nullptr);
                if (UnityEngine::Graphic_set_raycastTarget)
                    UnityEngine::Graphic_set_raycastTarget(reinterpret_cast<Graphic*>(trackImg), false, nullptr);
            }
        }

        // Track RT: bottom portion of container
        auto* trt = reinterpret_cast<RectTransform*>(
            UnityEngine::GameObject_get_transform(trackGO, nullptr));
        if (trt) {
            UnityEngine::RectTransform_set_anchorMin(trt, {0.03f, 0.1f}, nullptr);
            UnityEngine::RectTransform_set_anchorMax(trt, {0.97f, 0.45f}, nullptr);
            UnityEngine::RectTransform_set_offsetMin(trt, {0, 0}, nullptr);
            UnityEngine::RectTransform_set_offsetMax(trt, {0, 0}, nullptr);
        }
    }

    // ---- Fill (colored portion of the bar) ----
    auto* fillGO = reinterpret_cast<GameObject*>(
        resolver.NewObject(IMG_CORE, "UnityEngine", "GameObject"));
    if (fillGO) {
        auto* fstr = UnityEngine::il2cpp_string_new("Fill");
        if (UnityEngine::il2cpp_runtime_invoke && UnityEngine::methodInfo_GameObject_ctor) {
            void* args[] = { fstr };
            void* exc = nullptr;
            UnityEngine::il2cpp_runtime_invoke(UnityEngine::methodInfo_GameObject_ctor, fillGO, args, &exc);
        } else if (UnityEngine::GameObject_ctor) {
            UnityEngine::GameObject_ctor(fillGO, fstr, nullptr);
        }

        auto* fChildT = UnityEngine::GameObject_get_transform(fillGO, nullptr);
        auto* fParentT = UnityEngine::GameObject_get_transform(go, nullptr);
        if (fChildT && fParentT)
            UnityEngine::Transform_SetParent(fChildT, fParentT, false, nullptr);

        // Add Image to fill
        if (imgType && UnityEngine::il2cpp_runtime_invoke && UnityEngine::methodInfo_AddComponent) {
            void* args[] = { imgType };
            void* exc = nullptr;
            m_fillImage = reinterpret_cast<Image*>(
                UnityEngine::il2cpp_runtime_invoke(UnityEngine::methodInfo_AddComponent, fillGO, args, &exc));
            if (m_fillImage) {
                UnityEngine::Graphic_set_color(reinterpret_cast<Graphic*>(m_fillImage), COLOR_FILL, nullptr);
                if (UnityEngine::Graphic_set_raycastTarget)
                    UnityEngine::Graphic_set_raycastTarget(reinterpret_cast<Graphic*>(m_fillImage), false, nullptr);
            }
        }

        m_fillGO = fillGO;
        UpdateFill();
    }
}

void SeekBar::OnTouch(int phase, Vec2 localPos) {
    if (phase == (int)TouchPhase::Began) {
        m_dragging = true;
    }

    if (m_dragging && (phase == (int)TouchPhase::Began || phase == (int)TouchPhase::Moved)) {
        // Calculate value from X position relative to widget width
        float pct = localPos.x / m_size.x;
        if (pct < 0.0f) pct = 0.0f;
        if (pct > 1.0f) pct = 1.0f;

        m_current = m_min + (int)(pct * (float)(m_max - m_min));
        UpdateFill();
        UpdateValueText();
    }

    if (phase == (int)TouchPhase::Ended || phase == (int)TouchPhase::Canceled) {
        m_dragging = false;
    }
}

void SeekBar::AssignFont(void* font) {
    if (font && UnityEngine::Text_set_font) {
        if (m_label) UnityEngine::Text_set_font(m_label, font, nullptr);
        if (m_valueText) UnityEngine::Text_set_font(m_valueText, font, nullptr);
    }
}

void SeekBar::UpdateFill() {
    if (!m_fillGO) return;

    float fillPct = (m_max > m_min)
        ? (float)(m_current - m_min) / (float)(m_max - m_min)
        : 0.0f;
    if (fillPct < 0.0f) fillPct = 0.0f;
    if (fillPct > 1.0f) fillPct = 1.0f;

    auto* frt = reinterpret_cast<RectTransform*>(
        UnityEngine::GameObject_get_transform(m_fillGO, nullptr));
    if (frt) {
        UnityEngine::RectTransform_set_anchorMin(frt, {0.03f, 0.1f}, nullptr);
        UnityEngine::RectTransform_set_anchorMax(frt, {0.03f + 0.94f * fillPct, 0.45f}, nullptr);
        UnityEngine::RectTransform_set_offsetMin(frt, {0, 0}, nullptr);
        UnityEngine::RectTransform_set_offsetMax(frt, {0, 0}, nullptr);
    }
}

void SeekBar::UpdateValueText() {
    if (!m_valueText) return;
    char valStr[32];
    snprintf(valStr, sizeof(valStr), "%d", m_current);
    UnityEngine::Text_set_text(m_valueText, UnityEngine::il2cpp_string_new(valStr), nullptr);
}

} // namespace renderer::ui::widgets
