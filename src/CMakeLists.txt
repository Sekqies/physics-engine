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
    learnopengl
)
add_custom_target(TARGET physics-engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
    "${CMAKE_CURRENT_BINARY_DIR}/shaders" 
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "${CMAKE_CURRENT_BINARY_DIR}/assets"
    COMMENT "Copying additional folders to build directory..."
)

target_include_directories(physics-engine PRIVATE ".")