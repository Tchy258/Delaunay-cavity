#ifndef BFS_CAVITY_WALKER
#define BFS_CAVITY_WALKER
#include <concepts/mesh_data.hpp>

namespace refiners::helpers::delaunay_cavity {
    template <MeshData Mesh>
    struct BFSCavityNode {
        using TriangleIndex = typename Mesh::FaceIndex;
        TriangleIndex index;
        unsigned int level;
    };
}

#endif