#pragma once

// =============================================================================
// ui/widgets/Checkbox.hpp - Checkbox widget
//
// Layout: container(Image) + checkmark(Text) + label(Text)
// On tap: toggles checked state, updates checkmark text.
// =============================================================================

#include "../Widget.hpp"

namespace renderer::ui::widgets {

class Checkbox : public Widget {
public:
    Checkbox(const char* name, bool initialState = false, int id = -1);
    ~Checkbox() override = default;

    void Create(void* parent) override;
    void OnTouch(int phase, Vec2 localPos) override;
    void AssignFont(void* font) override;

    bool IsChecked() const { return m_checked; }
    int GetId() const { return m_id; }

private:
    void UpdateVisuals();

    bool m_checked = false;
    int m_id = -1;
    const char* m_name = nullptr;
    Image* m_bgImage = nullptr;
    Text* m_checkMark = nullptr;
    Text* m_label = nullptr;
};

} // namespace renderer::ui::widgets
