#ifndef MESH_HELPER_HALF_EDGE_DELAUNAY_CAVITY_HPP
#include <mesh_refiners/delaunay_cavity/mesh_helpers/mesh_helper_half_edge_delaunay_cavity.hpp>
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

    bool MeshHelper<HalfEdgeMesh>::isSharedEdge(const HalfEdgeMesh* mesh, EdgeIndex edge, FaceIndex triangle1, FaceIndex triangle2) {
        EdgeIndex candidate1 = mesh->getSharedEdge(triangle1,triangle2);
        EdgeIndex candidate2 = mesh->twin(candidate1);
        return edge == candidate1 || edge == candidate2;
    }

    std::array<HalfEdgeMesh::EdgeIndex,3> MeshHelper<HalfEdgeMesh>::getTriangleEdges(HalfEdgeMesh* outputMesh, FaceIndex triangle) {
        EdgeIndex edge1 = outputMesh->getPolygon(triangle);
        EdgeIndex edge2 = outputMesh->next(edge1);
        EdgeIndex edge3 = outputMesh->next(edge2);
        return {edge1, edge2, edge3};
    }

    unsigned int MeshHelper<HalfEdgeMesh>::seedPolygonEdgeCount(HalfEdgeMesh* mesh, OutputIndex seedIndex) {
        EdgeIndex currentEdge = seedIndex;
        unsigned int edgeCount = 0;
        do {
            ++edgeCount;
            currentEdge = mesh->next(currentEdge);
        } while (currentEdge != seedIndex);
        return edgeCount;
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
    template <PolygonMergingPolicy MergingPolicy>
    void MeshHelper<HalfEdgeMesh>::mergeIntoNeighbor(const HalfEdgeMesh* inputMesh, HalfEdgeMesh* outputMesh, std::vector<OutputIndex>& outputSeeds, OutputIndex seedToMerge) {
        std::array<HalfEdgeMesh::EdgeIndex,3> triangleEdges = {seedToMerge, outputMesh->next(seedToMerge), outputMesh->prev(seedToMerge)};
        std::array<HalfEdgeMesh::EdgeIndex,3> neighborSeeds = {-1,-1,-1};
        for (unsigned int i = 0; i < 3; ++i) {
            EdgeIndex twinEdge = outputMesh->twin(triangleEdges[i]);
            if (outputMesh->isBorderEdge(twinEdge)) continue;
            EdgeIndex initial = twinEdge;
            auto seedIterator = std::find(outputSeeds.begin(), outputSeeds.end(), twinEdge);
            if (seedIterator == outputSeeds.end()) {
                do {
                    twinEdge = outputMesh->next(twinEdge);
                    seedIterator = std::find(outputSeeds.begin(), outputSeeds.end(), twinEdge);
                    if (seedIterator != outputSeeds.end()) break;
                } while (initial != twinEdge);
            }
            while (*seedIterator == outputMesh->twin(triangleEdges[0]) || *seedIterator == outputMesh->twin(triangleEdges[1]) || *seedIterator == outputMesh->twin(triangleEdges[2])) {
                *seedIterator = outputMesh->next(*seedIterator);
            }
            neighborSeeds[i] = *seedIterator;
        }
        // Very particular edge case 
        if (neighborSeeds[0] == -1 && neighborSeeds[1] == -1 && neighborSeeds[2] == -1) return;
        bool wasMerged = mergeIntoNeighborImpl(MergingPolicy{}, inputMesh, outputMesh, triangleEdges, neighborSeeds);
        if (wasMerged) {
            outputSeeds.erase(std::remove(outputSeeds.begin(), outputSeeds.end(), seedToMerge), outputSeeds.end());
        }
    }

    bool MeshHelper<HalfEdgeMesh>::mergeIntoNeighborImpl(MaximizeConvexityMergingPolicy, const HalfEdgeMesh* inputMesh, HalfEdgeMesh* outputMesh, std::array<EdgeIndex,3> triangleEdges, std::array<EdgeIndex,3> neighborSeeds) {
        for (unsigned int i = 0; i < 3; ++i) {
            EdgeIndex twin = outputMesh->twin(triangleEdges[i]);
            if (outputMesh->isBorderEdge(twin)) continue;
            EdgeIndex prevToSharedEdge = outputMesh->prev(twin);
            EdgeIndex nextToSharedEdge = outputMesh->next(twin);
            EdgeIndex prevOfCurrent = outputMesh->prev(triangleEdges[i]);
            EdgeIndex nextOfCurrent = outputMesh->next(triangleEdges[i]);

            std::array<EdgeIndex,6> edges = { prevToSharedEdge, nextToSharedEdge, prevOfCurrent, nextOfCurrent, twin, triangleEdges[i] };
            std::array<EdgeIndex,6> origNext, origPrev;
            for (size_t k = 0; k < edges.size(); ++k) {
                origNext[k] = outputMesh->next(edges[k]);
                origPrev[k] = outputMesh->prev(edges[k]);
            }

            outputMesh->setNext(prevToSharedEdge, nextOfCurrent);
            outputMesh->setPrev(nextOfCurrent, prevToSharedEdge);
            outputMesh->setNext(prevOfCurrent, nextToSharedEdge);
            outputMesh->setPrev(nextToSharedEdge, prevOfCurrent);
            if (!outputMesh->isPolygonConvex(neighborSeeds[i])) {
                for (size_t k = 0; k < edges.size(); ++k) {
                    outputMesh->setNext(edges[k], origNext[k]);
                    outputMesh->setPrev(edges[k], origPrev[k]);
                }
            } else {
                outputMesh->updateEdgeCount(outputMesh->numberOfEdges() - 2);
                outputMesh->updatePolygonCount(outputMesh->numberOfPolygons() - 1);
                return true;
            }
        }
        return false;
    }
}