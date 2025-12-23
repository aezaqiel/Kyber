add_subdirectory(thirdparty/spdlog)
add_subdirectory(thirdparty/glfw)
add_subdirectory(thirdparty/glad)

add_executable(Kyber
    src/Kyber/EntryPoint.cpp

    src/Kyber/Core/Application.cpp
    src/Kyber/Core/Application.hpp
    src/Kyber/Core/Events.hpp
    src/Kyber/Core/JobSystem.cpp
    src/Kyber/Core/JobSystem.hpp
    src/Kyber/Core/KeyCodes.hpp
    src/Kyber/Core/Logger.cpp
    src/Kyber/Core/Logger.hpp
    src/Kyber/Core/Types.hpp
    src/Kyber/Core/Window.cpp
    src/Kyber/Core/Window.hpp

    src/Kyber/DSA/Xoshiro256.hpp
)

target_include_directories(Kyber
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}/generated
)

target_link_libraries(Kyber
PRIVATE
    spdlog
    glfw
    glad
)

target_compile_definitions(Kyber
PRIVATE
    NOMINMAX
    GLFW_INCLUDE_NONE
)

target_precompile_headers(Kyber
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PCH.hpp
)

if(WIN32)
    target_compile_definitions(Kyber
    PRIVATE
        GLFW_EXPOSE_NATIVE_WIN32
    )
endif()

file(TO_CMAKE_PATH ${CMAKE_CURRENT_BINARY_DIR}/log LOG_FOLDER)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/PathConfig.inl
    ${CMAKE_CURRENT_BINARY_DIR}/generated/PathConfig.inl
    @ONLY
)
