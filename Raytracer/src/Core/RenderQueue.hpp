#pragma once

#include "TileScheduler.hpp"

namespace Kyber {

    class RenderQueue
    {
    public:
        auto Push(const Tile& tile) -> void
        {
            std::scoped_lock<std::mutex> lock(m_Mutex);
            m_Queue.push_back(tile);
        }

        auto Flush() -> std::vector<Tile>
        {
            std::scoped_lock<std::mutex> lock(m_Mutex);
            if (m_Queue.empty()) return {};

            std::vector<Tile> queue = std::move(m_Queue);
            m_Queue.clear();

            return queue;
        }

    private:
        std::mutex m_Mutex;
        std::vector<Tile> m_Queue;
    };

}
