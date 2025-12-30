#pragma once

namespace Kyber {

    class Application
    {
    public:
        Application();
        virtual ~Application();

        auto Run() -> void;
    };

    Application* CreateApplication();

}
