
add_executable(Raytracer
    src/Main.cpp
    src/Raytracer.hpp
    src/Raytracer.cpp
    src/RTLayer.hpp
    src/RTLayer.cpp
)

target_include_directories(Raytracer
PRIVATE
    src
)

target_link_libraries(Raytracer
PRIVATE
    Kyber
)
