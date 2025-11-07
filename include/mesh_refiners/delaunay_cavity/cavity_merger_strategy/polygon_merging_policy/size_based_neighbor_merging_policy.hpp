#ifndef SIZE_BASED_NEIGHBOR_MERGING_POLICY_HPP
#define SIZE_BASED_NEIGHBOR_MERGING_POLICY_HPP
#include <concepts/mesh_data.hpp>

template <MeshData Mesh, typename Derived>
struct SizeBasedNeighborMergingPolicy {
    using OutputIndex = typename Mesh::OutputIndex;
    using EdgeIndex = typename Mesh::EdgeIndex;
    private:
        static bool isBetterCandidate(size_t currentCandidateSize, size_t newCandidateSize) {
            return Derived::isBetterCandidateImpl(currentCandidateSize, newCandidateSize);
        }
    public:
        static OutputIndex mergeBestCandidate(Mesh* mesh, OutputIndex seedToMerge, std::vector<OutputIndex>& seedNeighbors, std::vector<std::vector<EdgeIndex>>& sharedEdges) {
            int bestCandidateIndex = 0;
            size_t bestCandidateSize = mesh->getOutputSeedEdgeCount(seedNeighbors[0]);
            for (int i = 1; i < seedNeighbors.size(); ++i) {
                size_t newSize = mesh->getOutputSeedEdgeCount(seedNeighbors[i]);
                bestCandidateIndex = isBetterCandidate(bestCandidateSize, newSize) ? i : bestCandidateIndex;
                if (bestCandidateIndex == i) {
                    bestCandidateSize = newSize;
                }
            }
            mesh->mergeSeeds(seedNeighbors[bestCandidateIndex], std::pair<OutputIndex, std::vector<EdgeIndex>>{seedToMerge, sharedEdges[bestCandidateIndex]});
            return seedNeighbors[bestCandidateIndex];
        }
};

#endif // SIZE_BASED_NEIGHBOR_MERGING_POLICY_HPP