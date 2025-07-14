#ifndef DELAUNAY_CAVITY_REFINER_HPP
#define DELAUNAY_CAVITY_REFINER_HPP
#include<unordered_map>
#include<unordered_set>
#include<misc/mesh_stat.hpp>
#include<misc/time_stat.hpp>
#include<concepts/mesh_data.hpp>
#include<concepts/refinement_criterion.hpp>
#include<concepts/is_half_edge_vertex.hpp>
#include<mesh_data/structures/vertex.hpp>
#include<mesh_data/half_edge_mesh.hpp>
#include<mesh_refiners/mesh_refiner.hpp>
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
        std::unordered_map<MeshStat, int> meshStats;
        std::unordered_map<TimeStat, double> timeStats;
        std::unordered_map<int, std::unordered_set<HalfEdgeMesh::EdgeIndex>> selectCavityEdges(HalfEdgeMesh& outMesh, std::unordered_map<int, std::vector<int>>& cavities) requires std::same_as<MeshType, HalfEdgeMesh>;
        /**
         * Calls the mesh specific methods to edit it and form the cavity inside of it.
         * This method is specific for half edge based meshes
         * 
         * Different overloads should be defined for different mesh types with the appropriate `requires`
         * @param outputMesh A half edge based mesh, received as a copy of the mesh received in the refineMesh method
         * @param cavityMap A hashmap that maps polygon faces to the edges that the face should keep, so all other edges should be erased.
         * This erasure should be done by changing where other edges point to, and NOT by editing the edges vector
         * @returns A half edge based mesh that has cavities according to the `Criterion` used
         */
        void insertCavity(HalfEdgeMesh& outputMesh, std::unordered_map<int, std::unordered_set<HalfEdgeMesh::EdgeIndex>>& cavityMap) requires std::same_as<MeshType, HalfEdgeMesh>;
    public:
        MeshType refineMesh(const MeshType& inputMesh) override;
        explicit DelaunayCavityRefiner(Criterion criterion) : refinementCriterion(std::move(criterion)) {}

};

#include<template_implementations/mesh_refiners/delaunay_cavity_refiner.ipp>

#endif