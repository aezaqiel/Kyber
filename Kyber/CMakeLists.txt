
add_library(Kyber STATIC
    src/Kyber/Kyber.hpp

    src/Kyber/Core/EntryPoint.cpp

    src/Kyber/Core/Types.hpp
    src/Kyber/Core/Application.hpp
    src/Kyber/Core/Application.cpp
)

target_include_directories(Kyber
PUBLIC
    src
)

target_link_libraries(Kyber
PUBLIC
)

target_precompile_headers(Kyber
PUBLIC
    src/PCH.hpp
)

target_compile_definitions(Kyber
PUBLIC
    NOMINMAX
)
