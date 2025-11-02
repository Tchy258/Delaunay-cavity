#ifndef NULL_POLYGON_MERGING_POLICY_HPP
#define NULL_POLYGON_MERGING_POLICY_HPP
#include <concepts/polygon_merging_policy_concept.hpp>

struct NullPolygonMergingPolicy {};

template <>
inline constexpr bool isKnownMergingPolicy<NullPolygonMergingPolicy> = true;

#endif // NULL_POLYGON_MERGING_POLICY_HPP