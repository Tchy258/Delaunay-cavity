#ifndef EDGE_LENGTH_BASED_MERGING_POLICY
#define EDGE_LENGTH_BASED_MERGING_POLICY
#include <concepts/mesh_data.hpp>
#include <limits>

template <MeshData Mesh, typename Derived>
struct EdgeLengthBasedMergingPolicy {
    using OutputIndex = typename Mesh::OutputIndex;
    using EdgeIndex = typename Mesh::EdgeIndex;
    private:
        static bool isBetterCandidate(double currentCandidateLength, double newCandidateLength) {
            return Derived::isBetterCandidateImpl(currentCandidateLength, newCandidateLength);
        }
        static double initialValue() {
            return Derived::initialValueImpl();
        }
    public:
        static OutputIndex mergeBestCandidate(Mesh* mesh, OutputIndex seedToMerge, std::vector<OutputIndex>& seedNeighbors, std::vector<std::vector<EdgeIndex>>& sharedEdges) {
            int bestCandidateIndex = 0;
            double bestCandidateLength = initialValue();
            for (int i = 0; i < seedNeighbors.size(); ++i) {
                for (EdgeIndex edge : sharedEdges[i]) {
                    double newCandidateLength = mesh->edgeLength2(edge);
                    bestCandidateIndex = isBetterCandidate(bestCandidateLength, newCandidateLength) ? i : bestCandidateIndex;
                    if (bestCandidateIndex == i) {
                        bestCandidateLength = newCandidateLength;
                    }
                }
            }
            mesh->mergeSeeds(seedNeighbors[bestCandidateIndex], std::pair<OutputIndex, std::vector<EdgeIndex>>{seedToMerge, sharedEdges[bestCandidateIndex]});
            return seedNeighbors[bestCandidateIndex];
        }
};


#endif