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
        PolyllaRefiner() {
            data.meshStats[N_POLYGONS] = 0;
            data.meshStats[N_FRONTIER_EDGES] = 0;
            data.meshStats[N_BARRIER_EDGE_TIPS] = 0;
            data.meshStats[N_POLYGONS_TO_REPAIR] = 0;
            data.meshStats[N_POLYGONS_ADDED_AFTER_REPAIR] = 0;
            data.meshStats[N_VERTICES] = 0;
            data.meshStats[N_EDGES] = 0;
            data.meshStats[N_BORDER_EDGES] = 0;
            
            data.timeStats[T_TRIANGULATION_GENERATION] = 0.0;
            data.timeStats[T_LABEL_MAX_EDGES] = 0.0;
            data.timeStats[T_LABEL_FRONTIER_EDGES] = 0.0;
            data.timeStats[T_LABEL_SEED_EDGES] = 0.0;
            data.timeStats[T_TRAVERSAL_AND_REPAIR] = 0.0;
            data.timeStats[T_TRAVERSAL] = 0.0;
            data.timeStats[T_REPAIR] = 0.0;

            data.memoryStats[M_TOTAL] = 0;
            data.memoryStats[M_MAX_EDGES] = 0;
            data.memoryStats[M_FRONTIER_EDGES] = 0;
            data.memoryStats[M_SEED_EDGES] = 0;
            data.memoryStats[M_SEED_BOUNDARY_EDGE_TIP_MARK] = 0;
            data.memoryStats[M_TRIANGLE_LIST] = 0;
            data.memoryStats[M_EDGES_INPUT] = 0;
            data.memoryStats[M_EDGES_OUTPUT] = 0;
            data.memoryStats[M_VERTICES_INPUT] = 0;
            data.memoryStats[M_VERTICES_OUTPUT] = 0;
        }
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
        std::unordered_map<MeshStat,int>& getRefinementStats() override {
            return data.meshStats;
        }
        std::unordered_map<TimeStat,double>& getRefinementTimes() override {
            return data.timeStats;
        }
        std::unordered_map<MemoryStat, unsigned long long>& getRefinementMemory() override {
            return data.memoryStats;
        }
};

#include <mesh_refiners/polylla/polylla_refiner.ipp>

#endif