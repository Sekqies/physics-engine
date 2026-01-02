include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw
    GIT_TAG 3.3.8
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm
    GIT_TAG 1.0.3
)
FetchContent_MakeAvailable(glm)

add_library(glad STATIC 
    glad/src/glad.c)
add_library(learnopengl
    "learnopengl/src/shader.cpp"
    "learnopengl/include/shader/shader.h"
    "learnopengl/include/stb_image/stb_image.h" 
    "learnopengl/src/stb_image.cpp")
target_include_directories(glad PUBLIC 
    glad/include 
)
target_include_directories(glad PUBLIC glad/include)
target_include_directories(learnopengl PUBLIC learnopengl/include)
target_link_libraries(learnopengl PUBLIC glad)

find_package(OpenGL REQUIRED)