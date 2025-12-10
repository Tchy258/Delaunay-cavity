#ifndef DELAUNAY_CAVITY_DATA_HPP
#define DELAUNAY_CAVITY_DATA_HPP
#include <mesh_refiners/mesh_refiner_data.hpp>
#include <concepts/mesh_data.hpp>

template <MeshData Mesh>
struct DelaunayCavityData : public MeshRefinerData {
    using OutputIndex = typename Mesh::OutputIndex;
    std::vector<OutputIndex> outputSeeds;
    std::vector<uint8_t> inCavity;

    inline void removeInvalidSeeds() {
        outputSeeds.erase(std::remove(outputSeeds.begin(), outputSeeds.end(), -1), outputSeeds.end());
    }

    inline bool isValidSeed(OutputIndex seed) const {
        return outputSeeds[seed] != Mesh::invalidIndexValue;
    }
    DelaunayCavityData() {
        timeStats[T_TRIANGLE_SORTING] = 0.0;
        timeStats[T_CAVITY_COMPUTATION] = 0.0;
        timeStats[T_CAVITY_INSERTION] = 0.0;
        timeStats[T_CAVITY_MERGING] = 0.0;
        
        memoryStats[M_CAVITY_ARRAY] = 0;
        memoryStats[M_VISITED_ARRAY] = 0;
        memoryStats[M_EDGE_MAP] = 0;
    }
};

#endif