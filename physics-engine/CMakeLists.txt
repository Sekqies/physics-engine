add_executable (physics-engine "physics-engine.cpp" "physics-engine.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET physics-engine PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(YourExecutableName PRIVATE 
    glfw      
    glm::glm    
    OpenGL::GL
)