#ifndef POLYGON_MERGING_POLICY_CONCEPT_HPP
#define POLYGON_MERGING_POLICY_CONCEPT_HPP
#include <concepts>
#include <concepts/mesh_data.hpp>

template <typename MergingPolicy, MeshData Mesh>
inline constexpr bool isNullMergingPolicy = false;

template <typename MergingPolicy, typename Mesh>
concept PolygonMergingPolicy = isNullMergingPolicy<MergingPolicy,Mesh> ||
    requires(
        Mesh* mesh,
        typename Mesh::OutputIndex seedToMerge,
        std::vector<typename Mesh::OutputIndex>& seedNeighbors,
        std::vector<std::vector<typename Mesh::EdgeIndex>>& sharedEdges
    ) {
        { MergingPolicy::mergeBestCandidate(mesh, seedToMerge, seedNeighbors, sharedEdges)} -> std::convertible_to<typename Mesh::OutputIndex>;
    };

#endif // POLYGON_MERGING_POLICY_CONCEPT_HPP