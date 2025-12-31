set(SOURCE_FILES 
    "physics-engine.cpp"
    "physics-engine.h"
)

add_executable(physics-engine ${SOURCE_FILES})
find_package(OpenGL REQUIRED)

target_link_libraries(physics-engine PRIVATE 
    glad
    glfw        
    glm::glm    
    OpenGL::GL
)

target_include_directories(physics-engine PRIVATE ".")