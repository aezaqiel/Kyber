#include "Core/Logger.hpp"
#include "Core/JobSystem.hpp"
#include "Core/Application.hpp"

int main()
{
    Kyber::Logger::Init();
    Kyber::JobSystem::Init(8);

    constexpr u32 WIDTH = 1280;
    constexpr u32 HEIGHT = 720;

    Kyber::Application::Config config {
        .window = {
            .width = WIDTH,
            .height = HEIGHT,
            .title = "Kyber"
        }
    };

    Kyber::Application* app = new Kyber::Application(config);
    app->Run();
    delete app;

    Kyber::JobSystem::Shutdown();
    Kyber::Logger::Shutdown();
}
