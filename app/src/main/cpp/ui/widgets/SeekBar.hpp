#pragma once

// =============================================================================
// ui/widgets/SeekBar.hpp - Slider widget
//
// Layout: container(Image) + label(Text) + value(Text) + track(Image) + fill(Image)
// Drag handling updates fill width and value display.
// =============================================================================

#include "../Widget.hpp"

namespace renderer::ui::widgets {

class SeekBar : public Widget {
public:
    SeekBar(const char* name, int minVal, int maxVal, int currentVal, int id = -1);
    ~SeekBar() override = default;

    void Create(void* parent) override;
    void OnTouch(int phase, Vec2 localPos) override;
    void AssignFont(void* font) override;

    int GetValue() const { return m_current; }
    int GetId() const { return m_id; }
    bool IsDragging() const { return m_dragging; }

private:
    void UpdateFill();
    void UpdateValueText();

    int m_id = -1;
    const char* m_name = nullptr;
    int m_min = 0;
    int m_max = 100;
    int m_current = 50;
    bool m_dragging = false;

    Image* m_bgImage = nullptr;
    Text* m_label = nullptr;
    Text* m_valueText = nullptr;
    GameObject* m_fillGO = nullptr;
    Image* m_fillImage = nullptr;
};

} // namespace renderer::ui::widgets
