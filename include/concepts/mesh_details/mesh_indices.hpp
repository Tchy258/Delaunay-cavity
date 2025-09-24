#ifndef MESH_INDICES_HPP
#define MESH_INDICES_HPP
#include <concepts>
#include <concepts/primitive_integral.hpp>

/**
 * A Mesh type identifies its vertices, edges and faces via indices which must be some form of primitive integral type, such as
 * char, short, int, long, long long and their unsigned variants
 * 
 * The output index will depend on the final mesh type and it could be either of the other 3, for example, it will be the
 * EdgeIndex for half edge based meshes.
 */
template <typename Mesh>
concept MeshIndices = 
    requires {
        typename Mesh::VertexIndex;
        typename Mesh::EdgeIndex;
        typename Mesh::FaceIndex;
        typename Mesh::OutputIndex;
    }
    && PrimitiveIntegral<typename Mesh::VertexIndex>
    && PrimitiveIntegral<typename Mesh::EdgeIndex>
    && PrimitiveIntegral<typename Mesh::FaceIndex>
    && (std::same_as<typename Mesh::OutputIndex, typename Mesh::EdgeIndex> ||
        std::same_as<typename Mesh::OutputIndex, typename Mesh::FaceIndex> ||
        std::same_as<typename Mesh::OutputIndex, typename Mesh::VertexIndex>); 

#endif // MESH_INDICES_HPP