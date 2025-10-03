#ifndef EXCLUDE_PREVIOUS_CAVITY_STRATEGY_HPP
#define EXCLUDE_PREVIOUS_CAVITY_STRATEGY_HPP

#include <concepts/mesh_data.hpp>
#include <mesh_refiners/delaunay_cavity/helper_structs/cavity.hpp>
#include <vector>
#include <concepts/cavity_merging_strategy.hpp>

template <MeshData Mesh>
struct ExcludePreviousCavitiesStrategy {
    using Cavity = refiners::helpers::delaunay_cavity::Cavity<Mesh>;
    using FaceIndex = typename Mesh::FaceIndex;
    using MeshVertex = typename Mesh::VertexType;
    static bool preAdd(FaceIndex triangle, const std::vector<uint8_t>& inCavity) {
        return !inCavity[triangle];
    }
};


#endif