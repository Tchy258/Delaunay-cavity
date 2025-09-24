#ifndef MESH_SETTERS_HPP
#define MESH_SETTERS_HPP
#include <concepts>

/**
 * A Mesh type's vertex, edge and face count should be updateable
 */
template <typename Mesh>
concept MeshSetters = requires(Mesh& mesh) {
    { mesh.updatePolygonCount(1)} -> std::same_as<void>;
    { mesh.updateVertexCount(1)} -> std::same_as<void>;
    { mesh.updateEdgeCount(1)} -> std::same_as<void>;
};


#endif // MESH_SETTERS_HPP