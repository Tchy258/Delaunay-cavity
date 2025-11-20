#ifndef MERGE_TRIANGLES_STRATEGY
#define MERGE_TRIANGLES_STRATEGY
#include <mesh_refiners/delaunay_cavity/cavity_merger_strategy/exclude_previous_cavities_strategy.hpp>
#include <mesh_refiners/delaunay_cavity/cavity_merger_strategy/polygon_merging_policy/polygon_merging_policies.hpp>
#include <concepts/cavity_merging_strategy.hpp>
#include <mesh_data/half_edge_mesh.hpp>
#include <mesh_refiners/delaunay_cavity/helper_structs/delaunay_cavity_data.hpp>
template <MeshData Mesh, PolygonMergingPolicy<Mesh> MergingPolicy>
struct MergeTrianglesStrategyBase : ExcludePreviousCavitiesStrategyBase<Mesh, MergingPolicy> {
    using Base = CavityMergingStrategyBase<Mesh,MergingPolicy>;
    using typename Base::Cavity;
    using typename Base::FaceIndex;
    using typename Base::MeshVertex;
    using typename Base::OutputIndex;
    using typename Base::_MeshHelper;
    static void postInsertion(const Mesh* inputMesh, Mesh* outputMesh, DelaunayCavityData<Mesh>& refinerData) {
        size_t seedAmount = refinerData.outputSeeds.size();
        std::unordered_map<typename Mesh::EdgeIndex, OutputIndex> edgeToOutputMap = _MeshHelper::buildEdgeToOutputMap(outputMesh, refinerData.outputSeeds);
        for (size_t i = 0; i < seedAmount; ++i) {
            if (refinerData.isValidSeed(i) && outputMesh->getOutputSeedEdgeCount(refinerData.outputSeeds[i]) == 3) {
                _MeshHelper::template mergeIntoNeighbor<MergingPolicy>(inputMesh, outputMesh, refinerData.outputSeeds, refinerData.outputSeeds[i], edgeToOutputMap);
            }
        }
        refinerData.removeInvalidSeeds();
        // Source - https://stackoverflow.com/a/25438497
        // Posted by Tony Delroy, modified by community. See post 'Timeline' for change history
        // Retrieved 2025-11-20, License - CC BY-SA 4.0
        refinerData.memoryStats[M_EDGE_MAP] =  (edgeToOutputMap.size() * (sizeof(OutputIndex) + sizeof(void*)) + // data list
        edgeToOutputMap.bucket_count() * (sizeof(void*) + sizeof(size_t))) // bucket index
        * 1.5; // estimated allocation overheads

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