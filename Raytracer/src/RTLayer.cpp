#include "RTLayer.hpp"

#include <imgui.h>

#include "Core/RNG.hpp"

#include "Hittables/HittableList.hpp"
#include "Hittables/Sphere.hpp"

#include "Materials/Material.hpp"
#include "Materials/Lambertian.hpp"
#include "Materials/Metal.hpp"
#include "Materials/Dielectric.hpp"

namespace Kyber {

    namespace {

        auto Book1Scene() -> std::unique_ptr<HittableList>
        {
            std::vector<std::shared_ptr<Hittable>> hittables;

            hittables.push_back(std::make_shared<Sphere>(
                glm::vec3(0.0f, -1000.0f, 0.0f), 1000.0f,
                std::make_shared<Lambertian>(glm::vec3(0.5f))
            ));

            for (i32 a = -11; a < 11; ++a) {
                for (i32 b = -11; b < 11; ++b) {
                    f32 choose = RNG::F32();
                    glm::vec3 center(a + 0.9f * RNG::F32(), 0.2f, b + 0.9f * RNG::F32());

                    if (glm::length(center - glm::vec3(4.0f, 0.2f, 0.0f)) < 0.9f) continue;

                    if (choose < 0.8f) {
                        glm::vec3 albedo = RNG::Vec3() * RNG::Vec3();
                        hittables.push_back(std::make_shared<Sphere>(
                            center, 0.2f,
                            std::make_shared<Lambertian>(albedo)
                        ));
                    } else if (choose < 0.95f) {
                        glm::vec3 albedo = RNG::Vec3(0.5f, 1.0f);
                        f32 fuzz = RNG::F32(0.0f, 0.5f);
                        hittables.push_back(std::make_shared<Sphere>(
                            center, 0.2f,
                            std::make_shared<Metal>(albedo, fuzz)
                        ));
                    } else {
                        hittables.push_back(std::make_shared<Sphere>(
                            center, 0.2f,
                            std::make_shared<Dielectric>(1.5f)
                        ));
                    }
                }
            }

            hittables.push_back(std::make_shared<Sphere>(
                glm::vec3(0.0f, 1.0f, 0.0f), 1.0f,
                std::make_shared<Dielectric>(1.5f)
            ));

            hittables.push_back(std::make_shared<Sphere>(
                glm::vec3(-4.0f, 1.0f, 0.0f), 1.0f,
                std::make_shared<Lambertian>(glm::vec3(0.4f, 0.2f, 0.1f))
            ));

            hittables.push_back(std::make_shared<Sphere>(
                glm::vec3(4.0f, 1.0f, 0.0f), 1.0f,
                std::make_shared<Metal>(glm::vec3(0.7f, 0.6f, 0.5f), 0.0f)
            ));

            return std::make_unique<HittableList>(hittables);
        }
    
    }

