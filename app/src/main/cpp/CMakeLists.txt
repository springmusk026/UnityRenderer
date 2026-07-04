# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)

# Declares the project name. The project name can be accessed via ${ PROJECT_NAME},
# Since this is the top level CMakeLists.txt, the project name is also accessible
# with ${CMAKE_PROJECT_NAME} (both CMake variables are in-sync within the top level
# build script scope).
project("main")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
#
# In this top level CMakeLists.txt, ${CMAKE_PROJECT_NAME} is used to define
# the target library name; in the sub-module's CMakeLists.txt, ${PROJECT_NAME}
# is preferred for the same purpose.
#
# In order to load a library into your app from Java/Kotlin, you must call
# System.loadLibrary() and pass the name of the library defined here;
# for GameActivity/NativeActivity derived applications, the same library name must be

find_package(dobby REQUIRED CONFIG)

# Include directories
include_directories(prefab/modules/dobby/include)
include_directories(xdl/include)
include_directories(Il2cpp)
include_directories(Unity)
include_directories(core)
include_directories(ui)
include_directories(ui/widgets)
include_directories(input)
include_directories(util)
include_directories(entry)

aux_source_directory(xdl xdl-src)

# used in the AndroidManifest.xml file.
add_library(${CMAKE_PROJECT_NAME} SHARED
        # XDL sources
        ${xdl-src}
        # Legacy IL2CPP layer
        Il2cpp/Il2Cpp.cpp
        # Core layer
        core/Il2CppRuntime.cpp
        core/Il2CppResolver.cpp
        core/UnityEngine.cpp
        # UI layer
        ui/Widget.cpp
        ui/Canvas.cpp
        ui/Menu.cpp
        ui/widgets/Toggle.cpp
        ui/widgets/SeekBar.cpp
        ui/widgets/Checkbox.cpp
        ui/widgets/Category.cpp
        # Input layer
        input/TouchInput.cpp
        # Entry point
        entry/Entry.cpp
)

# Specifies libraries CMake should link to your target library. You
# can link libraries from various origins, such as libraries defined in this
# build script, prebuilt third-party libraries, or Android system libraries.
target_link_libraries(${CMAKE_PROJECT_NAME}
        # List libraries link to the target library
        android
        log
        dobby::dobby
)
