#ifndef MESH_DATA_HPP
#define MESH_DATA_HPP

#include<concepts>
#include<vector>
#include<mesh_data/structures/vertex.hpp>

template<typename Mesh>
concept MeshData = requires(
    Mesh& mesh,
    std::vector<typename Mesh::VertexType> vertexVec,
    std::vector<typename Mesh::EdgeType> edgeVec,
    std::vector<int> faceVec,
    typename Mesh::VertexType vertexT,
    typename Mesh::EdgeType edgeT,
    typename Mesh::VertexIndex,
    typename Mesh::EdgeIndex
) {
    { mesh.getVertex(1) } -> std::same_as<typename Mesh::VertexType&>;
    { mesh.getEdge(1) } -> std::same_as<typename Mesh::EdgeType&>;
    { mesh.getPolygon(1) } -> std::same_as<int>;
    { mesh.numberOfVertices()} -> std::same_as<size_t>;
    { mesh.numberOfEdges()} -> std::same_as<size_t>;
    { mesh.numberOfPolygons()} -> std::same_as<size_t>;
    { mesh.getNeighbors(1)} -> std::same_as<std::vector<int>>;
    Mesh(vertexVec, edgeVec, faceVec);
} &&
std::derived_from<typename Mesh::VertexType, Vertex>;

#endif