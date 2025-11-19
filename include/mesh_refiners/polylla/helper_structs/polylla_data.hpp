#ifndef POLYLLA_DATA_HPP
#define POLYLLA_DATA_HPP

#include <misc/mesh_stat.hpp>
#include <misc/time_stat.hpp>
#include <misc/memory_stat.hpp>
#include <unordered_map>
#include <vector>
#include <concepts/mesh_data.hpp>

template <MeshData MeshType>
struct PolyllaData {
    using OutputIndex = typename MeshType::OutputIndex;
    using BinaryVector = std::vector<uint8_t>;

    std::unordered_map<MeshStat, int> meshStats;
    std::unordered_map<TimeStat, double> timeStats;
    std::unordered_map<MemoryStat, unsigned long long> memoryStats;
    std::vector<OutputIndex> outputSeeds; //Seeds of the polygon

    //std::vector<int> triangles; //True if the edge generated a triangle CHANGE!!!!

    BinaryVector maxEdges; //True if the edge i is a max edge
    BinaryVector frontierEdges; //True if the edge i is a frontier edge
    std::vector<OutputIndex> seedCandidates; //Seed edges that generate polygon simple and non-simple

    BinaryVector seedBarrierEdgeTipMark;
};

#endif // POLYLLA_DATA_HPP