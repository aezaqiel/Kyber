cmake_minimum_required(VERSION 3.30)

project(Kyber LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(TINYGLTF_HEADER_ONLY ON CACHE INTERNAL "" FORCE)
set(TINYGLTF_INSTALL OFF CACHE INTERNAL "" FORCE)

add_subdirectory(thirdparty/spdlog)
add_subdirectory(thirdparty/glfw)
add_subdirectory(thirdparty/glm)
add_subdirectory(thirdparty/tinygltf)

file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/**.cpp
)

file(GLOB_RECURSE HDRS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/**.hpp
)
list(REMOVE_ITEM HDRS "src/PCH.hpp")

add_executable(${PROJECT_NAME}
    ${SRCS}
    ${HDRS}
)

target_include_directories(${PROJECT_NAME}
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/stb
)

target_link_libraries(${PROJECT_NAME}
PRIVATE
    spdlog
    glfw
    glm
    tinygltf
)

target_compile_definitions(${PROJECT_NAME}
PRIVATE
    NOMINMAX
    GLFW_INCLUDE_NONE
    GLM_ENABLE_EXPERIMENTAL
    TINYGLTF_NOEXCEPTION
)

if(WIN32)
    target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        GLFW_EXPOSE_NATIVE_WIN32
    )
endif()

target_precompile_headers(${PROJECT_NAME}
PRIVATE
    src/PCH.hpp
)
