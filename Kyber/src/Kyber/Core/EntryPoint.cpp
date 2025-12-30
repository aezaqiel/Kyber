#include "Logger.hpp"
#include "Application.hpp"

extern Kyber::Application* Kyber::CreateApplication();

auto main() -> int
{
    Kyber::Logger::Init();

    Kyber::Application* app = Kyber::CreateApplication();
    app->Run();
    delete app;

    Kyber::Logger::Shutdown();
}
