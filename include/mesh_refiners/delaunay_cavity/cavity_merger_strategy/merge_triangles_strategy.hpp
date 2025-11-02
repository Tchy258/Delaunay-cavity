#ifndef MERGE_TRIANGLES_STRATEGY
#define MERGE_TRIANGLES_STRATEGY
#include <mesh_refiners/delaunay_cavity/cavity_merger_strategy/exclude_previous_cavities_strategy.hpp>
#include <mesh_refiners/delaunay_cavity/cavity_merger_strategy/polygon_merging_policy/polygon_merging_policies.hpp>
#include <concepts/has_merge_method.hpp>

template <MeshData Mesh, PolygonMergingPolicy MergingPolicy>
struct MergeTrianglesStrategyBase : ExcludePreviousCavitiesStrategyBase<Mesh, MergingPolicy> {
    using Base = CavityMergingStrategyBase<Mesh,MergingPolicy>;
    using typename Base::Cavity;
    using typename Base::FaceIndex;
    using typename Base::MeshVertex;
    using typename Base::OutputIndex;
    using typename Base::_MeshHelper;
    static void postInsertion(const Mesh* inputMesh, Mesh* outputMesh, std::vector<OutputIndex>& outputSeeds, std::vector<uint8_t>& inCavity) requires HasMergeIntoNeighborMethod<_MeshHelper,Mesh,MergingPolicy> {
        std::vector<OutputIndex> seedsCopy = std::vector<OutputIndex>{outputSeeds.begin(), outputSeeds.end()};
        for (OutputIndex seed : seedsCopy) {
            if (std::find(outputSeeds.begin(), outputSeeds.end(), seed) != outputSeeds.end() && _MeshHelper::seedPolygonEdgeCount(outputMesh, seed) == 3) {
                _MeshHelper::template mergeIntoNeighbor<MergingPolicy>(inputMesh, outputMesh, outputSeeds, seed);
            }
        }
    }
};

template <MeshData Mesh>
using MergeTrianglesWithBestConvexityStrategy = MergeTrianglesStrategyBase<Mesh, MaximizeConvexityMergingPolicy>;

#endif // MERGE_TRIANGLES_STRATEGY