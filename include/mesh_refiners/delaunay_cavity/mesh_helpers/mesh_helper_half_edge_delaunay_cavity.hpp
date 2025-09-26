#ifndef MESH_HELPER_HALF_EDGE_DELAUNAY_CAVITY_HPP
#define MESH_HELPER_HALF_EDGE_DELAUNAY_CAVITY_HPP

#ifndef MESH_HELPER_DELAUNAY_CAVITY_HPP
#include <mesh_refiners/delaunay_cavity/mesh_helpers/mesh_helper_delaunay_cavity.hpp>
#endif

#include <mesh_data/half_edge_mesh.hpp>
#include <mesh_refiners/delaunay_cavity/helper_structs/cavity.hpp>
#include <unordered_set>
#include <array>
namespace refiners::helpers::delaunay_cavity {

    template <>
    struct MeshHelper<HalfEdgeMesh> {
        using EdgeIndex = HalfEdgeMesh::EdgeIndex;
        using FaceIndex = HalfEdgeMesh::FaceIndex;
        using VertexIndex = HalfEdgeMesh::VertexIndex;
        using OutputIndex = HalfEdgeMesh::OutputIndex;
        using Cavity = Cavity<HalfEdgeMesh>;

        /**
         * @param mesh A HalfEdgeMesh
         * @return A vector of indices to edges where each edge belongs to only one triangle
         */
        static std::vector<OutputIndex> generateInitialOutputSeeds(HalfEdgeMesh* mesh);

        /**
         * Changes the EdgeIndices of `outputSeeds` to -1 if they belong to a triangle inside `interiorFaces`
         * @param mesh A HalfEdgeMesh
         * @param outputSeeds A vector of edge indices where each one uniquely identifies a polygon
         * @param interiorFaces A vector of face indices with triangles to be "destroyed", as they are part of the inside
         * of a cavity
         */
        static void markInteriorOutputsFromSeeds(HalfEdgeMesh* mesh, std::vector<OutputIndex>& outputSeeds, const std::vector<FaceIndex>& interiorFaces);
        /**
         * Finds and deletes all instances of -1 inside `outputSeeds`, effectively getting rid of invalid output indices that belong to interior triangles
         * @param outputSeeds A vector of edge indices where each one uniquely identifies a polygon that contains invalid indices marked with a -1
         */
        static void eraseInteriorOutputsFromSeeds(std::vector<OutputIndex>& outputSeeds);

        /**
         * @param mesh A HalfEdgeMesh
         * @param e An EdgeIndex of `mesh`
         * @return Whether `e` or its twin is a border edge to make sure the half edges at the boundary aren't deleted
         */
        static bool isBorderEdge(HalfEdgeMesh* mesh, EdgeIndex e);

        /**
         * @param mesh A HalfEdgeMesh
         * @param edge An EdgeIndex of `mesh`
         * @param triangle1 A FaceIndex of `mesh` that identifies a triangle
         * @param triangle2 A FaceIndex of `mesh` that identifies a triangle different from `triangle1`
         * @return Whether `e` (or its twin) is the shared edge between `triangle1` and `triangle2`
         * 
         * Both are checked because in the generic logic two triangles only share a single edge, so they are considered "equal"
         */
        static bool isSharedEdge(HalfEdgeMesh* mesh, EdgeIndex edge, FaceIndex triangle1, FaceIndex triangle2);

        /**
         * @param mesh A HalfEdgeMesh
         * @param triangle A FaceIndex of `mesh` that identifies a triangle
         * @return An array of size 3 that contains the CCW interior half edge indices of `triangle`
         */
        static std::array<EdgeIndex,3> getEdges(HalfEdgeMesh* mesh, FaceIndex triangle);

        /**
         * Updates the values of `next` and `prev` of the edges present at the cavity's boundary so walking through them
         * results in making the closed loop of each cavity instead of the triangles that conformed it.
         * 
         * The face and edge count of the mesh are also updated to reflect this change
         * @param mesh A HalfEdgeMesh to insert the cavities into
         * @param cavities A vector of Cavity objects with information to do the cavity insertion
         */
        static void insertCavity(HalfEdgeMesh* mesh, std::vector<Cavity>& cavities);
    };
}

#include <mesh_refiners/delaunay_cavity/mesh_helpers/mesh_helper_half_edge_delaunay_cavity.ipp>

#endif