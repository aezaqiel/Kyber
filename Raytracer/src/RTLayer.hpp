#pragma once

#include <Kyber/Kyber.hpp>

#include "Camera.hpp"

#include "Core/TileScheduler.hpp"
#include "Core/PostProcess.hpp"

#include "Hittables/Hittable.hpp"

namespace Kyber {

    class RTLayer final : public Layer
    {
    public:
        RTLayer();
        virtual ~RTLayer() = default;

        virtual auto OnAttach() -> void override;
        virtual auto OnDetach() -> void override;

        virtual auto OnUpdate(Kyber::f32 dt) -> void override;

        virtual auto OnImGuiRender() -> void override;

    private:
        auto Start() -> void;
        auto Stop() -> void;

        auto Resize(u32 width, u32 height) -> void;

        auto WorkerThread() -> void;
        auto ExecuteTask(const RenderTask& task) -> void;
        auto TraceRay(Ray ray) -> glm::vec3;

    private:
        // TODO: How do we set this
        u32 m_Width { 1920 };
        u32 m_Height { 1080 };
        u32 m_Samples { 1024 };
        u32 m_Depth { 8 };

        u32 m_TileSize { 32 };

        std::atomic<bool> m_Running { false };
        std::vector<std::thread> m_Workers;

        std::unique_ptr<Hittable> m_Scene;
        std::unique_ptr<Camera> m_Camera;

        TileScheduler m_Scheduler;

        std::vector<glm::vec4> m_Accumulator;
        std::unique_ptr<PostProcess> m_PostProcess;
    };

}
