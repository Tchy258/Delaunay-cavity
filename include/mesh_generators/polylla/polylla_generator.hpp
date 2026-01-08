#ifndef POLYLLA_GENERATOR_HPP
#define POLYLLA_GENERATOR_HPP
#include<concepts/mesh_data.hpp>
#include<mesh_generators/mesh_generator.hpp>
#include<mesh_generators/polylla/mesh_helpers/mesh_helper_polylla.hpp>
#include<mesh_generators/polylla/helper_structs/polylla_data.hpp>
#include<misc/mesh_stat.hpp>
#include<misc/time_stat.hpp>

template <MeshData MeshType>
class PolyllaGenerator : public MeshGenerator<MeshType> {
    public:
        using MeshVertex = typename MeshType::VertexType;
        using MeshEdge = typename MeshType::EdgeType;
        using VertexIndex = typename MeshType::VertexIndex;
        using EdgeIndex = typename MeshType::EdgeIndex;
        using FaceIndex = typename MeshType::FaceIndex;
        using OutputIndex = typename MeshType::OutputIndex;
    private:
        using _MeshHelper = generators::helpers::polylla::MeshHelper<MeshType>;
        using BinaryVector = std::vector<uint8_t>;
        PolyllaData<MeshType> data;

    public:
        PolyllaGenerator() = default;
        std::vector<OutputIndex>& getOutputSeeds() override {
            return data.outputSeeds;
        }
        std::vector<OutputIndex>& getOutputSeedsBeforePostProcess() override {
            return data.outputSeeds;
        }
        MeshType* getMeshBeforePostProcess() override {
            return nullptr;
        }
        MeshType* generateMesh(const MeshType* inputMesh) override;
        const std::unordered_map<MeshStat,int>& getGenerationStats() override {
            return data.meshStats;
        }
        const std::unordered_map<TimeStat,double>& getGenerationTimes() override {
            return data.timeStats;
        }
        const std::unordered_map<MemoryStat, unsigned long long>& getGenerationMemory() override {
            return data.memoryStats;
        }
};
#include <mesh_generators/polylla/polylla_generator.ipp>

#endif