    RTLayer::RTLayer()
    {
        m_Scene = Book1Scene();

        m_Camera = std::make_unique<Camera>(
            m_Width,
            m_Height,
            20.0f,
            glm::vec3(13.0, 2.0f, 3.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            0.6f,
            10.0f
        );

        m_Accumulator.assign(m_Width * m_Height, glm::vec4(0.0f));
        m_PostProcess = std::make_unique<PostProcess>(m_Width, m_Height);
    }

    auto RTLayer::OnAttach() -> void
    {
    }

    auto RTLayer::OnDetach() -> void
    {
        Stop();
    }

    auto RTLayer::OnUpdate(Kyber::f32 dt) -> void
    {
        auto tiles = m_RenderQueue.Flush();
        if (!tiles.empty()) {
            m_PostProcess->UploadTiles(m_Accumulator, tiles);
            m_PostProcess->Dispatch();
        }
    }

    auto RTLayer::OnImGuiRender() -> void
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Viewport");

        ImVec2 size = ImGui::GetContentRegionAvail();

        if (size.x > 0 && size.y > 0) {
            f32 imageAR = static_cast<f32>(m_Width) / static_cast<f32>(m_Height);
            f32 viewportAR = size.x / size.y;

            ImVec2 imageSize = size;
            if (viewportAR > imageAR) {
                imageSize.x = size.y * imageAR;
            } else {
                imageSize.y = size.x / imageAR;
            }

            ImVec2 cursorPos = ImGui::GetCursorPos();
            ImVec2 offset = { (size.x - imageSize.x) * 0.5f, (size.y - imageSize.y) * 0.5f };
            ImGui::SetCursorPos({ cursorPos.x + offset.x, cursorPos.y + offset.y });

            ImGui::Image(
                static_cast<ImTextureID>(static_cast<intptr_t>(m_PostProcess->GetTextureID())),
                imageSize,
                ImVec2(0, 1), ImVec2(1, 0)
            );
        }

        ImGui::End();
        ImGui::PopStyleVar();

        ImGui::Begin("Settings");

        ImGui::Text("Resolution: %dx%d", m_Width, m_Height);
        ImGui::Text("Samples: %d", m_Samples);
        ImGui::Text("Depth: %d", m_Depth);

        ImGui::Separator();

        if (!m_Running) {
            if (ImGui::Button("Start Render")) {
                Start();
            }
        } else {
            if (ImGui::Button("Stop Render")) {
                Stop();
            }
        }

        ImGui::End();

        ImGui::Begin("Metrics");

        ImGui::Text("Viewport FPS: %.2f", ImGui::GetIO().Framerate);
        ImGui::Text("Render Progress: %.2f", m_Scheduler.GetProgress() * 100.0f);

        ImGui::End();
    }

    auto RTLayer::Start() -> void
    {
        Stop();

        m_Running = true;

        m_Scheduler.Reset(m_Width, m_Height, m_TileSize, m_Samples);
        (void)m_RenderQueue.Flush();

        m_Accumulator.assign(m_Width * m_Height, glm::vec4(0.0f));

        u32 workerCount = std::max(1u, std::thread::hardware_concurrency() - 2);

        for (u32 i = 0; i < workerCount; ++i) {
            m_Workers.emplace_back(&RTLayer::WorkerThread, this);
        }
    }

    auto RTLayer::Stop() -> void
    {
        m_Running = false;

        for (auto& worker : m_Workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }

        m_Workers.clear();
    }

    auto RTLayer::Resize(u32 width, u32 height) -> void
    {
        if (width == m_Width && height == m_Height) return;

        Stop();

        m_Width = width;
        m_Height = height;

        Start();
    }

    auto RTLayer::WorkerThread() -> void
    {
        RenderTask task;
        while (m_Running && m_Scheduler.GetTask(task)) {
            ExecuteTask(task);
            m_RenderQueue.Push(task.tile);
        }
    }

    auto RTLayer::ExecuteTask(const RenderTask& task) -> void
    {
        for (u32 y = task.tile.y; y < task.tile.y + task.tile.h; ++y) {
            for (u32 x = task.tile.x; x < task.tile.x + task.tile.w; ++x) {
                glm::vec2 offset = RNG::Vec2() - 0.5f;
                Ray ray = m_Camera->GetRay(x, y, offset);

                glm::vec3 color = TraceRay(ray);

                usize index = x + y * m_Width;
                m_Accumulator[index] += glm::vec4(color, 0.0f);
                m_Accumulator[index].a = static_cast<f32>(task.sample);
            }
        }
    }

    auto RTLayer::TraceRay(Ray ray) -> glm::vec3
    {
        glm::vec3 throughput(1.0f);
        glm::vec3 accumulated(0.0f);

        for (u32 depth = 0; depth < m_Depth; ++depth) {
            if (auto hit = m_Scene->Hit(ray, Interval(0.0001f, std::numeric_limits<f32>::infinity()))) {
                // TODO: emissions
                accumulated += throughput * glm::vec3(0.0f);

                if (auto scatter = hit->material->Scatter(ray, *hit)) {
                    ray = scatter->scattered;
                    throughput *= scatter->attenuation;
                } else {
                    break;
                }
            } else {
                glm::vec3 dir = glm::normalize(ray.direction);
                f32 t = 0.5f * (dir.y + 1.0f);
                glm::vec3 sky = glm::mix(glm::vec3(1.0f), glm::vec3(0.5f, 0.7f, 1.0f), t);

                accumulated += throughput * sky;
                break;
            }
        }

        return accumulated;
    }

}
