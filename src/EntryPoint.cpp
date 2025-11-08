#include "Core/Application.hpp"

int main()
{
    using namespace Kyber::Core;

    Logger::Init();

    Application* app = new Application();
    app->Run();
    delete app;

    Logger::Shutdown();
}
