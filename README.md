# UnityRenderer - Native Unity UI Framework

> Educational project demonstrating IL2CPP runtime interaction from native C++ on Android

[![Build](https://github.com/springmusk026/UnityRenderer/actions/workflows/build.yml/badge.svg)](https://github.com/springmusk026/UnityRenderer/actions/workflows/build.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

## ⚠️ Project Status: Early Stage

This project is in **very early development**. It is raw, minimally tested, and **will have bugs**. It has been tested on a limited number of devices and Unity versions.

If you encounter issues:
- This is open source — **fix it and submit a PR** instead of complaining
- Check the [issues](https://github.com/springmusk026/UnityRenderer/issues) page first
- Provide full logcat output when reporting crashes
- Understand that IL2CPP internals vary between Unity versions — what works on one game may not work on another

**No guarantees. No support promises. Contributions welcome.**

---

## Overview

UnityRenderer is a research framework that demonstrates how to create and render Unity UI elements entirely from native C++ on Android. It interacts with Unity's IL2CPP runtime to instantiate GameObjects, add UI components (Canvas, Image, Text, Button), handle touch input, and manage a widget-based overlay panel — all without writing any C# code.

## Features

- **Hook-free UI creation** — Uses `il2cpp_thread_attach` + `il2cpp_runtime_invoke` for safe object creation from any thread
- **Modular widget system** — Toggle, SeekBar, Checkbox, Category headers with extensible base class
- **Touch input handling** — Screen-to-canvas coordinate conversion, hit testing, drag-to-move
- **OOP architecture** — Clean separation: `core/`, `ui/`, `input/`, `entry/` layers
- **RAII memory management** — `unique_ptr` widget ownership, no raw new/delete
- **Auto-layout engine** — Widgets auto-position vertically with configurable spacing
- **Deferred font assignment** — Fonts created on render thread, assigned to all text components
- **Minimal footprint** — Single Dobby hook for input polling only

## Architecture

```
app/src/main/cpp/
├── entry/
│   └── Entry.cpp               ← JNI entry point, init orchestration
├── core/
│   ├── Types.hpp               ← IL2CPP/Unity struct definitions + enums
│   ├── Il2CppRuntime.hpp/cpp   ← IL2CPP lifecycle (library wait, attach, assemblies)
│   ├── Il2CppResolver.hpp/cpp  ← Method/class resolution with caching
│   └── UnityEngine.hpp/cpp     ← Typed function pointers for Unity API
├── ui/
│   ├── Canvas.hpp/cpp          ← Canvas + CanvasScaler + Raycaster
│   ├── Widget.hpp/cpp          ← Abstract base class for all widgets
│   ├── Menu.hpp/cpp            ← Menu controller (layout, drag, visibility)
│   └── widgets/
│       ├── Toggle.hpp/cpp      ← On/off switch with visual feedback
│       ├── SeekBar.hpp/cpp     ← Horizontal slider with fill bar
│       ├── Checkbox.hpp/cpp    ← Checkmark toggle
│       └── Category.hpp/cpp    ← Non-interactive section header
├── input/
│   └── TouchInput.hpp/cpp      ← Touch polling + coordinate conversion
├── util/
│   ├── Log.hpp                 ← Android logcat macros
│   └── Obfuscate.hpp           ← Compile-time string obfuscation
├── Il2cpp/                     ← IL2CPP helper library (xDL-based)
└── xdl/                        ← Enhanced dlopen/dlsym (HexHacking)
```

## How It Works

1. **Library Loading** — `dlopen("libunity.so")` loads the Unity engine into the process
2. **IL2CPP Bootstrap** — Waits for `libil2cpp.so` to appear in `/proc/self/maps`, then resolves all IL2CPP API exports via xDL
3. **Thread Attachment** — `il2cpp_thread_attach(domain)` makes our native thread valid for IL2CPP calls
4. **API Resolution** — Resolves ~40 Unity method pointers (GameObject, Canvas, Image, Text, etc.) from IL2CPP metadata
5. **UI Construction** — Creates Canvas → Panel → Widgets hierarchy using `il2cpp_runtime_invoke`
6. **Input Hook** — Minimal Dobby hook on `Canvas.SendWillRenderCanvases` for per-frame touch polling and font assignment
7. **Touch Loop** — Converts screen coordinates to canvas space, hit-tests widgets, handles drag

## Building

### Prerequisites

- Android Studio Arctic Fox or later
- Android NDK 25+ (installed via SDK Manager)
- CMake 3.22.1+
- JDK 11+

### Build Steps

```bash
git clone https://github.com/springmusk026/UnityRenderer.git
cd UnityRenderer
./gradlew assembleDebug
```

The output APK will be at `app/build/outputs/apk/debug/app-debug.apk`

## Adding Custom Widgets

1. Create `ui/widgets/YourWidget.hpp` inheriting from `Widget`
2. Implement `Create(void* parent)` — build your GameObjects
3. Implement `OnTouch(int phase, Vec2 localPos)` — handle interaction
4. Implement `AssignFont(void* font)` — set font on your Text components
5. Add factory method in `Menu.hpp/cpp`
6. Call it in `Entry.cpp` before `Build()`

```cpp
// In Entry.cpp:
menu.AddToggle("Feature [ON]", "Feature [OFF]", false, 1);
menu.AddSeekBar("Value", 0, 100, 50, 2);
menu.AddCheckbox("Option", false, 3);
```

## Key Technical Insights

| Challenge | Solution |
|-----------|----------|
| UI creation requires render thread | `il2cpp_thread_attach` makes any thread valid for IL2CPP |
| `AddComponent<Image>` crashes without RectTransform | Always add Image BEFORE calling `get_transform` |
| Text + Image can't be on same GameObject | Unity allows only one `Graphic` per GO — use child GOs |
| Font creation needs GPU context | Defer font assignment to render thread via hook |
| `Input.GetTouch` is thread-locked | Poll from `SendWillRenderCanvases` hook (render thread) |
| Assembly image names vary by Unity version | Enumerate images at runtime via `il2cpp_domain_get_assemblies` |

## Dependencies

| Library | Purpose | License |
|---------|---------|---------|
| [xDL](https://github.com/hexhacking/xDL) | Enhanced dlopen/dlsym for Android | MIT |
| [Dobby](https://github.com/jmpews/Dobby) | Lightweight function hooking | Apache 2.0 |

## Disclaimer

This project is for **educational and research purposes only**. It demonstrates IL2CPP runtime internals, Unity's component architecture, and native-managed code interaction patterns. Use responsibly and in compliance with applicable terms of service.

## License

MIT License — see [LICENSE](LICENSE) for details.
