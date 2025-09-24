#ifndef MESH_DATA_HPP
#define MESH_DATA_HPP

#include <concepts>
#include <concepts/mesh_details/mesh_accesors.hpp>
#include <concepts/mesh_details/mesh_constructible.hpp>
#include <concepts/mesh_details/mesh_edges.hpp>
#include <concepts/mesh_details/mesh_indices.hpp>
#include <concepts/mesh_details/mesh_setters.hpp>
#include <concepts/mesh_details/mesh_topology.hpp>
#include <concepts/mesh_details/mesh_vertices.hpp>

/**
 * All meshes used throughout the program as template arguments must adhere to this concept, so a mesh must:
 * - Define a type for its vertices and edges, `Mesh::VertexType` and `Mesh::EdgeType`
 * - Define a type for the indices of its vertices and edges, `Mesh::VertexIndex`, `Mesh::EdgeIndex` and `Mesh::FaceIndex`
 * - Define an output index type that is one of the three above
 * - The indices mentioned above must be some fundamental integer type (char, short, int, long, long long and unsigned variants)
 * - Have a vector of vertices, a vector of edges and a vector of ints representing polygons
 * - Have getters for vertices, edges and polygons as well as size getters and setters
 * - Have a way to retrieve the vertices of a triangle at a given face
 * - Have a method to retrieve the indices of the neighbors of a triangle
 * - Be constructible from a vector of vertices, edges and faces
 * - Be copy constructible
 * - Have vertices that are at least a type of `Vertex`
 */
template<typename Mesh>
concept MeshData = 
    MeshVertices<Mesh>
    && MeshEdges<Mesh>
    && MeshIndices<Mesh>
    && MeshAccessors<Mesh>
    && MeshSetters<Mesh>
    && MeshTopology<Mesh>
    && MeshConstructible<Mesh>;

#endif