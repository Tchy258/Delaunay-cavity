#ifndef POLYLLA_REFINER_HPP
#define POLYLLA_REFINER_HPP
#include<concepts/mesh_data.hpp>
#include<mesh_refiners/mesh_refiner.hpp>
#include<mesh_refiners/polylla/mesh_helpers/mesh_helper_polylla.hpp>
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
        using _polygon = std::vector<int>; 
        using BinaryVector = std::vector<uint8_t>;
        using _MeshHelper = refiners::helpers::polylla::MeshHelper<MeshType>;

        std::unordered_map<MeshStat, int> meshStats;
        std::unordered_map<TimeStat, double> timeStats;
        std::vector<OutputIndex> outputSeeds; //Seeds of the polygon

        //std::vector<int> triangles; //True if the edge generated a triangle CHANGE!!!!

        BinaryVector maxEdges; //True if the edge i is a max edge
        BinaryVector frontierEdges; //True if the edge i is a frontier edge
        std::vector<OutputIndex> seedOutputs; //Seed edges that generate polygon simple and non-simple

        // Auxiliary array used during the barrier-edge elimination
        std::vector<FaceIndex> triangleList;
        BinaryVector seed_bet_mark;

        friend class refiners::helpers::polylla::MeshHelper<MeshType>;
    public:
        PolyllaRefiner() {
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

        MeshType* refineMesh(const MeshType* inputMesh) override;
};

#include <mesh_refiners/polylla/polylla_refiner.ipp>

#endif