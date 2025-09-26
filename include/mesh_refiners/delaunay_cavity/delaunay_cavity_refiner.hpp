#ifndef DELAUNAY_CAVITY_REFINER_HPP
#define DELAUNAY_CAVITY_REFINER_HPP
#include<unordered_map>
#include<unordered_set>
#include<cstdint>
#include<queue>
#include<array>
#include<misc/mesh_stat.hpp>
#include<misc/time_stat.hpp>
#include<concepts/mesh_data.hpp>
#include<concepts/cavity_merging_strategy.hpp>
#include<concepts/refinement_criterion.hpp>
#include<concepts/is_half_edge_vertex.hpp>
#include<mesh_data/structures/vertex.hpp>
#include<mesh_data/half_edge_mesh.hpp>
#include<mesh_refiners/mesh_refiner.hpp>
#include<mesh_refiners/delaunay_cavity/mesh_helpers/mesh_helper_delaunay_cavity.hpp>
#include<mesh_refiners/delaunay_cavity/helper_structs/cavity.hpp>
#include<stdexcept>
#include<string>
#include<cmath>

template <MeshData MeshType, RefinementCriterion<MeshType> Criterion, CavityMergingStrategy<MeshType> MergingStrategy>
class DelaunayCavityRefiner : public MeshRefiner<MeshType> {
    public:
        using MeshVertex = typename MeshType::VertexType;
        using MeshEdge = typename MeshType::EdgeType;
        using VertexIndex = typename MeshType::VertexIndex;
        using EdgeIndex = typename MeshType::EdgeIndex;
        using FaceIndex = typename MeshType::FaceIndex;
        using OutputIndex = typename MeshType::OutputIndex;
    private:
        Criterion refinementCriterion;
        std::vector<OutputIndex> outputSeeds;
        using _MeshHelper = refiners::helpers::delaunay_cavity::MeshHelper<MeshType>;
        using Cavity = refiners::helpers::delaunay_cavity::Cavity<MeshType>;
        std::unordered_map<MeshStat, int> meshStats;
        std::unordered_map<TimeStat, double> timeStats;

        std::vector<std::pair<MeshVertex,FaceIndex>> findMatchingCircumcenters(MeshType* outputMesh, size_t polygonAmount);
        std::vector<Cavity> computeCavities(MeshType* outputMesh, const std::vector<std::pair<MeshVertex,FaceIndex>>& circumcenters, std::vector<uint8_t>& visited);

        inline void resetVisited(std::vector<uint8_t>& visited, const Cavity& cavity) {
            for (FaceIndex triangle: cavity.allTriangles) {
                visited[triangle] = 0;
            }
        }
        
    public:
        MeshType* refineMesh(const MeshType* inputMesh) override;
        std::vector<OutputIndex>& getOutputSeeds() override {
            return outputSeeds;
        }
        explicit DelaunayCavityRefiner(Criterion criterion) : refinementCriterion(std::move(criterion)) {
            meshStats[N_POLYGONS] = 0;
            meshStats[N_FRONTIER_EDGES] = 0;
            meshStats[N_BARRIER_EDGE_TIPS] = 0;
            meshStats[N_POLYGONS_TO_REPAIR] = 0;
            meshStats[N_POLYGONS_ADDED_AFTER_REPAIR] = 0;
            meshStats[N_VERTICES] = 0;
            meshStats[N_EDGES] = 0;
            meshStats[N_BORDER_EDGES] = 0;
            
            timeStats[T_TRIANGULATION_GENERATION] = 0.0;
            timeStats[T_LABEL_MAX_EDGES] = 0.0;
            timeStats[T_LABEL_FRONTIER_EDGES] = 0.0;
            timeStats[T_LABEL_SEED_EDGES] = 0.0;
            timeStats[T_TRAVERSAL_AND_REPAIR] = 0.0;
            timeStats[T_TRAVERSAL] = 0.0;
            timeStats[T_REPAIR] = 0.0;
        }
        std::unordered_map<MeshStat,int>& getRefinementStats() override {
            return meshStats;
        }
        std::unordered_map<TimeStat,double>& getRefinementTimes() override {
            return timeStats;
        }
};

#include<mesh_refiners/delaunay_cavity/delaunay_cavity_refiner.ipp>

#endif