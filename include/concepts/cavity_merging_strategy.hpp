#ifndef CAVITY_MERGING_STRATEGY
#define CAVITY_MERGING_STRATEGY
#include<concepts>
#include<concepts/mesh_data.hpp>
#include<vector>
#include<mesh_refiners/delaunay_cavity/helper_structs/cavity.hpp>

template <typename MergingStrategy, typename Mesh>
concept HasPreAddMethod = requires(Mesh* mesh, 
    const std::vector<refiners::helpers::delaunay_cavity::Cavity<Mesh>>& cavities,
    typename Mesh::FaceIndex triangle) {
        { MergingStrategy::preAdd(mesh, triangle, cavities)} -> std::convertible_to<bool>;
} && MeshData<Mesh>;

template <typename MergingStrategy, typename Mesh>
concept HasPostComputeMethod = requires(Mesh* mesh,
    std::vector<refiners::helpers::delaunay_cavity::Cavity<Mesh>>& cavities) {
        { MergingStrategy::postCompute(mesh, cavities)} -> std::same_as<void>;
} && MeshData<Mesh>;

/**
 * A cavity merging strategy must apply to a particular mesh that conforms to the MeshData concept,
 * and it must have either a preAdd method or postCompute method
 */
template <typename MergingStrategy, typename Mesh>
concept CavityMergingStrategy = HasPreAddMethod<MergingStrategy,Mesh> || HasPostComputeMethod<MergingStrategy,Mesh>;

#endif