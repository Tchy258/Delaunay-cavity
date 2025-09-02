#ifndef MESH_HELPER_HPP
#define MESH_HELPER_HPP
#include <concepts/mesh_data.hpp>

namespace refiners::helpers {
    /**
     * Generic struct that defines static methods for processing of different mesh types on the refiners.
     * These specializations allow faster compile times by reusing code for the same mesh with different refinement criteria.
     * It also keeps the refiner classes smaller by delegating the responsibility of processing specific mesh types outside of the class
     */
    template <MeshData MeshType>
    struct MeshHelper;
}

#include <mesh_refiners/helpers/delaunay_cavity/mesh_helper_half_edge.hpp>
#include <mesh_refiners/helpers/polylla/mesh_helper_half_edge.hpp>

#endif