#ifndef MESH_HELPER_HALF_EDGE_POLYLLA_HPP
#define MESH_HELPER_HALF_EDGE_POLYLLA_HPP
#ifndef MESH_HELPER_POLYLLA_HPP
#include <mesh_refiners/polylla/mesh_helpers/mesh_helper_polylla.hpp>
#endif

#include <mesh_data/half_edge_mesh.hpp>

namespace refiners::helpers::polylla {

    template <>
    struct MeshHelper<HalfEdgeMesh> {
        using MeshVertex = HalfEdgeMesh::VertexType;
        using MeshEdge = HalfEdgeMesh::EdgeType;
        using VertexIndex = HalfEdgeMesh::VertexIndex;
        using EdgeIndex = HalfEdgeMesh::EdgeIndex;
        using FaceIndex = HalfEdgeMesh::FaceIndex;
        using OutputIndex = HalfEdgeMesh::OutputIndex;
        // TODO: add methods required for a generic polylla
        static void labelMaxEdges(PolyllaRefiner<HalfEdgeMesh>& refiner, HalfEdgeMesh* mesh);

        static EdgeIndex findMaxEdge(PolyllaRefiner<HalfEdgeMesh>& refiner, HalfEdgeMesh* mesh, FaceIndex face);

        static void labelFrontierEdges(PolyllaRefiner<HalfEdgeMesh>& refiner, HalfEdgeMesh* mesh);

        static bool isFrontierEdge(PolyllaRefiner<HalfEdgeMesh>& refiner, HalfEdgeMesh* mesh, EdgeIndex edge);
    };
}
#endif // MESH_HELPER_HALF_EDGE_POLYLLA_HPP