#ifndef POLYLLA_REFINER_HPP
#define POLYLLA_REFINER_HPP
#include<concepts/mesh_data.hpp>
#include<mesh_refiners/mesh_refiner.hpp>
#include<mesh_refiners/polylla/mesh_helpers/mesh_helper_polylla.hpp>
#include<mesh_refiners/polylla/helper_structs/polylla_data.hpp>
#include<misc/mesh_stat.hpp>
#include<misc/time_stat.hpp>

template <MeshData MeshType>
class PolyllaRefiner : public MeshRefiner<MeshType> {
    public:
        using MeshVertex = typename MeshType::VertexType;
        using MeshEdge = typename MeshType::EdgeType;
        using VertexIndex = typename MeshType::VertexIndex;
        using EdgeIndex = typename MeshType::EdgeIndex;
        using FaceIndex = typename MeshType::FaceIndex;
        using OutputIndex = typename MeshType::OutputIndex;
    private:
        using _MeshHelper = refiners::helpers::polylla::MeshHelper<MeshType>;
        using BinaryVector = std::vector<uint8_t>;
        PolyllaData<MeshType> data;

    public:
        PolyllaRefiner() = default;
        std::vector<OutputIndex>& getOutputSeeds() override {
            return data.outputSeeds;
        }
        std::vector<OutputIndex>& getOutputSeedsBeforePostProcess() override {
            return data.outputSeeds;
        }
        MeshType* getMeshBeforePostProcess() override {
            return nullptr;
        }
        MeshType* refineMesh(const MeshType* inputMesh) override;
        const std::unordered_map<MeshStat,int>& getRefinementStats() override {
            return data.meshStats;
        }
        const std::unordered_map<TimeStat,double>& getRefinementTimes() override {
            return data.timeStats;
        }
        const std::unordered_map<MemoryStat, unsigned long long>& getRefinementMemory() override {
            return data.memoryStats;
        }
};
#include <mesh_refiners/polylla/polylla_refiner.ipp>

#endif