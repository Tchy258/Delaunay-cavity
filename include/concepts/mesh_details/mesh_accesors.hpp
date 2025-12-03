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
    requires(Mesh& mesh, const Mesh& cmesh, typename Mesh::OutputIndex out,
    typename Mesh::VertexIndex vIdx, typename Mesh::EdgeIndex eIdx, typename Mesh::FaceIndex fIdx) {
        { mesh.getVertex(vIdx) } -> std::same_as<typename Mesh::VertexType&>;
        { mesh.getEdge(eIdx) } -> std::same_as<typename Mesh::EdgeType&>;
        { mesh.getPolygon(fIdx) } -> std::same_as<typename Mesh::OutputIndex>;
        { cmesh.numberOfVertices()} -> std::convertible_to<size_t>;
        { cmesh.getVertexVectorSize()} -> std::convertible_to<size_t>;
        { cmesh.numberOfEdges()} -> std::convertible_to<size_t>;
        { cmesh.getEdgeVectorSize()} -> std::convertible_to<size_t>;
        { cmesh.numberOfPolygons()} -> std::convertible_to<size_t>;
        { cmesh.getFaceVectorSize()} -> std::convertible_to<size_t>;
        { cmesh.getOutputSeedEdgeCount(out)} -> std::convertible_to<size_t>;
    };

#endif // MESH_ACCESORS_HPP