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

    std::array<HalfEdgeMesh::EdgeIndex,3> MeshHelper<HalfEdgeMesh>::getEdges(HalfEdgeMesh* outputMesh, FaceIndex triangle) {
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
            edgeCount -= (cavity.allTriangles.size() * 3) + boundaryEdges.size();
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

}