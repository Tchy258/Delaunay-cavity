#ifndef MESH_HELPER_HALF_EDGE_DELAUNAY_CAVITY_HPP
#include <mesh_refiners/delaunay_cavity/mesh_helpers/mesh_helper_half_edge_delaunay_cavity.hpp>
#include "mesh_helper_half_edge_delaunay_cavity.hpp"
#endif 

namespace refiners::helpers::delaunay_cavity {
    
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

    std::vector<HalfEdgeMesh::OutputIndex> MeshHelper<HalfEdgeMesh>::insertCavity(const HalfEdgeMesh* inputMesh, HalfEdgeMesh* outputMesh, std::vector<Cavity>& cavities, const std::vector<uint8_t>& inCavity) {
        size_t faceCount = outputMesh->numberOfPolygons();
        size_t edgeCount = outputMesh->numberOfEdges();
        std::vector<uint8_t> presentInBoundary(edgeCount);
        std::vector<uint8_t> alreadyUsed(edgeCount,0);
        std::vector<OutputIndex> outputSeeds;
        outputSeeds.reserve(faceCount);
        for (FaceIndex face = 0; face < faceCount; ++face) {
            if (!inCavity[face]) {
                EdgeIndex incidentHE = inputMesh->getPolygon(face);
                outputSeeds.push_back(incidentHE);
            }
        }
        for (const Cavity& cavity : cavities) {
            const std::vector<EdgeIndex>& boundaryEdges = cavity.boundaryEdges;
            std::fill(presentInBoundary.begin(), presentInBoundary.end(), 0);
            for (EdgeIndex boundaryEdge : boundaryEdges) {
                presentInBoundary[boundaryEdge] = 1;
            }
            faceCount -= (cavity.allTriangles.size() - 1);
            edgeCount -= (cavity.allTriangles.size() * 3) - boundaryEdges.size();
            EdgeIndex firstEdge = boundaryEdges.front();
            outputSeeds.push_back(firstEdge);
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
    inline std::unordered_map<HalfEdgeMesh::EdgeIndex, HalfEdgeMesh::OutputIndex> MeshHelper<HalfEdgeMesh>::buildEdgeToOutputMap(HalfEdgeMesh *outputMesh, const std::vector<OutputIndex> &outputSeeds) {
        std::unordered_map<EdgeIndex, OutputIndex> edgeMap;
        edgeMap.reserve(outputMesh->numberOfEdges());
        for (OutputIndex seed : outputSeeds) {
            EdgeIndex currentEdge = seed;
            do {
                edgeMap.insert_or_assign(currentEdge,seed);
                currentEdge = outputMesh->next(currentEdge);
            } while (currentEdge != seed);
        }
        return edgeMap;
    }
    inline HalfEdgeMesh::OutputIndex MeshHelper<HalfEdgeMesh>::changeToValidRepresentative(HalfEdgeMesh *outputMesh, std::unordered_map<EdgeIndex, OutputIndex> &edgeToOutputMap, std::vector<EdgeIndex> invalidEdges, OutputIndex currentRepresentaive) {
        EdgeIndex newRepresentative = currentRepresentaive;
        std::vector<EdgeIndex> twins(invalidEdges.begin(), invalidEdges.end());
        for (int i = 0; i < twins.size(); ++i) {
            twins[i] = outputMesh->twin(twins[i]);
        }
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
                edgeToOutputMap[currentEdge] = newRepresentative;
                currentEdge = outputMesh->next(currentEdge);
            } while (firstEdge != currentEdge);
        }
        return newRepresentative;
    }

    template <PolygonMergingPolicy<HalfEdgeMesh> MergingPolicy>
    void MeshHelper<HalfEdgeMesh>::mergeIntoNeighbor(const HalfEdgeMesh *inputMesh, HalfEdgeMesh *outputMesh, std::vector<OutputIndex> &outputSeeds, OutputIndex seedToMerge, std::unordered_map<EdgeIndex, OutputIndex>& edgeToOutputMap)
    {
        std::vector<OutputIndex> seedEdges;
        std::vector<OutputIndex> neighborSeeds;
        std::vector<std::vector<EdgeIndex>> sharedEdges;
        size_t edgeCount = outputMesh->getOutputSeedEdgeCount(seedToMerge);
        seedEdges.reserve(edgeCount);
        neighborSeeds.reserve(edgeCount);
        sharedEdges.reserve(edgeCount);
        EdgeIndex currentEdge = seedToMerge;
        do {
            if (!outputMesh->isBorderEdge(outputMesh->twin(currentEdge))) {
                seedEdges.push_back(currentEdge);
                std::vector<EdgeIndex> edgesSharedWithNeighbor = outputMesh->getSharedEdges(currentEdge, outputMesh->twin(currentEdge));
                sharedEdges.push_back(edgesSharedWithNeighbor);
                OutputIndex oldRepresentative = edgeToOutputMap[outputMesh->twin(currentEdge)];
                OutputIndex matchingNeighborSeed = changeToValidRepresentative(outputMesh, edgeToOutputMap, edgesSharedWithNeighbor, edgeToOutputMap[outputMesh->twin(currentEdge)]);
                if (oldRepresentative != matchingNeighborSeed) {
                    std::replace(outputSeeds.begin(), outputSeeds.end(), oldRepresentative, matchingNeighborSeed);
                }
                neighborSeeds.push_back(matchingNeighborSeed);
            }
            currentEdge = outputMesh->next(currentEdge);
        } while (currentEdge != seedToMerge);
        
        OutputIndex chosenNeighbor = MergingPolicy::mergeBestCandidate(outputMesh, seedToMerge, neighborSeeds, sharedEdges);
        if (chosenNeighbor != -1) {
            outputSeeds.erase(std::remove(outputSeeds.begin(), outputSeeds.end(), seedToMerge), outputSeeds.end());
            currentEdge = chosenNeighbor;
            do {
                edgeToOutputMap[currentEdge] = chosenNeighbor;
                currentEdge = outputMesh->next(currentEdge);
            } while (currentEdge != chosenNeighbor);
        }
    }
}