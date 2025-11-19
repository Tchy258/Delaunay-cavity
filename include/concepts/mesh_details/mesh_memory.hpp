#ifndef MESH_MEMORY_HPP
#define MESH_MEMORY_HPP
#include <concepts>
#include <cstddef>
/**
 * A Mesh type has getters for its memory usage
 */
template<typename Mesh>
concept MeshMemory =
    requires(const Mesh& cmesh) {
        { cmesh.getVertexMemoryUsage()} -> std::convertible_to<size_t>;
        { cmesh.getEdgesMemoryUsage()} -> std::convertible_to<size_t>;
    };

#endif // MESH_MEMORY_HPP