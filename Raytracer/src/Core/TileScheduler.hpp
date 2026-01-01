#pragma once

namespace Kyber {

    struct Tile
    {
        u32 x;
        u32 y;
        u32 w;
        u32 h;
    };

    struct RenderTask
    {
        Tile tile;
        u32 sample;
    };

    class TileScheduler
    {
    public:
        TileScheduler() = default;
        ~TileScheduler() = default;

        auto Reset(u32 width, u32 height, u32 tileSize, u32 totalSamples) -> void;
        auto GetTask(RenderTask& task) -> bool;

        auto GetProgress() -> f32;

    private:
        std::vector<Tile> m_Tiles;
        std::atomic<u64> m_Offset { 0 };
        usize m_TotalTiles { 0 };
        u32 m_TotalSamples { 0 };
    };

}
