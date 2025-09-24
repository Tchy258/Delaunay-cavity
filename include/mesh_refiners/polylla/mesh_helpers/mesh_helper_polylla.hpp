#ifndef MESH_HELPER_POLYLLA_HPP
#define MESH_HELPER_POLYLLA_HPP
#include <concepts/mesh_data.hpp>
#include <mesh_refiners/polylla/polylla_refiner.hpp>
#include <chrono>

namespace refiners::helpers::polylla {

    template <MeshData MeshType>
    struct MeshHelper {
        using MeshVertex = typename MeshType::VertexType;
        using MeshEdge = typename MeshType::EdgeType;
        using VertexIndex = typename MeshType::VertexIndex;
        using EdgeIndex = typename MeshType::EdgeIndex;
        using FaceIndex = typename MeshType::FaceIndex;
        using OutputIndex = typename MeshType::OutputIndex;
        // TODO: add methods required for a generic polylla and remember to = delete; them

        static void labelMaxEdges(PolyllaRefiner<MeshType>& refiner, MeshType* mesh) = delete;

        static EdgeIndex findMaxEdge(PolyllaRefiner<MeshType>& refiner, MeshType* mesh, FaceIndex face) = delete;

        static void labelFrontierEdges(PolyllaRefiner<MeshType>& refiner, MeshType* mesh) = delete;

        static bool isFrontierEdge(PolyllaRefiner<MeshType>& refiner, MeshType* mesh, EdgeIndex edge) = delete;
    };
}

#include <mesh_refiners/polylla/mesh_helpers/mesh_helper_half_edge_polylla.hpp>

#endif // MESH_HELPER_POLYLLA_HPP