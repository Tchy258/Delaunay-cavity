#ifndef CAVITY_MERGING_STRATEGY_BASE_HPP
#define CAVITY_MERGING_STRATEGY_BASE_HPP
#include <concepts/mesh_data.hpp>
#include <concepts/polygon_merging_policy_concept.hpp>
#include <mesh_refiners/delaunay_cavity/helper_structs/cavity.hpp>
#include <vector>
#include <mesh_refiners/delaunay_cavity/mesh_helpers/mesh_helper_delaunay_cavity.hpp>

template <MeshData Mesh, PolygonMergingPolicy<Mesh> MergingPolicy>
struct CavityMergingStrategyBase {
    using Cavity = refiners::helpers::delaunay_cavity::Cavity<Mesh>;
    using FaceIndex = typename Mesh::FaceIndex;
    using MeshVertex = typename Mesh::VertexType;
    using OutputIndex = typename Mesh::OutputIndex;
    protected:
        using _MeshHelper = refiners::helpers::delaunay_cavity::MeshHelper<Mesh>;
};

#endif