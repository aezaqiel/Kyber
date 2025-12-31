#include "TileScheduler.hpp"

namespace Kyber {

    auto TileScheduler::Reset(u32 width, u32 height, u32 tileSize, u32 totalSamples) -> void
    {
        m_Tiles.clear();

        u32 cols = (width + tileSize - 1) / tileSize;
        u32 rows = (height + tileSize - 1) / tileSize;

        for (u32 row = 0; row < rows; ++row) {
            for (u32 col = 0; col < cols; ++col) {
                u32 x = col * tileSize;
                u32 y = row * tileSize;
                u32 w = std::min(tileSize, width - x);
                u32 h = std::min(tileSize, height - y);

                m_Tiles.push_back({x, y, w, h});
            }
        }

        m_TotalTiles = m_Tiles.size();
        m_TotalSamples = totalSamples;
        m_Offset.store(0);
    }
    
    auto TileScheduler::GetTask(RenderTask& task) -> bool
    {
        u64 currentIndex = m_Offset.fetch_add(1, std::memory_order_relaxed);

        u64 totalTasks = static_cast<u64>(m_TotalTiles) * static_cast<u64>(m_TotalSamples);
        if (currentIndex >= totalTasks) return false;

        u32 samplePass = static_cast<u32>(currentIndex / m_TotalTiles);
        u32 tileIndex = static_cast<u32>(currentIndex % m_TotalTiles);

        task.tile = m_Tiles[tileIndex];
        task.sample = samplePass + 1;

        return true;
    }

}
