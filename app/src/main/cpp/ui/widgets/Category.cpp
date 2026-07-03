// =============================================================================
// ui/widgets/Category.cpp - Section header widget implementation
// =============================================================================

#include "Category.hpp"
#include "../../util/Log.hpp"

namespace renderer::ui::widgets {

static constexpr Color COLOR_CATEGORY = {0.4f, 0.8f, 1.0f, 1.0f};

Category::Category(const char* name)
    : m_name(name) {
    m_size = {350, 30};
}

void Category::Create(void* parent) {
    auto& resolver = Il2CppResolver::Get();

    // ---- Container: NewGO → Parent → AddImage(transparent) → ConfigureRT ----
    auto* go = reinterpret_cast<GameObject*>(
        resolver.NewObject(IMG_CORE, "UnityEngine", "GameObject"));
    if (!go) return;

    auto* str = UnityEngine::il2cpp_string_new("Category");
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

    // AddImage (transparent - just for RectTransform creation)
    void* imgType = UnityEngine::GetType(UnityEngine::klass_Image);
    if (imgType && UnityEngine::il2cpp_runtime_invoke && UnityEngine::methodInfo_AddComponent) {
        void* args[] = { imgType };
        void* exc = nullptr;
        auto* img = reinterpret_cast<Image*>(
            UnityEngine::il2cpp_runtime_invoke(UnityEngine::methodInfo_AddComponent, go, args, &exc));
        if (img) {
            // Transparent background
            UnityEngine::Graphic_set_color(reinterpret_cast<Graphic*>(img), {0, 0, 0, 0}, nullptr);
            if (UnityEngine::Graphic_set_raycastTarget)
                UnityEngine::Graphic_set_raycastTarget(reinterpret_cast<Graphic*>(img), false, nullptr);
        }
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

    // ---- Label Text (child GO, colored) ----
    m_label = CreateTextChild(go, m_name, 16, (int)TextAnchor::MiddleLeft, COLOR_CATEGORY);
}

void Category::AssignFont(void* font) {
    if (m_label && font && UnityEngine::Text_set_font) {
        UnityEngine::Text_set_font(m_label, font, nullptr);
    }
}

} // namespace renderer::ui::widgets
