#pragma once

#include "Layer.hpp"

namespace Kyber {

    class LayerStack
    {
    public:
        LayerStack() = default;
        ~LayerStack();

        auto PushLayer(const std::shared_ptr<Layer>& layer) -> void;
        auto PushOverlay(const std::shared_ptr<Layer>& overlay) -> void;

        auto PopLayer(const std::shared_ptr<Layer>& layer) -> void;
        auto PopOverlay(const std::shared_ptr<Layer>& overlay) -> void;

        auto begin() -> std::vector<std::shared_ptr<Layer>>::iterator
        {
            return m_Layers.begin();
        }

        auto end() -> std::vector<std::shared_ptr<Layer>>::iterator
        {
            return m_Layers.end();
        }

        auto begin() const -> const std::vector<std::shared_ptr<Layer>>::const_iterator
        {
            return m_Layers.begin();
        }

        auto end() const -> const std::vector<std::shared_ptr<Layer>>::const_iterator
        { 
            return m_Layers.end();
        }

        auto rbegin() -> std::vector<std::shared_ptr<Layer>>::reverse_iterator
        { 
            return m_Layers.rbegin();
        }

        auto rend() -> std::vector<std::shared_ptr<Layer>>::reverse_iterator
        { 
            return m_Layers.rend();
        }

        auto rbegin() const -> const std::vector<std::shared_ptr<Layer>>::const_reverse_iterator
        { 
            return m_Layers.rbegin();
        }

        auto rend() const -> const std::vector<std::shared_ptr<Layer>>::const_reverse_iterator
        { 
            return m_Layers.rend();
        }

    private:
        std::vector<std::shared_ptr<Layer>> m_Layers;
        usize m_LayerInsertIndex { 0 };
    };

}
