#ifndef MESH_HELPER_HALF_EDGE_DELAUNAY_CAVITY_HPP
#include <mesh_generators/delaunay_cavity/mesh_helpers/mesh_helper_half_edge_delaunay_cavity.hpp>
#endif 

namespace generators::helpers::delaunay_cavity {
    
    std::vector<HalfEdgeMesh::EdgeIndex> MeshHelper<HalfEdgeMesh>::generateInitialOutputSeeds(HalfEdgeMesh* mesh) {
        size_t polygonAmount = mesh->numberOfPolygons();
        std::vector<EdgeIndex> incidentHalfEdges;
        incidentHalfEdges.reserve(polygonAmount);
        for (size_t i = 0; i < polygonAmount; ++i) {
            incidentHalfEdges.push_back(mesh->getPolygon(i));
        }
        return incidentHalfEdges;
    }

    bool MeshHelper<HalfEdgeMesh>::isBorderEdge(const HalfEdgeMesh* mesh, EdgeIndex e) {
        return mesh->isBorderEdge(e) || mesh->isBorderEdge(mesh->twin(e));
    }

    bool MeshHelper<HalfEdgeMesh>::isSharedTriangleEdge(const HalfEdgeMesh* mesh, EdgeIndex edge, FaceIndex triangle1, FaceIndex triangle2) {
        EdgeIndex candidate1 = mesh->getTriangleSharedEdge(triangle1,triangle2);
        EdgeIndex candidate2 = mesh->twin(candidate1);
        return edge == candidate1 || edge == candidate2;
    }

    std::array<HalfEdgeMesh::EdgeIndex,3> MeshHelper<HalfEdgeMesh>::getTriangleEdges(HalfEdgeMesh* outputMesh, FaceIndex triangle) {
        EdgeIndex edge1 = outputMesh->getPolygon(triangle);
        EdgeIndex edge2 = outputMesh->next(edge1);
        EdgeIndex edge3 = outputMesh->next(edge2);
        return {edge1, edge2, edge3};
    }

    std::vector<HalfEdgeMesh::OutputIndex> MeshHelper<HalfEdgeMesh>::insertCavity(const HalfEdgeMesh* inputMesh, HalfEdgeMesh* outputMesh, std::vector<_Cavity>& cavities, const std::vector<uint8_t>& inCavity) {
        size_t faceCount = outputMesh->numberOfPolygons();
        size_t edgeCount = outputMesh->numberOfEdges();
        std::vector<uint8_t> presentInBoundary(edgeCount);
        std::vector<OutputIndex> outputSeeds;
        outputSeeds.reserve(faceCount);
        for (FaceIndex face = 0; face < faceCount; ++face) {
            if (!inCavity[face]) {
                EdgeIndex incidentHE = inputMesh->getPolygon(face);
                outputSeeds.push_back(incidentHE);
            }
        }
        for (const _Cavity& cavity : cavities) {
            const std::vector<EdgeIndex>& boundaryEdges = cavity.boundaryEdges;
            EdgeIndex firstEdge = boundaryEdges.front();
            outputSeeds.push_back(firstEdge);
            if (cavity.allTriangles.size() == 1) { 
                continue;
            }
            faceCount -= (cavity.allTriangles.size() - 1);
            edgeCount -= (cavity.allTriangles.size() * 3) - boundaryEdges.size();
            std::fill(presentInBoundary.begin(), presentInBoundary.end(), 0);
            for (EdgeIndex boundaryEdge : boundaryEdges) {
                presentInBoundary[boundaryEdge] = 1;
            }
            EdgeIndex currentEdge = firstEdge;
            do {
                EdgeIndex nextEdge = inputMesh->next(currentEdge);
                while (!presentInBoundary[nextEdge]) {
                    nextEdge = inputMesh->CCWEdgeToVertex(nextEdge);
                }
                outputMesh->setNext(currentEdge,nextEdge);
                outputMesh->setPrev(nextEdge, currentEdge);
                currentEdge = nextEdge;
            } while( currentEdge != firstEdge);
        }
        outputMesh->updateEdgeCount(edgeCount);
        outputMesh->updatePolygonCount(faceCount);
        return outputSeeds;
    }
    inline UnionFindCavityMerger<HalfEdgeMesh> MeshHelper<HalfEdgeMesh>::buildEdgeToOutputMap(HalfEdgeMesh *outputMesh, const std::vector<OutputIndex> &outputSeeds) {
        UnionFindCavityMerger<HalfEdgeMesh> edgeMap(outputMesh->getEdgeVectorSize());
        for (OutputIndex seed : outputSeeds) {
            EdgeIndex currentEdge = seed;
            do {
                edgeMap.unite(currentEdge,seed);
                currentEdge = outputMesh->next(currentEdge);
            } while (currentEdge != seed);
        }
        return edgeMap;
    }
    inline HalfEdgeMesh::OutputIndex MeshHelper<HalfEdgeMesh>::changeToValidRepresentative(HalfEdgeMesh *outputMesh, UnionFindCavityMerger<HalfEdgeMesh> &edgeToOutputMap, std::vector<EdgeIndex> invalidEdges, OutputIndex currentRepresentaive) {
        EdgeIndex newRepresentative = currentRepresentaive;
        std::vector<EdgeIndex> twins;
        twins.reserve(invalidEdges.size());

        std::ranges::transform(
            invalidEdges,
            std::back_inserter(twins),
            [&](EdgeIndex e){ return outputMesh->twin(e); }
        );

        bool representativeIsValid = false;
        while (!representativeIsValid) {
            representativeIsValid = true;
            for (EdgeIndex edge : twins) {
                if ( edge == newRepresentative) {
                    representativeIsValid = false;
                    newRepresentative = outputMesh->next(newRepresentative);
                    break;
                }
            }
        }
        if (currentRepresentaive != newRepresentative) {
            EdgeIndex firstEdge = twins[0];
            EdgeIndex currentEdge = firstEdge;
            do {
                edgeToOutputMap.unite(currentEdge, newRepresentative);
                currentEdge = outputMesh->next(currentEdge);
            } while (firstEdge != currentEdge);
        }
        return newRepresentative;
    }

