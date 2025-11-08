#include "Core/Application.hpp"

int main()
{
    Core::Logger::Init();

    Core::Application* app = new Core::Application();
    app->Run();
    delete app;

    Core::Logger::Shutdown();
}
