#ifndef MESH_VERTICES_HPP
#define MESH_VERTICES_HPP
#include <concepts>
#include <mesh_data/structures/vertex.hpp>

/**
 * A Mesh type must have a VertexType which in turn is a type of Vertex
 */
template <typename Mesh>
concept MeshVertices = requires { typename Mesh::VertexType; }
    && std::derived_from<typename Mesh::VertexType, Vertex>;

#endif // MESH_VERTICES_HPP