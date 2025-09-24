#ifndef MESH_CONSTRUCTIBLE_HPP
#define MESH_CONSTRUCTIBLE_HPP
#include <concepts>
#include <vector>

/**
 * A Mesh type can be constructed from vectors of vertices, edges and faces, from another mesh, and is also movable
 */
template<typename Mesh>
concept MeshConstructible =
    requires(std::vector<typename Mesh::VertexType> v,
             std::vector<typename Mesh::EdgeType> e,
             std::vector<typename Mesh::FaceIndex> f,
             Mesh m) {
        Mesh(v, e, f); // constructible from containers
        Mesh(m);       // copy constructible
    }
    && std::movable<Mesh>;

#endif // MESH_CONSTRUCTIBLE_HPP