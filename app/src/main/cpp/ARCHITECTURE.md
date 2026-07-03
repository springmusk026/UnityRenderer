// =============================================================================
// ARCHITECTURE.md - UnityRenderer OOP Refactor Design
// =============================================================================
//
// Directory Structure:
// app/src/main/cpp/
// ├── CMakeLists.txt
// ├── entry/
// │   └── Entry.cpp              - JNI_OnLoad, native method registration, init orchestration
// ├── core/
// │   ├── Il2CppRuntime.hpp/cpp  - IL2CPP lifecycle (attach, domain, thread)
// │   ├── Il2CppResolver.hpp/cpp - Method/class resolution with LRU caching
// │   ├── UnityEngine.hpp/cpp    - Unity engine bindings (typed function pointers)
// │   └── Types.hpp              - All IL2CPP/Unity struct definitions and enums
// ├── ui/
// │   ├── Canvas.hpp/cpp         - Canvas + CanvasScaler + Raycaster setup
// │   ├── Widget.hpp/cpp         - Abstract base class for all UI widgets
// │   ├── Menu.hpp/cpp           - Menu controller (layout engine, visibility, drag)
// │   └── widgets/
// │       ├── Toggle.hpp/cpp     - Toggle button widget
// │       ├── SeekBar.hpp/cpp    - Slider widget
// │       ├── Checkbox.hpp/cpp   - Checkbox widget
// │       ├── Category.hpp/cpp   - Section header (non-interactive)
// │       ├── Spinner.hpp/cpp    - Dropdown cycle selector
// │       └── InputValue.hpp/cpp - Numeric +/- input
// ├── input/
// │   └── TouchInput.hpp/cpp     - Touch polling, coordinate conversion, hit testing
// ├── util/
// │   ├── Log.hpp                - Logging macros
// │   └── Obfuscate.hpp          - String obfuscation
// └── xdl/                       - (existing, untouched)
//     └── ...
//
// =============================================================================
// CLASS RESPONSIBILITIES
// =============================================================================
//
// core::Il2CppRuntime (Singleton)
//   - Wait for libil2cpp.so to load
//   - Call il2cpp_thread_attach
//   - Provide domain/thread handles
//   - Check IsAssembliesLoaded
//
// core::Il2CppResolver (Singleton)
//   - Resolve classes by (image, namespace, class)
//   - Resolve methods by (image, namespace, class, method, argc)
//   - Cache all resolved pointers in std::unordered_map
//   - Provide GetType() helper
//   - Provide il2cpp_runtime_invoke wrapper
//
// core::UnityEngine (Static)
//   - Typed function pointers for all Unity methods
//   - Init() resolves all pointers via Il2CppResolver
//   - Provides: NewGameObject(), AddComponent(), SetParent(), etc.
//   - Thread-safe: creation from any attached thread
//
// core::Types
//   - All struct definitions (GameObject, Canvas, Text, Image, etc.)
//   - All enums (RenderMode, TextAnchor, etc.)
//   - Il2CppVector2, Il2CppColor, etc.
//
// ui::Canvas
//   - Create/destroy the Canvas + CanvasScaler + GraphicRaycaster
//   - Manages the root GameObject
//   - DontDestroyOnLoad
//
// ui::Widget (Abstract Base)
//   - virtual void Create(GameObject* parent) = 0
//   - virtual void OnTouch(TouchPhase, Vec2 localPos) {}
//   - virtual void AssignFont(void* font) {}
//   - virtual void SetVisible(bool) 
//   - Stores: gameObject, position, size, collapseGroup
//   - RAII: destructor hides/destroys GO
//
// ui::widgets::Toggle : Widget
//   - Image background + Text label child
//   - Toggle state, color change on tap
//
// ui::widgets::SeekBar : Widget
//   - Track + fill + label + value text
//   - Drag handling
//
// ui::widgets::Checkbox : Widget
//   - [X]/[ ] indicator + label
//
// ui::widgets::Category : Widget
//   - Transparent container + colored text
//   - Non-interactive
//
// ui::Menu (Singleton)
//   - Owns Canvas
//   - Owns vector<unique_ptr<Widget>>
//   - Layout engine: auto-positions widgets vertically
//   - AddToggle(), AddSeekBar(), AddCheckbox(), etc.
//   - Show(), Hide(), Toggle()
//   - Update(TouchInput&) - delegates to widgets
//   - AssignFonts(void* font) - delegates to widgets
//   - Drag handling for the panel
//
// input::TouchInput
//   - Reads Input.GetTouch via IL2CPP
//   - Converts screen coords → canvas coords
//   - Provides: GetTouchCount(), GetTouch(int), ScreenToCanvas()
//   - Must be called from render thread
//
// entry::Entry
//   - JNI_OnLoad: register natives
//   - load(): dlopen libunity.so, start init thread
//   - initThread(): Il2CppRuntime::Init() → UnityEngine::Init() → Menu::Build()
//   - Hook: Canvas::SendWillRenderCanvases for font + input
//
// =============================================================================
// MEMORY MANAGEMENT RULES
// =============================================================================
//
// 1. All Widget instances owned by Menu via std::unique_ptr<Widget>
// 2. No raw new/delete - use make_unique
// 3. IL2CPP objects (GameObject*, Text*, etc.) are GC-managed - we hold raw
//    pointers but never free them (IL2CPP GC handles it)
// 4. std::string for all dynamic strings (options lists, etc.)
// 5. No global mutable state except singletons (Il2CppRuntime, Menu)
// 6. Thread safety: menu creation on attached thread, input on render thread
//    Protected by atomic<bool> flags, no mutexes needed (single-writer patterns)
//
// =============================================================================
// BUILD ORDER RULE
// =============================================================================
//
// When creating UI elements under a Canvas:
// 1. NewGameObject()
// 2. SetParent(child, parent)  
// 3. AddComponent<Image>()     ← MUST be first component (creates RectTransform)
// 4. Configure RectTransform
// 5. Create child GO for Text (Text goes on SEPARATE GO from Image)
//
// Never: AddComponent<Text> and AddComponent<Image> on same GO
// Never: Call GetOrAddRectTransform before AddComponent<Image>
//
