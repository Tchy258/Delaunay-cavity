#ifndef MESH_HELPER_POLYLLA_HPP
#define MESH_HELPER_POLYLLA_HPP
#include <concepts/mesh_data.hpp>
#include <mesh_refiners/polylla/helper_structs/polylla_data.hpp>
#include <chrono>

namespace refiners::helpers::polylla {

    template <MeshData MeshType>
    struct MeshHelperBase {
        using MeshVertex = typename MeshType::VertexType;
        using MeshEdge = typename MeshType::EdgeType;
        using VertexIndex = typename MeshType::VertexIndex;
        using EdgeIndex = typename MeshType::EdgeIndex;
        using FaceIndex = typename MeshType::FaceIndex;
        using OutputIndex = typename MeshType::OutputIndex;
        using BinaryVector = std::vector<uint8_t>;
        using RefinementData = PolyllaData<MeshType>;
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
        using typename Base::RefinementData;
        // TODO: add docs
        static void labelMaxEdges(RefinementData& data, MeshType* mesh) = delete;

        static EdgeIndex findMaxEdge(MeshType* mesh, FaceIndex face) = delete;

        static void labelFrontierEdges(RefinementData& data, MeshType* mesh) = delete;

        static bool isFrontierEdge(RefinementData& data, MeshType* mesh, EdgeIndex edge) = delete;

        static std::vector<OutputIndex> generateSeedCandidates(RefinementData& data, MeshType* mesh) = delete;

        static bool isSeedCandidateIndex(RefinementData& data, MeshType* mesh, OutputIndex seedCandidate) = delete;

        static std::vector<OutputIndex> generateOutputSeeds(RefinementData& data, const MeshType* inputMesh, MeshType* outputMesh) = delete;

        static OutputIndex generatePolygonFromSeed(RefinementData& data, const MeshType* inputMesh, MeshType* outputMesh, OutputIndex seed) = delete;

        static EdgeIndex getNextFrontierEdge(const BinaryVector& frontierEdges, const MeshType* mesh, EdgeIndex edge) = delete;

        static void barrierEdgeTipReparation(RefinementData& data, const MeshType* inputMesh, MeshType* outputMesh, OutputIndex nonSimpleSeed, std::vector<OutputIndex>& currentOutputs) = delete;

        static EdgeIndex calculateMiddleEdge(RefinementData& data, const MeshType* inputMesh, VertexIndex barrierEdgeTipVertex) = delete;

        static OutputIndex generateRepairedPolygon(RefinementData& data, const MeshType* inputMesh, MeshType* outputMesh, OutputIndex seedToRepair) = delete;
    };
}

#include <mesh_refiners/polylla/mesh_helpers/mesh_helper_half_edge_polylla.hpp>

#endif // MESH_HELPER_POLYLLA_HPP