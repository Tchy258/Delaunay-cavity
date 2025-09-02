#ifndef POLYLLA_REFINER_HPP
#define POLYLLA_REFINER_HPP
#include<concepts/mesh_data.hpp>
#include<mesh_refiners/mesh_refiner.hpp>
#include<mesh_refiners/helpers/mesh_helper.hpp>
#include<misc/mesh_stat.hpp>
#include<misc/time_stat.hpp>

template <MeshData MeshType>
class PolyllaRefiner : public MeshRefiner<MeshType> {
    public:
        using MeshVertex = typename MeshType::VertexType;
        using MeshEdge = typename MeshType::EdgeType;
    private:
        using _polygon = std::vector<int>; 
        using bit_vector = std::vector<uint8_t>;
        using _MeshHelper = refiners::helpers::polylla::MeshHelper<MeshData>;

        std::unordered_map<MeshStat, int> meshStats;
        std::unordered_map<TimeStat, double> timeStats;
        std::vector<int> outputSeeds; //Seeds of the polygon

        //std::vector<int> triangles; //True if the edge generated a triangle CHANGE!!!!

        bit_vector max_edges; //True if the edge i is a max edge
        bit_vector frontier_edges; //True if the edge i is a frontier edge
        std::vector<int> seed_edges; //Seed edges that generate polygon simple and non-simple

        // Auxiliary array used during the barrier-edge elimination
        std::vector<int> triangle_list;
        bit_vector seed_bet_mark;

        friend class refiners::helpers::polylla::MeshHelper<MeshData>;
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

};

#endif