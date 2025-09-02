#ifndef MESH_HELPER_HALF_EDGE_POLYLLA_HPP
#define MESH_HELPER_HALF_EDGE_POLYLLA_HPP

#ifndef MESH_HELPER_HPP // This is just for intellisense
#include <mesh_refiners/helpers/mesh_helper.hpp>
#endif
#include <mesh_data/half_edge_mesh.hpp>

namespace refiners::helpers::polylla {
    template <MeshData MeshType>
    struct MeshHelper;

    template <>
    struct MeshHelper<HalfEdgeMesh> {
        
    };
}

#endif