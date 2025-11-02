#ifndef CAVITY_MERGING_STRATEGY_CONCEPT_HPP
#define CAVITY_MERGING_STRATEGY_CONCEPT_HPP
#include<concepts>
#include<concepts/mesh_data.hpp>
#include<vector>
#include<mesh_refiners/delaunay_cavity/helper_structs/cavity.hpp>
#include<concepts/has_merge_method.hpp>


template <typename MergingStrategy, typename Mesh>
concept HasPreAddMethodPerCavity = requires(Mesh* mesh, 
    const std::vector<refiners::helpers::delaunay_cavity::Cavity<Mesh>>& cavities,
    typename Mesh::FaceIndex triangle) {
        { MergingStrategy::preAdd(mesh, triangle, cavities)} -> std::convertible_to<bool>;
} && MeshData<Mesh>;

template <typename MergingStrategy, typename Mesh>
concept HasPreAddMethodByPresence = requires(Mesh* mesh, 
    const std::vector<uint8_t>& inCavity,
    typename Mesh::FaceIndex triangle) {
        { MergingStrategy::preAdd(triangle, inCavity)} -> std::convertible_to<bool>;
} && MeshData<Mesh>;

template <typename MergingStrategy, typename Mesh>
concept HasPostComputeMethod = requires(Mesh* mesh,
    std::vector<refiners::helpers::delaunay_cavity::Cavity<Mesh>>& cavities) {
        { MergingStrategy::postCompute(mesh, cavities)} -> std::same_as<void>;
} && MeshData<Mesh>;

template <typename MergingStrategy, typename Mesh>
concept HasPostInsertionMethod = 
    requires(const Mesh* inputMesh, Mesh* outputMesh,
    std::vector<typename Mesh::OutputIndex>& outputSeeds,
    std::vector<uint8_t>& inCavity) {
        { MergingStrategy::postInsertion(inputMesh, outputMesh, outputSeeds, inCavity)} -> std::same_as<void>;
} && MeshData<Mesh>;

/**
 * A cavity merging strategy must apply to a particular mesh that conforms to the MeshData concept,
 * and it must have either a preAdd method or postCompute method
 */
template <typename MergingStrategy, typename Mesh>
concept CavityMergingStrategy = HasPreAddMethodPerCavity<MergingStrategy,Mesh> || HasPostComputeMethod<MergingStrategy,Mesh> || HasPreAddMethodByPresence<MergingStrategy,Mesh> || HasPostInsertionMethod<MergingStrategy, Mesh>;

#endif