#ifndef MESH_DATA_HPP
#define MESH_DATA_HPP

#include<concepts>
#include<vector>
#include<unordered_map>
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
    typename Mesh::EdgeIndex,
    std::unordered_map<int, std::vector<typename Mesh::EdgeIndex>> cavityMap 
) {
    { mesh.getVertex(1) } -> std::same_as<typename Mesh::VertexType&>;
    { mesh.getEdge(1) } -> std::same_as<typename Mesh::EdgeType&>;
    { mesh.getPolygon(1) } -> std::same_as<int>;
    { mesh.numberOfVertices()} -> std::same_as<size_t>;
    { mesh.numberOfEdges()} -> std::same_as<size_t>;
    { mesh.numberOfPolygons()} -> std::same_as<size_t>;
    { mesh.getNeighbors(1)} -> std::same_as<std::vector<int>>;
    { mesh.cavityInsertion(cavityMap)} -> std::same_as<void>;
    Mesh(vertexVec, edgeVec, faceVec);
    Mesh(mesh);
} &&
std::derived_from<typename Mesh::VertexType, Vertex>;

#endif