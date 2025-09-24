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
             typename Mesh::VertexType v,
             typename Mesh::EdgeIndex e,
             typename Mesh::FaceIndex f) {
        { mesh.getNeighbors(f) } -> std::same_as<std::vector<typename Mesh::FaceIndex>>;
        { mesh.getVerticesOfTriangle(f, v, v, v)} -> std::same_as<void>;
        { mesh.getEdgesOfTriangle(f) } -> std::same_as<std::array<typename Mesh::EdgeIndex,3>>;
        { mesh.isBorderEdge(e) } -> std::same_as<bool>;
        { mesh.getSharedEdge(f, f) } -> std::same_as<typename Mesh::EdgeIndex>;
        { mesh.getFacesAssociatedWithEdge(e)} -> std::same_as<std::pair<typename Mesh::FaceIndex, typename Mesh::FaceIndex>>;
        { mesh.edgeLength2(e) } -> std::same_as<double>;
    };

#endif // MESH_TOPOLOGY_HPP