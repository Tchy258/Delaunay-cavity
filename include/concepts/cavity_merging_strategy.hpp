#ifndef CAVITY_MERGING_STRATEGY_CONCEPT_HPP
#define CAVITY_MERGING_STRATEGY_CONCEPT_HPP
#include<concepts>
#include<concepts/mesh_data.hpp>
#include<vector>
#include<mesh_generators/delaunay_cavity/helper_structs/cavity.hpp>
#include<mesh_generators/delaunay_cavity/helper_structs/delaunay_cavity_data.hpp>


template <typename MergingStrategy, typename Mesh>
concept HasPreAddMethodPerCavity = requires(Mesh* mesh, 
    const std::vector<generators::helpers::delaunay_cavity::Cavity<Mesh>>& cavities,
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
    std::vector<generators::helpers::delaunay_cavity::Cavity<Mesh>>& cavities) {
        { MergingStrategy::postCompute(mesh, cavities)} -> std::same_as<void>;
} && MeshData<Mesh>;

template <typename MergingStrategy, typename Mesh>
concept HasPostInsertionMethod = 
    requires(const Mesh* inputMesh, Mesh* outputMesh,
    DelaunayCavityData<Mesh>& generatorData) {
        { MergingStrategy::postInsertion(inputMesh, outputMesh, generatorData)} -> std::same_as<void>;
} && MeshData<Mesh>;

/**
 * A cavity merging strategy must apply to a particular mesh that conforms to the MeshData concept,
 * and it must have either a preAdd method, postCompute method or postInsertion method
 */
template <typename MergingStrategy, typename Mesh>
concept CavityMergingStrategy = HasPreAddMethodPerCavity<MergingStrategy,Mesh> || HasPostComputeMethod<MergingStrategy,Mesh> || HasPreAddMethodByPresence<MergingStrategy,Mesh> || HasPostInsertionMethod<MergingStrategy, Mesh>;

#endif