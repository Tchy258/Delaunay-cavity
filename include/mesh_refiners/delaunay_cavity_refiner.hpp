#ifndef DELAUNAY_CAVITY_REFINER_HPP
#define DELAUNAY_CAVITY_REFINER_HPP
#include<unordered_map>
#include<unordered_set>
#include<cstdint>
#include<queue>
#include<misc/mesh_stat.hpp>
#include<misc/time_stat.hpp>
#include<concepts/mesh_data.hpp>
#include<concepts/refinement_criterion.hpp>
#include<concepts/is_half_edge_vertex.hpp>
#include<mesh_data/structures/vertex.hpp>
#include<mesh_data/half_edge_mesh.hpp>
#include<mesh_refiners/mesh_refiner.hpp>
#include<mesh_refiners/helpers/mesh_helper.hpp>
#include<stdexcept>
#include<string>
#include<cmath>

template <MeshData MeshType, RefinementCriterion<MeshType> Criterion>
class DelaunayCavityRefiner : public MeshRefiner<MeshType> {
    public:
        using MeshVertex = typename MeshType::VertexType;
        using MeshEdge = typename MeshType::EdgeType;
    private:
        Criterion refinementCriterion;
        using _MeshHelper = refiners::helpers::delaunay_cavity::MeshHelper<MeshType>;
        std::unordered_map<MeshStat, int> meshStats;
        std::unordered_map<TimeStat, double> timeStats;
        std::unordered_map<int, std::vector<typename MeshType::EdgeIndex>> selectCavityEdges(MeshType* outMesh, std::unordered_map<int, std::vector<int>>& cavities) {
            return _MeshHelper::selectCavityEdges(outMesh,cavities);
        }
        /**
         * Calls the mesh specific methods to edit it and form the cavity inside of it.
         * 
         * A mesh helper should implement the logic for insertion on a specific mesh
         * @param outputMesh A mesh, received as a copy of the mesh received in the refineMesh method
         * @param cavityMap A hashmap that maps polygon faces to the edges that the face should keep, so all other edges should be erased.
         * This erasure should be done by changing where other edges point to, and NOT by editing the edges vector
         * @returns A mesh that has cavities according to the `Criterion` used
         */
        void insertCavity(MeshType* outputMesh, std::unordered_map<int, std::vector<typename MeshType::EdgeIndex>>& cavityMap) {
            return _MeshHelper::insertCavity(outputMesh,cavityMap);
        }

        std::vector<std::pair<MeshVertex,int>> findMatchingCircumcenters(MeshType* outputMesh, size_t polygonAmount);
        std::vector<int> computeCavity(MeshType* outputMesh, const std::pair<MeshVertex,int>& circumcenterData, std::vector<uint8_t>& visited);

        inline void resetVisited(std::vector<uint8_t>& visited, const std::vector<int>& cavity) {
            for (int triangle: cavity) {
                visited[triangle] = 0;
            }
        }
    public:
        MeshType* refineMesh(const MeshType* inputMesh) override;
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

};

#include<template_implementations/mesh_refiners/delaunay_cavity_refiner.ipp>

#endif