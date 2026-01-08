#ifndef MESH_HELPER_HALF_EDGE_POLYLLA_HPP
#define MESH_HELPER_HALF_EDGE_POLYLLA_HPP
#ifndef MESH_HELPER_POLYLLA_HPP
#include <mesh_generators/polylla/mesh_helpers/mesh_helper_polylla.hpp>
#endif

#include <mesh_data/half_edge_mesh.hpp>

namespace generators::helpers::polylla {

    template <>
    struct MeshHelper<HalfEdgeMesh> : MeshHelperBase<HalfEdgeMesh> {
        // TODO: add docs
        static void labelMaxEdges(GeneratorData& data, HalfEdgeMesh* mesh);

        static EdgeIndex findMaxEdge(HalfEdgeMesh* mesh, FaceIndex face);

        static void labelFrontierEdges(GeneratorData& data, HalfEdgeMesh* mesh);

        static bool isFrontierEdge(GeneratorData& data, HalfEdgeMesh* mesh, EdgeIndex edge);

        static std::vector<OutputIndex> generateSeedCandidates(GeneratorData& data, HalfEdgeMesh* mesh);

        static bool isSeedCandidateIndex(GeneratorData& data, HalfEdgeMesh* mesh, OutputIndex seedCandidate);

        static std::vector<OutputIndex> generateOutputSeeds(GeneratorData& data, const HalfEdgeMesh* inputMesh, HalfEdgeMesh* outputMesh);

        static OutputIndex generatePolygonFromSeed(GeneratorData& data, const HalfEdgeMesh* inputMesh, HalfEdgeMesh* outputMesh, OutputIndex seed);

        static EdgeIndex getNextFrontierEdge(GeneratorData& data, const HalfEdgeMesh* mesh, EdgeIndex edge);

        static void barrierEdgeTipReparation(GeneratorData& data, const HalfEdgeMesh* inputMesh, HalfEdgeMesh* outputMesh, OutputIndex nonSimpleSeed, std::vector<OutputIndex>& currentOutputs);

        static EdgeIndex calculateMiddleEdge(GeneratorData& data, const HalfEdgeMesh* inputMesh, VertexIndex barrierEdgeTipVertex);

        static OutputIndex generateRepairedPolygon(GeneratorData& data, const HalfEdgeMesh* inputMesh, HalfEdgeMesh* outputMesh, OutputIndex seedToRepair);
    };
}

#include <mesh_generators/polylla/mesh_helpers/mesh_helper_half_edge_polylla.ipp>

#endif // MESH_HELPER_HALF_EDGE_POLYLLA_HPP