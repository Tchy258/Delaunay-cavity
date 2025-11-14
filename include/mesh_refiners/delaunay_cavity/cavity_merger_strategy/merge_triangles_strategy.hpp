#ifndef MERGE_TRIANGLES_STRATEGY
#define MERGE_TRIANGLES_STRATEGY
#include <mesh_refiners/delaunay_cavity/cavity_merger_strategy/exclude_previous_cavities_strategy.hpp>
#include <mesh_refiners/delaunay_cavity/cavity_merger_strategy/polygon_merging_policy/polygon_merging_policies.hpp>
#include <concepts/cavity_merging_strategy.hpp>
#include <mesh_data/half_edge_mesh.hpp>
template <MeshData Mesh, PolygonMergingPolicy<Mesh> MergingPolicy>
struct MergeTrianglesStrategyBase : ExcludePreviousCavitiesStrategyBase<Mesh, MergingPolicy> {
    using Base = CavityMergingStrategyBase<Mesh,MergingPolicy>;
    using typename Base::Cavity;
    using typename Base::FaceIndex;
    using typename Base::MeshVertex;
    using typename Base::OutputIndex;
    using typename Base::_MeshHelper;
    static void postInsertion(const Mesh* inputMesh, Mesh* outputMesh, std::vector<OutputIndex>& outputSeeds, std::vector<uint8_t>& inCavity) {
        size_t seedAmount = outputSeeds.size();
        std::unordered_map<typename Mesh::EdgeIndex, OutputIndex> edgeToOutputMap = _MeshHelper::buildEdgeToOutputMap(outputMesh, outputSeeds);
        for (size_t i = 0; i < seedAmount; ++i) {
            if (outputSeeds[i] != -1 && outputMesh->getOutputSeedEdgeCount(outputSeeds[i]) == 3) {
                _MeshHelper::template mergeIntoNeighbor<MergingPolicy>(inputMesh, outputMesh, outputSeeds, outputSeeds[i], edgeToOutputMap);
            }
        }
        outputSeeds.erase(std::remove(outputSeeds.begin(), outputSeeds.end(), -1), outputSeeds.end());
    }
};

template <MeshData Mesh>
using MergeTrianglesWithBestConvexityStrategy = MergeTrianglesStrategyBase<Mesh, MaximizeConvexityMergingPolicy<Mesh>>;

template <MeshData Mesh>
using MergeTrianglesIntoSmallestNeighbor = MergeTrianglesStrategyBase<Mesh, SmallestNeighborMergingPolicy<Mesh>>;

template <MeshData Mesh>
using MergeTrianglesIntoBiggestNeighbor = MergeTrianglesStrategyBase<Mesh, BiggestNeighborMergingPolicy<Mesh>>;

template <MeshData Mesh>
using MergeTrianglesThroughLongestEdge = MergeTrianglesStrategyBase<Mesh, LongestEdgeMergingPolicy<Mesh>>;

template <MeshData Mesh>
using MergeTrianglesThroughShortestEdge = MergeTrianglesStrategyBase<Mesh, ShortestEdgeMergingPolicy<Mesh>>;

#endif // MERGE_TRIANGLES_STRATEGY