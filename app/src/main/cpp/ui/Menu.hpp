#pragma once

// =============================================================================
// ui/Menu.hpp - Menu controller (Singleton)
//
// Owns the Canvas, panel GO, and all widgets. Provides builder methods and
// handles layout, visibility, drag, and font assignment.
// =============================================================================

#include "Canvas.hpp"
#include "Widget.hpp"
#include "widgets/Toggle.hpp"
#include "widgets/SeekBar.hpp"
#include "widgets/Checkbox.hpp"
#include "widgets/Category.hpp"

#include <memory>
#include <vector>

namespace renderer::ui {

class Menu {
public:
    static Menu& Get();

    // Build the entire menu: canvas + panel + title bar + widgets
    void Build();

    // Add widgets (call before Build, or they'll be positioned automatically)
    widgets::Toggle* AddToggle(const char* nameOn, const char* nameOff,
                               bool initial = false, int id = -1);
    widgets::SeekBar* AddSeekBar(const char* name, int min, int max,
                                  int current, int id = -1);
    widgets::Checkbox* AddCheckbox(const char* name, bool initial = false, int id = -1);
    widgets::Category* AddCategory(const char* name);

    // Per-frame update: hit test touches and delegate to widgets
    void Update(Vec2 touchPos, int touchPhase, bool touching);

    // Assign font to all widgets (called when font becomes available)
    void AssignFonts(void* font);

    // Visibility
    void Show();
    void Hide();
    void Toggle();
    bool IsVisible() const { return m_visible; }
    bool IsDragging() const { return m_dragging; }

    // Collapse group support
    void SetCollapseGroupVisible(int group, bool visible);

    Menu(const Menu&) = delete;
    Menu& operator=(const Menu&) = delete;

private:
    Menu() = default;

    void CreatePanel();
    void LayoutWidgets();
    bool HitTest(Vec2 screenPos, Vec2 objPos, Vec2 objSize) const;

    std::unique_ptr<Canvas> m_canvas;
    std::vector<std::unique_ptr<Widget>> m_widgets;

    GameObject* m_panelGO = nullptr;
    Text* m_titleText = nullptr;

    // Layout state
    float m_currentY = 0.0f;
    static constexpr float WIDGET_SPACING = 5.0f;
    static constexpr float TITLE_HEIGHT = 40.0f;
    static constexpr float PANEL_WIDTH = 370.0f;
    static constexpr float PANEL_PADDING = 10.0f;

    // Panel position (for dragging)
    Vec2 m_panelPos = {0, 0};
    Vec2 m_panelSize = {PANEL_WIDTH, 400};

    // Visibility & drag state
    bool m_visible = true;
    bool m_dragging = false;
    Vec2 m_dragOffset = {0, 0};
    bool m_built = false;
};

} // namespace renderer::ui
