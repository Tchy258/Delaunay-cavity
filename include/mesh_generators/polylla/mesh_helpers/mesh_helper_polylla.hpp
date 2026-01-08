#ifndef MESH_HELPER_POLYLLA_HPP
#define MESH_HELPER_POLYLLA_HPP
#include <concepts/mesh_data.hpp>
#include <mesh_generators/polylla/helper_structs/polylla_data.hpp>
#include <chrono>

namespace generators::helpers::polylla {

    template <MeshData MeshType>
    struct MeshHelperBase {
        using MeshVertex = typename MeshType::VertexType;
        using MeshEdge = typename MeshType::EdgeType;
        using VertexIndex = typename MeshType::VertexIndex;
        using EdgeIndex = typename MeshType::EdgeIndex;
        using FaceIndex = typename MeshType::FaceIndex;
        using OutputIndex = typename MeshType::OutputIndex;
        using BinaryVector = std::vector<uint8_t>;
        using GeneratorData = PolyllaData<MeshType>;
    };
    template <MeshData MeshType>
    struct MeshHelper : MeshHelperBase<MeshType> {
        using Base = MeshHelperBase<MeshType>;
        using typename Base::MeshVertex;
        using typename Base::MeshEdge;
        using typename Base::VertexIndex;
        using typename Base::EdgeIndex;
        using typename Base::FaceIndex;
        using typename Base::OutputIndex;
        using typename Base::BinaryVector;
        using typename Base::GeneratorData;
        // TODO: add docs
        static void labelMaxEdges(GeneratorData& data, MeshType* mesh) = delete;

        static EdgeIndex findMaxEdge(MeshType* mesh, FaceIndex face) = delete;

        static void labelFrontierEdges(GeneratorData& data, MeshType* mesh) = delete;

        static bool isFrontierEdge(GeneratorData& data, MeshType* mesh, EdgeIndex edge) = delete;

        static std::vector<OutputIndex> generateSeedCandidates(GeneratorData& data, MeshType* mesh) = delete;

        static bool isSeedCandidateIndex(GeneratorData& data, MeshType* mesh, OutputIndex seedCandidate) = delete;

        static std::vector<OutputIndex> generateOutputSeeds(GeneratorData& data, const MeshType* inputMesh, MeshType* outputMesh) = delete;

        static OutputIndex generatePolygonFromSeed(GeneratorData& data, const MeshType* inputMesh, MeshType* outputMesh, OutputIndex seed) = delete;

        static EdgeIndex getNextFrontierEdge(const BinaryVector& frontierEdges, const MeshType* mesh, EdgeIndex edge) = delete;

        static void barrierEdgeTipReparation(GeneratorData& data, const MeshType* inputMesh, MeshType* outputMesh, OutputIndex nonSimpleSeed, std::vector<OutputIndex>& currentOutputs) = delete;

        static EdgeIndex calculateMiddleEdge(GeneratorData& data, const MeshType* inputMesh, VertexIndex barrierEdgeTipVertex) = delete;

        static OutputIndex generateRepairedPolygon(GeneratorData& data, const MeshType* inputMesh, MeshType* outputMesh, OutputIndex seedToRepair) = delete;
    };
}

#include <mesh_generators/polylla/mesh_helpers/mesh_helper_half_edge_polylla.hpp>

#endif // MESH_HELPER_POLYLLA_HPP