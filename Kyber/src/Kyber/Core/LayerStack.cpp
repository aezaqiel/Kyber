#include "LayerStack.hpp"

namespace Kyber {

    LayerStack::~LayerStack()
    {
        for (auto& layer : m_Layers) {
            layer->OnDetach();
        }
    }

    auto LayerStack::PushLayer(const std::shared_ptr<Layer>& layer) -> void
    {
        m_Layers.insert(m_Layers.begin() + m_LayerInsertIndex, layer);
        m_LayerInsertIndex++;

        layer->OnAttach();
    }

    auto LayerStack::PushOverlay(const std::shared_ptr<Layer>& overlay) -> void
    {
        m_Layers.push_back(overlay);
        overlay->OnAttach();
    }

    auto LayerStack::PopLayer(const std::shared_ptr<Layer>& layer) -> void
    {
        auto it = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);
        if (it != m_Layers.end()) {
            m_Layers.erase(it);
            m_LayerInsertIndex--;
        }

        layer->OnDetach();
    }

    auto LayerStack::PopOverlay(const std::shared_ptr<Layer>& overlay) -> void
    {
        auto it = std::find(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(), overlay);
        if (it != m_Layers.end()) {
            m_Layers.erase(it);
        }

        overlay->OnDetach();
    }

}
