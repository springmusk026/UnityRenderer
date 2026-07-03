#pragma once

// =============================================================================
// ui/widgets/Category.hpp - Section header (non-interactive)
//
// Layout: container(Image transparent) + label(Text colored)
// =============================================================================

#include "../Widget.hpp"

namespace renderer::ui::widgets {

class Category : public Widget {
public:
    explicit Category(const char* name);
    ~Category() override = default;

    void Create(void* parent) override;
    void OnTouch(int phase, Vec2 localPos) override {} // Non-interactive
    void AssignFont(void* font) override;

private:
    const char* m_name = nullptr;
    Text* m_label = nullptr;
};

} // namespace renderer::ui::widgets
