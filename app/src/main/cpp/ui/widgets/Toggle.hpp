#pragma once

// =============================================================================
// ui/widgets/Toggle.hpp - Toggle button widget
//
// Layout: container(Image) + child(Text)
// On tap: toggles state, changes background color and label text.
// =============================================================================

#include "../Widget.hpp"

namespace renderer::ui::widgets {

class Toggle : public Widget {
public:
    Toggle(const char* nameOn, const char* nameOff, bool initialState = false, int id = -1);
    ~Toggle() override = default;

    void Create(void* parent) override;
    void OnTouch(int phase, Vec2 localPos) override;
    void AssignFont(void* font) override;

    bool IsEnabled() const { return m_enabled; }
    int GetId() const { return m_id; }

private:
    void UpdateVisuals();

    bool m_enabled = false;
    int m_id = -1;
    const char* m_nameOn = nullptr;
    const char* m_nameOff = nullptr;
    Image* m_bgImage = nullptr;
    Text* m_label = nullptr;
};

} // namespace renderer::ui::widgets
