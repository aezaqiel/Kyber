cmake_minimum_required(VERSION 3.30)

project(Application LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(thirdparty/spdlog)
add_subdirectory(thirdparty/glfw)

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
)

target_link_libraries(${PROJECT_NAME}
PRIVATE
    spdlog
    glfw
)

target_compile_definitions(${PROJECT_NAME}
PRIVATE
    NOMINMAX
    GLFW_INCLUDE_NONE
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
