#ifndef BIGGEST_NEIGHBOR_MERGING_POLICY_HPP
#define BIGGEST_NEIGHBOR_MERGING_POLICY_HPP
#include <mesh_refiners/delaunay_cavity/cavity_merger_strategy/polygon_merging_policy/size_based_neighbor_merging_policy.hpp>

template <MeshData Mesh>
struct BiggestNeighborMergingPolicy : public SizeBasedNeighborMergingPolicy<Mesh,BiggestNeighborMergingPolicy<Mesh>> {
    static bool isBetterCandidateImpl(size_t currentCandidateSize, size_t newCandidateSize) {
        return newCandidateSize > currentCandidateSize;
    }
};


#endif