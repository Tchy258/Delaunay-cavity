#ifndef MAXIMIZE_CONVEXITY_MERGING_POLICY_HPP
#define MAXIMIZE_CONVEXITY_MERGING_POLICY_HPP
#include <concepts/mesh_data.hpp>
#include <concepts/polygon_merging_policy_concept.hpp>
#include <mesh_data/half_edge_mesh.hpp>

template <MeshData Mesh>
struct MaximizeConvexityMergingPolicy {
    using EdgeIndex = typename Mesh::EdgeIndex;
    using OutputIndex = typename Mesh::OutputIndex;
    using ConnectivityBackupT = typename Mesh::ConnectivityBackup;
    static OutputIndex mergeBestCandidate(Mesh* mesh, OutputIndex seedToMerge, std::vector<OutputIndex>& seedNeighbors, std::vector<std::vector<EdgeIndex>>& sharedEdges) {
        for (int i = 0; i < seedNeighbors.size(); ++i) {
            std::vector<ConnectivityBackupT> backup = mesh->mergeSeeds(seedNeighbors[i], std::pair<OutputIndex, std::vector<EdgeIndex>>{seedToMerge, sharedEdges[i]});
            if (mesh->isPolygonConvex(seedNeighbors[i])) {
                return seedNeighbors[i];
            } else {
                mesh->rollbackMerge(backup);
            }
        }
        return -1;
    }
};

static_assert(PolygonMergingPolicy<MaximizeConvexityMergingPolicy<HalfEdgeMesh>, HalfEdgeMesh>);

#endif // MAXIMIZE_CONVEXITY_MERGING_POLICY_HPP