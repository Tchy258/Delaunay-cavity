#ifndef HAS_MERGE_METHOD_CONCEPT_HPP
#define HAS_MERGE_METHOD_CONCEPT_HPP
#include <concepts>
#include <concepts/mesh_data.hpp>
#include <concepts/polygon_merging_policy_concept.hpp>

template <typename MeshHelper, typename Mesh, typename MergingPolicy>
concept HasMergeIntoNeighborMethod = 
    PolygonMergingPolicy<MergingPolicy> &&
    requires(const Mesh* inputMesh, Mesh* outputMesh,
             std::vector<typename Mesh::OutputIndex>& outputSeeds,
             typename Mesh::OutputIndex seedToMerge) {
        { MeshHelper::template mergeIntoNeighbor<MergingPolicy>(inputMesh, outputMesh, outputSeeds, seedToMerge) } -> std::same_as<void>;
    } && MeshData<Mesh>;



#endif