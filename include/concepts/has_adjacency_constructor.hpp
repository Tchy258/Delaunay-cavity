#ifndef HAS_ADJACENCY_CONSTRUCTOR_HPP
#define HAS_ADJACENCY_CONSTRUCTOR_HPP
#include <concepts>
#include <concepts/primitive_integral.hpp>
#include <concepts/mesh_data.hpp>
#include <vector>


/**
 * A Mesh type has an adjacency constructor if it can be constructed from a vector of vertices, edges, faces and neighbor faces
 */
template<typename Mesh>
concept HasAdjacencyConstructor =
    requires (
        std::vector<typename Mesh::VertexType> vertices,
        std::vector<typename Mesh::EdgeType> edges,
        std::vector<typename Mesh::FaceIndex> faces,
        std::vector<typename Mesh::FaceIndex> neighbors
    ) {
        Mesh(
            vertices,
            edges,
            faces,
            neighbors
        );
    }
    && MeshData<Mesh>;

#endif