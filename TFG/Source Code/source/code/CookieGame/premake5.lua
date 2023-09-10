project "CookieGame"
    kind "ConsoleApp"
    files {
        "**.cpp",
        "**.h"
    }
    links {
        "opengl32.lib",
        "GLFW"
    }

    UseCookieEngine();

    postbuildcommands {
        "{COPYDIR} %{wks.location}/source/resources/** %{cfg.targetdir}"
    }