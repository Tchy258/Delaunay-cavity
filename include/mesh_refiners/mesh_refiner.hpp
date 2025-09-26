#ifndef MESH_REFINER_HPP
#define MESH_REFINER_HPP

#include <concepts/mesh_data.hpp>
#include <misc/mesh_stat.hpp>
#include <misc/time_stat.hpp>
#include <cstdint>
template <MeshData Mesh>
class MeshRefiner {
    public:
        virtual Mesh* refineMesh(const Mesh* inputMesh) = 0;
        virtual std::vector<typename Mesh::OutputIndex>& getOutputSeeds() = 0;
        virtual std::unordered_map<MeshStat,int>& getRefinementStats() = 0;
        virtual std::unordered_map<TimeStat,double>& getRefinementTimes() = 0;
};

#endif