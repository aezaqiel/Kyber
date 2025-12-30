add_subdirectory(thirdparty/spdlog)

add_library(Kyber STATIC
    src/Kyber/Kyber.hpp

    src/Kyber/Core/EntryPoint.cpp

    src/Kyber/Core/Types.hpp
    src/Kyber/Core/Application.hpp
    src/Kyber/Core/Application.cpp
    src/Kyber/Core/Logger.hpp
    src/Kyber/Core/Logger.cpp
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
)

target_precompile_headers(Kyber
PUBLIC
    src/PCH.hpp
)

target_compile_definitions(Kyber
PUBLIC
    NOMINMAX
)

file(TO_CMAKE_PATH ${CMAKE_CURRENT_BINARY_DIR}/log LOG_FOLDER)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/PathConfig.inl
    ${CMAKE_CURRENT_BINARY_DIR}/generated/PathConfig.inl
    @ONLY
)
