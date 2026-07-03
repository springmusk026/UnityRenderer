#pragma once

// =============================================================================
// ui/Widget.hpp - Abstract base class for all UI widgets
//
// Provides common infrastructure: position/size management, visibility,
// collapse groups, and static helpers for creating UI GameObjects following
// the correct IL2CPP component ordering rules.
// =============================================================================

#include "../core/Types.hpp"
#include "../core/UnityEngine.hpp"
#include <vector>

namespace renderer::ui {

using namespace renderer::core;

class Widget {
public:
    virtual ~Widget();

    // Create the widget's GameObjects under the given parent.
    virtual void Create(void* parent) = 0;

    // Handle touch input. phase = TouchPhase, localPos relative to widget.
    virtual void OnTouch(int phase, Vec2 localPos) {}

    // Assign a resolved Font* to any Text components owned by this widget.
    virtual void AssignFont(void* font) {}

    // Show or hide the widget's root GameObject.
    void SetVisible(bool visible);

    // Getters
    Vec2 GetPosition() const { return m_position; }
    void SetPosition(Vec2 pos) { m_position = pos; }
    Vec2 GetSize() const { return m_size; }
    GameObject* GetGameObject() const { return m_gameObject; }
    int GetCollapseGroup() const { return m_collapseGroup; }
    bool IsVisible() const { return m_visible; }

    // =========================================================================
    // Static helpers - follow the mandatory build order:
    //   NewGO → SetParent → AddImage → ConfigureRectTransform
    // =========================================================================

    // Create a container GO with Image (for RectTransform) and configure size/position.
    // Returns the created GameObject, or nullptr on failure.
    static GameObject* CreateContainerGO(const char* name, void* parent,
                                         Vec2 size, Vec2 position);

    // Create a child GO with only a Text component (no Image - one Graphic per GO rule).
    // Returns the Text* component. Registers it in the global text list for font assignment.
    static Text* CreateTextChild(void* parent, const char* text, int fontSize,
                                 int alignment, Color color);

    // Global registry of all Text* for deferred font assignment
    static inline std::vector<Text*> s_allTexts;

protected:
    GameObject* m_gameObject = nullptr;
    Vec2 m_position = {0, 0};
    Vec2 m_size = {0, 0};
    bool m_visible = true;
    int m_collapseGroup = -1;
};

} // namespace renderer::ui
