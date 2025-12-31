add_subdirectory(thirdparty/spdlog)
add_subdirectory(thirdparty/glfw)
add_subdirectory(thirdparty/glad)
add_subdirectory(thirdparty/imgui)

add_library(Kyber STATIC
    src/Kyber/Kyber.hpp

    src/Kyber/Core/EntryPoint.cpp

    src/Kyber/Core/Types.hpp
    src/Kyber/Core/Application.hpp
    src/Kyber/Core/Application.cpp
    src/Kyber/Core/Logger.hpp
    src/Kyber/Core/Logger.cpp
    src/Kyber/Core/KeyCodes.hpp
    src/Kyber/Core/Events.hpp
    src/Kyber/Core/Window.hpp
    src/Kyber/Core/Window.cpp
    src/Kyber/Core/Input.hpp
    src/Kyber/Core/Input.cpp
    src/Kyber/Core/Layer.hpp
    src/Kyber/Core/LayerStack.hpp
    src/Kyber/Core/LayerStack.cpp

    src/Kyber/ImGui/ImGuiLayer.hpp
    src/Kyber/ImGui/ImGuiLayer.cpp
    
    src/Platform/ImGui/ImGuiImpl.cpp
)

target_include_directories(Kyber
PUBLIC
    src
PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/generated
)

target_link_libraries(Kyber
PUBLIC
    spdlog
    glfw
    glad
    imgui
)

target_precompile_headers(Kyber
PUBLIC
    src/PCH.hpp
)

target_compile_definitions(Kyber
PUBLIC
    NOMINMAX
    _CRT_SECURE_NO_WARNINGS
    GLFW_INCLUDE_NONE
)

if(WIN32)
    target_compile_definitions(Kyber
    PUBLIC
        GLFW_EXPOSE_NATIVE_WIN32
    )
endif()

file(TO_CMAKE_PATH ${CMAKE_CURRENT_BINARY_DIR}/log LOG_FOLDER)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/PathConfig.inl
    ${CMAKE_CURRENT_BINARY_DIR}/generated/PathConfig.inl
    @ONLY
)
