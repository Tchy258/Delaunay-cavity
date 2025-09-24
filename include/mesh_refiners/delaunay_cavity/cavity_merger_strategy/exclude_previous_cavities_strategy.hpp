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
    static bool preAdd(Mesh* mesh, FaceIndex triangle, const std::vector<Cavity>& cavities) {
        auto inCavity = [&](const Cavity& cavity) { return std::find(cavity.allTriangles.begin(), cavity.allTriangles.end(), triangle) != cavity.allTriangles.end(); };
        return std::find_if(cavities.begin(), cavities.end(), inCavity) == cavities.end();
    }
};


#endif