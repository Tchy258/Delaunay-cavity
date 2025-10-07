#ifndef MESH_HELPER_DELAUNAY_CAVITY_HPP
#define MESH_HELPER_DELAUNAY_CAVITY_HPP
#include <concepts/mesh_data.hpp>

namespace refiners::helpers::delaunay_cavity {

    /**
     * Helper class to do mesh specific operations on generic refiner methods.
     * 
     * This class is only a generic interface with no implementation, concrete implementations must be
     * written for each specific type of MeshData
     */
    template <MeshData MeshType>
    struct MeshHelper {
        using EdgeIndex = typename MeshType::EdgeIndex;
        using FaceIndex = typename MeshType::FaceIndex;
        using VertexIndex = typename MeshType::VertexIndex;
        using OutputIndex = typename MeshType::OutputIndex;
        using Cavity = Cavity<MeshType>;

        /**
         * @param mesh A particular MeshData implementation
         * @returns A vector of output indices that represent what uniquely identifies a particular polygon in
         * the specific implementation of `mesh`
         */
        static std::vector<OutputIndex> generateInitialOutputSeeds(MeshType* mesh) = delete;

        /**
         * @param mesh A particular MeshData implementation
         * @param e An edge index that identifies an edge of `mesh`
         * @return Whether edge `e` is part of the border of the mesh `mesh`. A "border" is implied to be an edge that
         * is only part of a single triangle
         */
        static bool isBorderEdge(const MeshType* mesh, EdgeIndex e) = delete;

        /**
         * @param mesh A particular MeshData implementation
         * @param edge An edge index that identifes an edge of `mesh`
         * @param triangle1 A face index that identifies a triangle of `mesh`
         * @param triangle2 A face index that identifies another triangle of `mesh` different from `triangle1`
         * @return Whether the edge `edge` is the shared edge between `triangle1` and `triangle2`
         */
        static bool isSharedEdge(const MeshType* mesh, EdgeIndex edge, FaceIndex triangle1, FaceIndex triangle2) = delete;

        /**
         * @param mesh A particular MeshData implementation
         * @param triangle A face index that identifies a triangle of `mesh`
         * @return An array that contains the 3 edge indices of `triangle`
         */
        static std::array<EdgeIndex,3> getEdges(MeshType* mesh, FaceIndex triangle) = delete;

        /**
         * Given the information of `cavities`, updates the necessary indices in `mesh` to reflect
         * the changes in the final output
         * @param inputMesh A particular MeshData implementation to traverse the original triangles
         * @param outputMesh A particular MeshData implementation whose contents will be modified
         * @param cavities A vector of Cavity objects with the necessary information to insert the cavities
         * @return A vector of outputs to write
         */
        static std::vector<OutputIndex> insertCavity(const MeshType* inputMesh, MeshType* outputMesh, std::vector<Cavity>& cavities, const std::vector<uint8_t>& inCavity) = delete;
    };

}

#include <mesh_refiners/delaunay_cavity/mesh_helpers/mesh_helper_half_edge_delaunay_cavity.hpp>
#endif // MESH_HELPER_DELAUNAY_CAVITY_HPP