#ifndef MESH_TOPOLOGY_HPP
#define MESH_TOPOLOGY_HPP
#include <concepts>
#include <vector>
#include <array>

/**
 * Topology operations for a Mesh type such as getting the neighboring triangles of a triangle, the edges of triangle and other queries
 */
template<typename Mesh>
concept MeshTopology =
    requires(Mesh& mesh,
             const Mesh& cmesh,
             typename Mesh::VertexType v,
             typename Mesh::EdgeIndex e,
             typename Mesh::FaceIndex f,
             typename Mesh::OutputIndex out) {
        { cmesh.getNeighbors(f) } -> std::same_as<std::vector<typename Mesh::FaceIndex>>;
        { cmesh.getVerticesOfTriangle(f, v, v, v)} -> std::same_as<void>;
        { cmesh.getEdgesOfTriangle(f) } -> std::same_as<std::array<typename Mesh::EdgeIndex,3>>;
        { cmesh.isBorderEdge(e) } -> std::same_as<bool>;
        { cmesh.getSharedEdge(f, f) } -> std::same_as<typename Mesh::EdgeIndex>;
        { cmesh.getFacesAssociatedWithEdge(e)} -> std::same_as<std::pair<typename Mesh::FaceIndex, typename Mesh::FaceIndex>>;
        { cmesh.edgeLength2(e) } -> std::same_as<double>;
        { cmesh.isPolygonConvex(out)} -> std::convertible_to<bool>;
    };

#endif // MESH_TOPOLOGY_HPP