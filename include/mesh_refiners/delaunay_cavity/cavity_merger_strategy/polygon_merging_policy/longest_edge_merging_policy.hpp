#ifndef LONGEST_EDGE_MERGING_POLICY
#define LONGEST_EDGE_MERGING_POLICY
#include <mesh_refiners/delaunay_cavity/cavity_merger_strategy/polygon_merging_policy/edge_length_based_merging_policy.hpp>

template <MeshData Mesh>
struct LongestEdgeMergingPolicy : public EdgeLengthBasedMergingPolicy<Mesh,LongestEdgeMergingPolicy<Mesh>> {
    static bool isBetterCandidateImpl(double currentCandidateLength, double newCandidateLength) {
        return newCandidateLength > currentCandidateLength;
    }
    static double initialValueImpl() {
        return std::numeric_limits<double>::lowest();
    }
};


#endif