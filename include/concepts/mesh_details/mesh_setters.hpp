#ifndef MESH_SETTERS_HPP
#define MESH_SETTERS_HPP
#include <concepts>
#include <vector>
#include <utility>
/**
 * A Mesh type's vertex, edge and face count should be updateable
 */
template <typename Mesh>
concept MeshSetters = requires(Mesh& mesh, 
    typename Mesh::OutputIndex seedIndexToMergeInto, 
    std::pair<typename Mesh::OutputIndex, std::vector<typename Mesh::EdgeIndex>> seedIndexToMergeFrom, 
    const std::vector<typename Mesh::ConnectivityBackupT>& backup) {
    { mesh.updatePolygonCount(1)} -> std::same_as<void>;
    { mesh.updateVertexCount(1)} -> std::same_as<void>;
    { mesh.updateEdgeCount(1)} -> std::same_as<void>;
    { mesh.mergeSeeds(seedIndexToMergeInto, seedIndexToMergeFrom)} -> std::same_as<std::vector<typename Mesh::ConnectivityBackupT>>;
    { mesh.rollbackMerge(backup)} -> std::same_as<void>;
};


#endif // MESH_SETTERS_HPP