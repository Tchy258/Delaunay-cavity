#ifndef NULL_POLYGON_MERGING_POLICY_HPP
#define NULL_POLYGON_MERGING_POLICY_HPP
#include <concepts/polygon_merging_policy_concept.hpp>

template <MeshData Mesh>
struct NullPolygonMergingPolicy {};

template <MeshData Mesh>
inline constexpr bool isNullMergingPolicy<NullPolygonMergingPolicy<Mesh>,Mesh> = true;

#endif // NULL_POLYGON_MERGING_POLICY_HPP