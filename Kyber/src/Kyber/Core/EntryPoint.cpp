#include "Application.hpp"

extern Kyber::Application* Kyber::CreateApplication();

auto main() -> int
{
    Kyber::Application* app = Kyber::CreateApplication();
    app->Run();
    delete app;
}
