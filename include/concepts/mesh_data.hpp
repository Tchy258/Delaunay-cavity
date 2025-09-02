#ifndef MESH_DATA_HPP
#define MESH_DATA_HPP

#include<concepts>
#include<vector>
#include<unordered_map>
#include<mesh_data/structures/vertex.hpp>

/**
 * All meshes used throughout the program as template arguments must adhere to this concept, so a mesh must:
 * - Define a type for its vertices and edges, `Mesh::VertexType` and `Mesh::EdgeType`
 * - Define a type for the indices of its vertices and edges, `Mesh::VertexIndex` and `Mesh::EdgeIndex`
 * - Have a vector of vertices, a vector of edges and a vector of ints representing polygons
 * - Have getters for vertices, edges and polygons as well as size getters and setters
 * - Have a way to retrieve the vertices of a triangle at a given face
 * - Have a method to retrieve the indices of the neighbors of a triangle
 * - Be constructible from a vector of vertices, edges and faces
 * - Be copy constructible
 * - Have vertices that are at least a type of `Vertex`
 */
template<typename Mesh>
concept MeshData = requires(
    Mesh& mesh,
    const Mesh& cmesh,
    std::vector<typename Mesh::VertexType> vertexVec,
    std::vector<typename Mesh::EdgeType> edgeVec,
    std::vector<int> faceVec,
    typename Mesh::VertexType vertexT,
    typename Mesh::EdgeType edgeT,
    typename Mesh::VertexIndex,
    typename Mesh::EdgeIndex
) {
    typename Mesh::VertexType;
    typename Mesh::EdgeType;
    typename Mesh::VertexIndex;
    typename Mesh::EdgeIndex;
    { std::declval<const typename Mesh::VertexType&>() } -> std::same_as<const typename Mesh::VertexType&>;
    { mesh.getVertex(1) } -> std::same_as<typename Mesh::VertexType&>;
    { mesh.getEdge(1) } -> std::same_as<typename Mesh::EdgeType&>;
    { mesh.getPolygon(1) } -> std::same_as<int>;
    { cmesh.numberOfVertices()} -> std::same_as<size_t>;
    { cmesh.numberOfEdges()} -> std::same_as<size_t>;
    { cmesh.numberOfPolygons()} -> std::same_as<size_t>;
    { mesh.updatePolygonCount(1)} -> std::same_as<void>;
    { mesh.updateVertexCount(1)} -> std::same_as<void>;
    { mesh.updateEdgeCount(1)} -> std::same_as<void>;
    { mesh.getNeighbors(1)} -> std::same_as<std::vector<int>>;
    { mesh.getVerticesOfTriangle(1,vertexT, vertexT, vertexT)} -> std::same_as<void>;
    Mesh(vertexVec, edgeVec, faceVec); // Constructible from these 3 vectors
    Mesh(mesh); // Copy constructible
} &&
std::derived_from<typename Mesh::VertexType, Vertex>
&& 
std::movable<Mesh>;

#endif