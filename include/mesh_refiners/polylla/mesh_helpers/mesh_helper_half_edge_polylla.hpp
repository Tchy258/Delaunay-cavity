#ifndef MESH_HELPER_HALF_EDGE_POLYLLA_HPP
#define MESH_HELPER_HALF_EDGE_POLYLLA_HPP
#ifndef MESH_HELPER_POLYLLA_HPP
#include <mesh_refiners/polylla/mesh_helpers/mesh_helper_polylla.hpp>
#endif

#include <mesh_data/half_edge_mesh.hpp>

namespace refiners::helpers::polylla {

    template <>
    struct MeshHelper<HalfEdgeMesh> : MeshHelperBase<HalfEdgeMesh> {
        // TODO: add docs
        static void labelMaxEdges(RefinementData& data, HalfEdgeMesh* mesh);

        static EdgeIndex findMaxEdge(HalfEdgeMesh* mesh, FaceIndex face);

        static void labelFrontierEdges(RefinementData& data, HalfEdgeMesh* mesh);

        static bool isFrontierEdge(RefinementData& data, HalfEdgeMesh* mesh, EdgeIndex edge);

        static std::vector<OutputIndex> generateSeedCandidates(RefinementData& data, HalfEdgeMesh* mesh);

        static bool isSeedCandidateIndex(RefinementData& data, HalfEdgeMesh* mesh, OutputIndex seedCandidate);

        static std::vector<OutputIndex> generateOutputSeeds(RefinementData& data, const HalfEdgeMesh* inputMesh, HalfEdgeMesh* outputMesh);

        static OutputIndex generatePolygonFromSeed(RefinementData& data, const HalfEdgeMesh* inputMesh, HalfEdgeMesh* outputMesh, OutputIndex seed);

        static EdgeIndex getNextFrontierEdge(RefinementData& data, const HalfEdgeMesh* mesh, EdgeIndex edge);

        static void barrierEdgeTipReparation(RefinementData& data, const HalfEdgeMesh* inputMesh, HalfEdgeMesh* outputMesh, OutputIndex nonSimpleSeed, std::vector<OutputIndex>& currentOutputs);

        static EdgeIndex calculateMiddleEdge(RefinementData& data, const HalfEdgeMesh* inputMesh, VertexIndex barrierEdgeTipVertex);

        static OutputIndex generateRepairedPolygon(RefinementData& data, const HalfEdgeMesh* inputMesh, HalfEdgeMesh* outputMesh, OutputIndex seedToRepair);
    };
}

#include <mesh_refiners/polylla/mesh_helpers/mesh_helper_half_edge_polylla.ipp>

#endif // MESH_HELPER_HALF_EDGE_POLYLLA_HPP