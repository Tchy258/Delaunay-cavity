#ifndef EXCLUDE_PREVIOUS_CAVITY_STRATEGY_HPP
#define EXCLUDE_PREVIOUS_CAVITY_STRATEGY_HPP
#include <mesh_generators/delaunay_cavity/cavity_merger_strategy/cavity_merging_strategy_base.hpp>
#include <mesh_generators/delaunay_cavity/cavity_merger_strategy/polygon_merging_policy/null_polygon_merging_policy.hpp>

template <MeshData Mesh, PolygonMergingPolicy<Mesh> MergingPolicy>
struct ExcludePreviousCavitiesStrategyBase : CavityMergingStrategyBase<Mesh, MergingPolicy> {
    using Base = CavityMergingStrategyBase<Mesh,MergingPolicy>;
    using typename Base::Cavity;
    using typename Base::FaceIndex;
    using typename Base::MeshVertex;
    using typename Base::OutputIndex;
    using typename Base::_MeshHelper;
    static bool preAdd(FaceIndex triangle, const std::vector<uint8_t>& inCavity) {
        return !inCavity[triangle];
    }
};

template <MeshData Mesh>
using ExcludePreviousCavitiesStrategy = ExcludePreviousCavitiesStrategyBase<Mesh, NullPolygonMergingPolicy<Mesh>>;


#endif