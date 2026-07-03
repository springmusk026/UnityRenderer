// =============================================================================
// ui/Menu.cpp - Menu controller implementation
// =============================================================================

#include "Menu.hpp"
#include "../util/Log.hpp"
#include "../core/Il2CppResolver.hpp"

namespace renderer::ui {

Menu& Menu::Get() {
    static Menu instance;
    return instance;
}

// =============================================================================
// Build - Create canvas, panel, title, and all widget GameObjects
// =============================================================================
void Menu::Build() {
    if (m_built) return;

    LOGI("Menu::Build: Starting menu construction");

    // Create Canvas
    m_canvas = std::make_unique<Canvas>();
    if (!m_canvas->Create(100, Vec2{1080, 1920})) {
        LOGE("Menu::Build: Canvas creation failed");
        return;
    }

    // Calculate panel height based on registered widgets
    float totalHeight = TITLE_HEIGHT + PANEL_PADDING * 2;
    for (auto& w : m_widgets) {
        totalHeight += w->GetSize().y + WIDGET_SPACING;
    }
    if (totalHeight < 200) totalHeight = 200;
    m_panelSize = {PANEL_WIDTH, totalHeight};

    // Create panel centered
    m_panelPos = {0, 0};
    CreatePanel();

    // Layout and create all widgets
    LayoutWidgets();

    m_built = true;
    LOGI("Menu::Build: Menu built with %zu widgets", m_widgets.size());
}

// =============================================================================
// CreatePanel - Panel GO with background Image under the Canvas
// =============================================================================
void Menu::CreatePanel() {
    auto* canvasGO = m_canvas->GetGameObject();
    if (!canvasGO) return;

    // Create panel GO: NewGO → Parent → AddImage → ConfigureRT
    m_panelGO = Widget::CreateContainerGO("MenuPanel", canvasGO, m_panelSize, m_panelPos);
    if (!m_panelGO) {
        LOGE("Menu::CreatePanel: Failed to create panel GO");
        return;
    }

    // ---- Title bar: a separate container positioned at the top of the panel ----
    float titleY = m_panelSize.y / 2.0f - TITLE_HEIGHT / 2.0f;
    auto* titleBarGO = Widget::CreateContainerGO("TitleBar", m_panelGO,
                                                  {m_panelSize.x, TITLE_HEIGHT}, {0, titleY});
    if (titleBarGO) {
        // Add title text inside the title bar
        m_titleText = Widget::CreateTextChild(titleBarGO, "~ UI Demo ~", 22,
                                              (int)TextAnchor::MiddleCenter,
                                              Color{0.9f, 0.7f, 1.0f, 1.0f});
    }
}

// =============================================================================
// LayoutWidgets - Position and create all widget GameObjects
// =============================================================================
void Menu::LayoutWidgets() {
    if (!m_panelGO) return;

    // Start below the title
    m_currentY = (m_panelSize.y / 2.0f) - TITLE_HEIGHT - PANEL_PADDING;

    for (auto& widget : m_widgets) {
        Vec2 widgetSize = widget->GetSize();

        // Position: centered X, descending Y from top
        float yPos = m_currentY - widgetSize.y / 2.0f;
        // We need to set position before Create. Use a cast to access protected member.
        // Instead, we'll make position part of construction or provide a setter.
        // For now: widgets position is set via their m_position field.

        // Access protected m_position via friend or just keep the Y tracking
        // and pass position during Create. Since Widget::Create takes parent only,
        // we set position after creation by updating the RT.

        // Create the widget under the panel
        widget->Create(m_panelGO);

        // Update its RectTransform position and store it
        Vec2 pos = {0, yPos};
        widget->SetPosition(pos);
        auto* go = widget->GetGameObject();
        if (go) {
            auto* rt = reinterpret_cast<RectTransform*>(
                UnityEngine::GameObject_get_transform(go, nullptr));
            if (rt) {
                UnityEngine::RectTransform_set_anchoredPosition(rt, pos, nullptr);
            }
        }

        m_currentY -= (widgetSize.y + WIDGET_SPACING);
    }

    // Resize panel to fit all widgets
    float actualHeight = TITLE_HEIGHT + PANEL_PADDING * 2 +
        (m_panelSize.y / 2.0f - m_currentY);
    m_panelSize.y = actualHeight > m_panelSize.y ? actualHeight : m_panelSize.y;

    // Update panel RT size
    auto* prt = reinterpret_cast<RectTransform*>(
        UnityEngine::GameObject_get_transform(m_panelGO, nullptr));
    if (prt) {
        UnityEngine::RectTransform_set_sizeDelta(prt, m_panelSize, nullptr);
    }
}

// =============================================================================
// Widget factory methods
// =============================================================================

widgets::Toggle* Menu::AddToggle(const char* nameOn, const char* nameOff,
                                  bool initial, int id) {
    auto toggle = std::make_unique<widgets::Toggle>(nameOn, nameOff, initial, id);
    auto* ptr = toggle.get();
    m_widgets.push_back(std::move(toggle));
    return ptr;
}

widgets::SeekBar* Menu::AddSeekBar(const char* name, int min, int max,
                                    int current, int id) {
    auto seekbar = std::make_unique<widgets::SeekBar>(name, min, max, current, id);
    auto* ptr = seekbar.get();
    m_widgets.push_back(std::move(seekbar));
    return ptr;
}

widgets::Checkbox* Menu::AddCheckbox(const char* name, bool initial, int id) {
    auto checkbox = std::make_unique<widgets::Checkbox>(name, initial, id);
    auto* ptr = checkbox.get();
    m_widgets.push_back(std::move(checkbox));
    return ptr;
}

widgets::Category* Menu::AddCategory(const char* name) {
    auto category = std::make_unique<widgets::Category>(name);
    auto* ptr = category.get();
    m_widgets.push_back(std::move(category));
    return ptr;
}

// =============================================================================
// Update - Process touch input
// =============================================================================
void Menu::Update(Vec2 touchPos, int touchPhase, bool touching) {
    if (!m_visible || !m_built) return;

    if (!touching || touchPhase == (int)TouchPhase::Ended || touchPhase == (int)TouchPhase::Canceled) {
        m_dragging = false;
        return;
    }

    if (touchPhase == (int)TouchPhase::Began) {
        // First check if touch is on the panel at all
        if (!HitTest(touchPos, m_panelPos, m_panelSize)) {
            return; // Touch outside panel
        }

        // Check each widget for hit
        bool widgetHit = false;
        for (size_t i = 0; i < m_widgets.size(); i++) {
            auto& widget = m_widgets[i];
            if (!widget->IsVisible()) continue;

            // Widget position in canvas space = panel position + widget's local offset
            Vec2 wPos = {m_panelPos.x + widget->GetPosition().x,
                         m_panelPos.y + widget->GetPosition().y};
            Vec2 wSize = widget->GetSize();

            if (HitTest(touchPos, wPos, wSize)) {
                Vec2 localPos = {touchPos.x - wPos.x, touchPos.y - wPos.y};
                widget->OnTouch(touchPhase, localPos);
                widgetHit = true;
                break;
            }
        }

        // If no widget was hit, start dragging the panel
        if (!widgetHit) {
            m_dragging = true;
            m_dragOffset = {touchPos.x - m_panelPos.x, touchPos.y - m_panelPos.y};
        }
    }
    else if (touchPhase == (int)TouchPhase::Moved || touchPhase == (int)TouchPhase::Stationary) {
        if (m_dragging) {
            m_panelPos = {touchPos.x - m_dragOffset.x, touchPos.y - m_dragOffset.y};

            // Update panel RectTransform position
            if (m_panelGO) {
                auto* prt = reinterpret_cast<RectTransform*>(
                    UnityEngine::GameObject_get_transform(m_panelGO, nullptr));
                if (prt) {
                    UnityEngine::RectTransform_set_anchoredPosition(prt, m_panelPos, nullptr);
                }
            }
        }
    }
}

// =============================================================================
// AssignFonts - Iterate all widgets and assign font
// =============================================================================
void Menu::AssignFonts(void* font) {
    if (!font) return;

    // Assign to title
    if (m_titleText && UnityEngine::Text_set_font) {
        UnityEngine::Text_set_font(m_titleText, font, nullptr);
    }

    // Assign to all widgets
    for (auto& widget : m_widgets) {
        widget->AssignFont(font);
    }

    // Also assign to all globally registered Text components
    for (auto* text : Widget::s_allTexts) {
        if (text && UnityEngine::Text_set_font) {
            UnityEngine::Text_set_font(text, font, nullptr);
        }
    }

    LOGI("Menu::AssignFonts: Assigned font to %zu text components", Widget::s_allTexts.size());
}

// =============================================================================
// Visibility
// =============================================================================

void Menu::Show() {
    m_visible = true;
    if (m_panelGO) {
        UnityEngine::GameObject_SetActive(m_panelGO, true, nullptr);
    }
}

void Menu::Hide() {
    m_visible = false;
    if (m_panelGO) {
        UnityEngine::GameObject_SetActive(m_panelGO, false, nullptr);
    }
}

void Menu::Toggle() {
    if (m_visible) Hide();
    else Show();
}

void Menu::SetCollapseGroupVisible(int group, bool visible) {
    for (auto& widget : m_widgets) {
        if (widget->GetCollapseGroup() == group) {
            widget->SetVisible(visible);
        }
    }
}

// =============================================================================
// HitTest - Simple AABB test (position is center, size is width/height)
// =============================================================================
bool Menu::HitTest(Vec2 screenPos, Vec2 objPos, Vec2 objSize) const {
    float halfW = objSize.x / 2.0f;
    float halfH = objSize.y / 2.0f;

    return screenPos.x >= (objPos.x - halfW) && screenPos.x <= (objPos.x + halfW) &&
           screenPos.y >= (objPos.y - halfH) && screenPos.y <= (objPos.y + halfH);
}

} // namespace renderer::ui