    template <PolygonMergingPolicy<HalfEdgeMesh> MergingPolicy>
    void MeshHelper<HalfEdgeMesh>::mergeIntoNeighbor(const HalfEdgeMesh *inputMesh, HalfEdgeMesh *outputMesh, std::vector<OutputIndex> &outputSeeds, OutputIndex seedToMerge, UnionFindCavityMerger<HalfEdgeMesh>& edgeToOutputMap) {

        std::vector<OutputIndex> neighborSeeds;
        std::vector<std::vector<EdgeIndex>> sharedEdges;
        size_t edgeCount = outputMesh->getOutputSeedEdgeCount(seedToMerge);

        neighborSeeds.reserve(edgeCount);
        sharedEdges.reserve(edgeCount);
        EdgeIndex currentEdge = seedToMerge;
        do {
            if (!outputMesh->isBorderEdge(outputMesh->twin(currentEdge))) {
                std::vector<EdgeIndex> edgesSharedWithNeighbor = outputMesh->getSharedEdges(currentEdge, outputMesh->twin(currentEdge));
                sharedEdges.push_back(edgesSharedWithNeighbor);
                OutputIndex oldRepresentative = edgeToOutputMap.find(outputMesh->twin(currentEdge));
                OutputIndex matchingNeighborSeed = changeToValidRepresentative(outputMesh, edgeToOutputMap, edgesSharedWithNeighbor, oldRepresentative);
                if (oldRepresentative != matchingNeighborSeed) {
                    std::replace(outputSeeds.begin(), outputSeeds.end(), oldRepresentative, matchingNeighborSeed);
                }
                neighborSeeds.push_back(matchingNeighborSeed);
            }
            currentEdge = outputMesh->next(currentEdge);
        } while (currentEdge != seedToMerge);
        
        OutputIndex chosenNeighbor = MergingPolicy::mergeBestCandidate(outputMesh, seedToMerge, neighborSeeds, sharedEdges);
        if (chosenNeighbor != HalfEdgeMesh::invalidIndexValue) {
            std::replace(outputSeeds.begin(), outputSeeds.end(), seedToMerge, HalfEdgeMesh::invalidIndexValue);
            currentEdge = chosenNeighbor;
            do {
                edgeToOutputMap.unite(currentEdge, chosenNeighbor);
                currentEdge = outputMesh->next(currentEdge);
            } while (currentEdge != chosenNeighbor);
        }
    }
}