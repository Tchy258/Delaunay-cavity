#ifndef POLYGON_MERGING_POLICY_CONCEPT_HPP
#define POLYGON_MERGING_POLICY_CONCEPT_HPP
#include <concepts>
#include <concepts/mesh_data.hpp>

template <typename MergingPolicy>
inline constexpr bool isKnownMergingPolicy = false;

template <typename MergingPolicy>
concept PolygonMergingPolicy = isKnownMergingPolicy<MergingPolicy>;

#endif // POLYGON_MERGING_POLICY_CONCEPT_HPP