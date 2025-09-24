#ifndef MESH_EDGES_HPP
#define MESH_EDGES_HPP
#include <concepts>

/**
 * A Mesh type must have an edge type, since not all edge types have a common
 * defined structure as evidenced by half edges that lack a target in lieu of
 * saving memory, no type enforcement is required at this time, however,
 * an edge type must describe the connection between two Vertex objects
 */
template <typename Mesh>
concept MeshEdges = requires { typename Mesh::EdgeType; };


#endif // MESH_EDGES_HPP