#ifndef DELAUNAY_CAVITY_REFINER_HPP
#define DELAUNAY_CAVITY_REFINER_HPP
#include<unordered_map>
#include<misc/mesh_stat.hpp>
#include<misc/time_stat.hpp>
#include<concepts/mesh_data.hpp>
#include<concepts/refinement_criterion.hpp>
#include<concepts/is_half_edge_vertex.hpp>
#include<mesh_data/structures/vertex.hpp>
#include<mesh_refiners/mesh_refiner.hpp>
#include<stdexcept>
#include<string>
#include<cmath>

template <MeshData MeshType, RefinementCriterion<MeshType> Criterion>
class DelaunayCavityRefiner : public MeshRefiner<MeshType> {
    public:
        using MeshVertex = typename Mesh::VertexType;
        using MeshEdge = typename Mesh::EdgeType;
    private:
        Criterion refinementCriterion;
        std::unordered_map<MeshStat, int> meshStats;
        std::unordered_map<TimeStat, double> timeStats;
        MeshVertex findCircumcenter(MeshVertex vertex0, MeshVertex vertex1, MeshVertex vertex2);
    public:
        MeshType refineMesh(const MeshType& inputMesh) override;
        explicit DelaunayCavityRefiner(Criterion criterion) : refinementCriterion(std::move(criterion)) {}

};

#include<template_implementations/mesh_refiners/delaunay_cavity_refiner.ipp>

#endif