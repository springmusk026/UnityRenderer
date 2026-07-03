# Contributing to UnityRenderer

Thank you for your interest in contributing! This project is an educational framework for understanding Unity IL2CPP runtime interaction from native C++.

## Code Style

- **C++17** standard
- **Namespaces**: `renderer::core`, `renderer::ui`, `renderer::input`
- **RAII**: Use `std::unique_ptr` for ownership, no raw `new`/`delete`
- **Headers**: `.hpp` for C++ headers, `.h` for C-compatible headers
- **Naming**: `PascalCase` for classes/methods, `m_camelCase` for members, `UPPER_CASE` for constants
- **Include guards**: `#pragma once`

## Pull Request Process

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/my-widget`)
3. Follow the existing architecture (see `ARCHITECTURE.md`)
4. Ensure the project builds: `./gradlew assembleDebug`
5. Write clear commit messages
6. Open a PR against `main`

## Adding Widgets

New widget contributions are welcome! Follow the pattern in `ui/widgets/`:
1. Inherit from `Widget`
2. Implement `Create()`, `OnTouch()`, `AssignFont()`
3. Add a factory method to `Menu`
4. Document in README

## What We Accept

- ✅ New widget types (sliders, dropdowns, color pickers)
- ✅ Performance optimizations
- ✅ Bug fixes and stability improvements
- ✅ Documentation improvements
- ✅ Multi-Unity-version compatibility fixes
- ✅ Build system improvements

## What We Don't Accept

- ❌ Code intended to violate terms of service of any application
- ❌ Anti-detection or obfuscation mechanisms
- ❌ Application-specific targeting or automation
- ❌ Contributions without clear educational purpose

## Reporting Issues

Use the issue templates provided. Include:
- Android device/emulator info
- Unity version of the target application
- Full logcat output around the crash/issue
- Steps to reproduce
