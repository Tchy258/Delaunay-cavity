#ifndef MESH_ACCESORS_HPP
#define MESH_ACCESORS_HPP
#include <concepts>
#include <cstddef>
/**
 * A Mesh type has getters for its vertices, edges and polygons.
 * 
 * Their count can also be queried
 */
template<typename Mesh>
concept MeshAccessors =
    requires(Mesh& mesh, const Mesh& cmesh) {
        { mesh.getVertex(1) } -> std::same_as<typename Mesh::VertexType&>;
        { mesh.getEdge(1) } -> std::same_as<typename Mesh::EdgeType&>;
        { mesh.getPolygon(1) } -> std::same_as<typename Mesh::FaceIndex>;
        { cmesh.numberOfVertices()} -> std::convertible_to<size_t>;
        { cmesh.numberOfEdges()} -> std::convertible_to<size_t>;
        { cmesh.numberOfPolygons()} -> std::convertible_to<size_t>;
    };

#endif // MESH_ACCESORS_HPP