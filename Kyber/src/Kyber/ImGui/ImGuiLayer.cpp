#include "ImGuiLayer.hpp"

#include <imgui_internal.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <PathConfig.inl>

namespace Kyber {

    namespace {

        std::filesystem::path s_ResPath(PathConfig::ResDir);
    
    }

    ImGuiLayer::ImGuiLayer(const std::shared_ptr<Window>& window)
        : Layer("ImGui"), m_Window(window)
    {
    }

    auto ImGuiLayer::OnAttach() -> void
    {
        IMGUI_CHECKVERSION();

        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        ImGui::StyleColorsDark();

        ImGuiStyle& style = ImGui::GetStyle();

        // 1. Configuration
        style.FrameBorderSize = 1.0f;
        style.WindowBorderSize = 0.0f; // No border around windows for a clean docking look
        style.PopupBorderSize = 1.0f;
        style.ChildBorderSize = 1.0f;
        style.TabBorderSize = 0.0f;

        // 2. Geometry (Modern/Soft)
        style.WindowRounding = 4.0f;
        style.ChildRounding = 4.0f;
        style.FrameRounding = 4.0f;
        style.PopupRounding = 4.0f;
        style.ScrollbarRounding = 12.0f; // Pill shape
        style.GrabRounding = 4.0f;
        style.TabRounding = 4.0f;

        // 3. Spacing
        style.WindowPadding = ImVec2(10.0f, 10.0f);
        style.FramePadding = ImVec2(6.0f, 4.0f);     // More vertical padding on inputs
        style.ItemSpacing = ImVec2(8.0f, 6.0f);      // Comfortable spacing between widgets
        style.ItemInnerSpacing = ImVec2(6.0f, 6.0f);
        style.IndentSpacing = 20.0f;
        style.ScrollbarSize = 14.0f;                 // Slightly thinner scrollbar

        style.WindowMenuButtonPosition = ImGuiDir_None;

        // 4. Color Palette
        ImVec4* colors = style.Colors;

        // Define base colors for easy tweaking
        const ImVec4 colorBGVeryDark  = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
        const ImVec4 colorBGDark      = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        const ImVec4 colorBGMedium    = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
        const ImVec4 colorBGLight     = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);

        const ImVec4 colorText         = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
        const ImVec4 colorTextDisabled = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);

        // Primary Accent: "Steel Blue"
        const ImVec4 colorAccent       = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        const ImVec4 colorAccentHover  = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
        const ImVec4 colorAccentActive = ImVec4(0.26f, 0.59f, 0.98f, 0.60f);

        // --- Base Elements ---
        colors[ImGuiCol_Text] = colorText;
        colors[ImGuiCol_TextDisabled] = colorTextDisabled;
        colors[ImGuiCol_WindowBg] = colorBGDark;
        colors[ImGuiCol_ChildBg] = colorBGVeryDark; // Darker background for lists/trees
        colors[ImGuiCol_PopupBg] = colorBGDark;
        colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.20f); // Subtle border
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

        // --- Headers & Title Bars ---
        // Make title bars blend with the window to save vertical visual space
        colors[ImGuiCol_TitleBg] = colorBGDark;
        colors[ImGuiCol_TitleBgActive] = colorBGDark;
        colors[ImGuiCol_TitleBgCollapsed] = colorBGDark;
        colors[ImGuiCol_MenuBarBg] = colorBGVeryDark;

        // --- Scrolling ---
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.00f); // Transparent track
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);

        // --- Interactive Elements (Checkboxes, Inputs, Sliders) ---
        colors[ImGuiCol_FrameBg] = colorBGMedium;
        colors[ImGuiCol_FrameBgHovered] = colorBGLight;
        colors[ImGuiCol_FrameBgActive] = colorBGLight;

        colors[ImGuiCol_CheckMark] = colorAccent;
        colors[ImGuiCol_SliderGrab] = colorAccent;
        colors[ImGuiCol_SliderGrabActive] = colorAccentActive;

        // --- Buttons ---
        colors[ImGuiCol_Button] = colorBGMedium;
        colors[ImGuiCol_ButtonHovered] = colorBGLight;
        colors[ImGuiCol_ButtonActive] = colorAccentActive;

        // --- Headers (Collapsing Header, Tree Nodes) ---
        colors[ImGuiCol_Header] = colorBGMedium;
        colors[ImGuiCol_HeaderHovered] = colorBGLight;
        colors[ImGuiCol_HeaderActive] = colorBGLight;

        // --- Tabs (Docking) ---
        colors[ImGuiCol_Tab] = colorBGDark;
        colors[ImGuiCol_TabHovered] = colorAccentHover;
        colors[ImGuiCol_TabActive] = colorBGMedium;        // Active tab matches the window bg
        colors[ImGuiCol_TabUnfocused] = colorBGDark;
        colors[ImGuiCol_TabUnfocusedActive] = colorBGMedium;

        // --- Docking Preview ---
        // The blue overlay when dragging windows
        colors[ImGuiCol_DockingPreview] = ImVec4(colorAccent.x, colorAccent.y, colorAccent.z, 0.30f);
        colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f); // Fallback

        // --- Plotting / Graphs ---
        colors[ImGuiCol_PlotLines] = colorText;
        colors[ImGuiCol_PlotLinesHovered] = colorAccent;
        colors[ImGuiCol_PlotHistogram] = colorText;
        colors[ImGuiCol_PlotHistogramHovered] = colorAccent;

        // --- Text Selection ---
        colors[ImGuiCol_TextSelectedBg] = ImVec4(colorAccent.x, colorAccent.y, colorAccent.z, 0.35f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);

        // --- Navigation ---
        colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

        io.Fonts->Clear();

        f32 baseFontSize = 18.0f;

        ImFontConfig fontCfg;
        fontCfg.PixelSnapH = true;
        fontCfg.OversampleH = 2;
        fontCfg.OversampleV = 2;

        std::string fontFile = (s_ResPath / "Fonts/Inter/Inter-VariableFont_opsz,wght.ttf").string();
        io.Fonts->AddFontFromFileTTF(fontFile.c_str(), baseFontSize, &fontCfg);

        GLFWwindow* window = m_Window->GetNative();

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 460");
    }

    auto ImGuiLayer::OnDetach() -> void
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    auto ImGuiLayer::OnEvent(const EventDispatcher& dispatcher) -> void
    {
        ImGuiIO& io = ImGui::GetIO();

        if (io.WantCaptureMouse) {
            dispatcher.Block<MouseButtonPressedEvent>();
            dispatcher.Block<MouseButtonReleasedEvent>();
            dispatcher.Block<MouseMovedEvent>();
            dispatcher.Block<MouseScrolledEvent>();
        }

        if (io.WantCaptureKeyboard) {
            dispatcher.Block<KeyPressedEvent>();
            dispatcher.Block<KeyReleasedEvent>();
            dispatcher.Block<KeyTypedEvent>();
        }
    }

    auto ImGuiLayer::Begin() -> void
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        ImGui::Begin("DockSpace", nullptr, window_flags);
        ImGui::PopStyleVar();
        ImGui::PopStyleVar(2);

        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_NoTabBar);
    }

    auto ImGuiLayer::End() -> void
    {
        ImGui::End();

        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(static_cast<f32>(m_Window->GetWidth()), static_cast<f32>(m_Window->GetHeight()));

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* ctx = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(ctx);
        }
    }

}
