#pragma once

#include <Kyber/Kyber.hpp>

#include "Camera.hpp"

#include "Acceleration/BVH.hpp"

#include "Core/TileScheduler.hpp"
#include "Core/RenderQueue.hpp"
#include "Core/PostProcess.hpp"

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
        auto Reset() -> void;

        auto WorkerThread() -> void;
        auto ExecuteTask(const RenderTask& task) -> void;
        auto TraceRay(Ray ray, u32& rayCount) -> glm::vec3;

    private:
        glm::uvec2 m_Resolution { 1920, 1080 };
        u32 m_Samples { 512 };
        u32 m_Depth { 8 };
        u32 m_TileSize { 32 };

        TileScheduler m_Scheduler;
        RenderQueue m_RenderQueue;

        std::atomic<bool> m_Running { false };
        std::vector<std::thread> m_Workers;

        std::unique_ptr<BVH> m_Aggregate;
        std::unique_ptr<Camera> m_Camera;

        std::vector<glm::vec4> m_Accumulator;
        std::unique_ptr<PostProcess> m_PostProcess;

        std::chrono::time_point<std::chrono::steady_clock> m_RenderStartTime;
        f32 m_AccumulatedTime { 0.0f };
        std::atomic<u64> m_TotalRayCount { 0 };
    };

}
