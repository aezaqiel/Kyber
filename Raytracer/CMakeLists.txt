
add_executable(Raytracer
    src/Raytracer.hpp
    src/Raytracer.cpp
    src/RTLayer.hpp
    src/RTLayer.cpp

    src/Camera.hpp
    src/Camera.cpp
    src/Core/TileScheduler.hpp
    src/Core/TileScheduler.cpp
    src/Core/RenderQueue.hpp
    src/Core/Framebuffer.hpp
    src/Core/Viewport.hpp
    src/Core/Viewport.cpp
    src/Core/RNG.hpp

    src/Containers/Ray.hpp
    src/Containers/Interval.hpp

    src/Hittables/Hittable.hpp
    src/Hittables/Sphere.hpp
    src/Hittables/Sphere.cpp
    src/Hittables/HittableList.hpp
    src/Hittables/HittableList.cpp

    src/Materials/Material.hpp
    src/Materials/Lambertian.hpp
    src/Materials/Lambertian.cpp
    src/Materials/Metal.hpp
    src/Materials/Metal.cpp
    src/Materials/Dielectric.hpp
    src/Materials/Dielectric.cpp
)

target_include_directories(Raytracer
PRIVATE
    src
    ${CMAKE_CURRENT_BINARY_DIR}/generated
)

target_link_libraries(Raytracer
PRIVATE
    Kyber
)

file(TO_CMAKE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/res RES_FOLDER)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/PathConfig.inl
    ${CMAKE_CURRENT_BINARY_DIR}/generated/PathConfig.inl
    @ONLY
)
