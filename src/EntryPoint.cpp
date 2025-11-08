#include "Core/JobSystem.hpp"
#include "Core/Application.hpp"

int main()
{
    using namespace Kyber::Core;

    Logger::Init();
    JobSystem::Init();

    Application* app = new Application();
    app->Run();
    delete app;

    JobSystem::Shutdown();
    Logger::Shutdown();
}
