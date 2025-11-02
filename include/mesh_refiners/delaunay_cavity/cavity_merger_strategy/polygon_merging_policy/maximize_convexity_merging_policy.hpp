#ifndef MAXIMIZE_CONVEXITY_MERGING_POLICY_HPP
#define MAXIMIZE_CONVEXITY_MERGING_POLICY_HPP
#include <concepts/polygon_merging_policy_concept.hpp>


struct MaximizeConvexityMergingPolicy {};

template<>
inline constexpr bool isKnownMergingPolicy<MaximizeConvexityMergingPolicy> = true;

#endif // MAXIMIZE_CONVEXITY_MERGING_POLICY_HPP