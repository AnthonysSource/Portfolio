project "GLFW"
    kind "StaticLib"
    files {
        -- Common Headers
        "src/glfw_config.h",
        "include/GLFW/glfw3.h",
        "include/GLFW/glfw3native.h",
        -- Common Sources
        "src/context.c",
        "src/init.c",
        "src/input.c",
        "src/monitor.c",
        "src/vulkan.c",
        "src/window.c",
        -- Win32 Headers
        "src/win32_platform.h",
        "src/win32_joystick.h",
        "src/wgl_context.h",
        "src/egl_context.h",
        "src/osmesa_context.h",
        -- Win32 Sources
        "src/win32_init.c",
        "src/win32_joystick.c",
        "src/win32_monitor.c",
        "src/win32_time.c",
        "src/win32_thread.c",
        "src/win32_window.c",
        "src/wgl_context.c",
        "src/egl_context.c",
        "src/osmesa_context.c",
    }
    includedirs {
        "include"
    }
    defines {
        "_GLFW_WIN32",
        "_CRT_SECURE_NO_WARNINGS" -- MSVC Specific define
    }
    removedefines "GLFW_INCLUDE_NONE"