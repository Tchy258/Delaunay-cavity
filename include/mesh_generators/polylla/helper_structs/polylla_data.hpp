#ifndef POLYLLA_DATA_HPP
#define POLYLLA_DATA_HPP

#include <mesh_generators/mesh_generator_data.hpp>
#include <unordered_map>
#include <vector>
#include <concepts/mesh_data.hpp>

template <MeshData MeshType>
struct PolyllaData : public MeshGeneratorData {
    using OutputIndex = typename MeshType::OutputIndex;
    using BinaryVector = std::vector<uint8_t>;
    int polygonToRepairAccumulator = 0;
    int addedPolygonAfterRepairAccumulator = 0;
    int barrierEdgeTipAccumulator = 0;
    int frontierEdgeAccumulator = 0;
    size_t triangleListMaxSize = 0;
    std::vector<OutputIndex> outputSeeds; //Seeds of the polygon

    //std::vector<int> triangles; //True if the edge generated a triangle CHANGE!!!!

    BinaryVector maxEdges; //True if the edge i is a max edge
    BinaryVector frontierEdges; //True if the edge i is a frontier edge
    std::vector<OutputIndex> seedCandidates; //Seed edges that generate polygon simple and non-simple

    BinaryVector seedBarrierEdgeTipMark;

    PolyllaData() {

        meshStats[N_FRONTIER_EDGES] = 0;
        meshStats[N_BARRIER_EDGE_TIPS] = 0;
        meshStats[N_POLYGONS_TO_REPAIR] = 0;
        meshStats[N_POLYGONS_ADDED_AFTER_REPAIR] = 0;
        meshStats[N_BORDER_EDGES] = 0;

        timeStats[T_LABEL_MAX_EDGES] = 0.0;
        timeStats[T_LABEL_FRONTIER_EDGES] = 0.0;
        timeStats[T_LABEL_SEED_EDGES] = 0.0;
        timeStats[T_TRAVERSAL_AND_REPAIR] = 0.0;
        timeStats[T_TRAVERSAL] = 0.0;
        timeStats[T_REPAIR] = 0.0;
        
        memoryStats[M_MAX_EDGES] = 0;
        memoryStats[M_FRONTIER_EDGES] = 0;
        memoryStats[M_SEED_EDGES] = 0;
        memoryStats[M_SEED_BARRIER_EDGE_TIP_MARK] = 0;
        memoryStats[M_TRIANGLE_LIST] = 0;

    }
};

#endif // POLYLLA_DATA_HPP