set(SOURCE_FILES 
    "physics-engine.cpp"
    "n_body_simulation.cpp"
)

add_executable(physics-engine ${SOURCE_FILES} "n_body_simulation.cpp")
find_package(OpenGL REQUIRED)

target_link_libraries(physics-engine PRIVATE 
    glad
    glfw        
    glm::glm    
    OpenGL::GL
    learnopengl
)
add_custom_target(copy-additional-folders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
    "${CMAKE_CURRENT_BINARY_DIR}/shaders" 
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "${CMAKE_CURRENT_BINARY_DIR}/assets"
    COMMENT "Copying additional folders to build directory..."
)

add_dependencies(physics-engine copy-additional-folders)
target_include_directories(physics-engine PRIVATE ".")