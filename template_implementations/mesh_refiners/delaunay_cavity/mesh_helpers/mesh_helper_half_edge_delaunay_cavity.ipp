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

    void MeshHelper<HalfEdgeMesh>::markInteriorOutputsFromSeeds(HalfEdgeMesh* mesh, std::vector<OutputIndex>& outputSeeds, const std::vector<FaceIndex>& interiorFaces) {
        std::unordered_set<EdgeIndex> interiorIncidentHalfEdges;
        for (FaceIndex face : interiorFaces) {
            EdgeIndex incidentHe = mesh->getPolygon(face);
            interiorIncidentHalfEdges.insert(incidentHe);
        }
        for (EdgeIndex& seedEdge : outputSeeds) {
            if (interiorIncidentHalfEdges.count(seedEdge)) {
                seedEdge = -1;
            }
        }
    }

    void MeshHelper<HalfEdgeMesh>::eraseInteriorOutputsFromSeeds(std::vector<OutputIndex>& outputSeeds) {
        outputSeeds.erase(std::remove(outputSeeds.begin(), outputSeeds.end(), -1), outputSeeds.end());
        outputSeeds.shrink_to_fit();
    }

    bool MeshHelper<HalfEdgeMesh>::isBorderEdge(HalfEdgeMesh* mesh, EdgeIndex e) {
        return mesh->isBorderEdge(e) || mesh->isBorderEdge(mesh->twin(e));
    }

    bool MeshHelper<HalfEdgeMesh>::isSharedEdge(HalfEdgeMesh* mesh, EdgeIndex edge, FaceIndex triangle1, FaceIndex triangle2) {
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

    void MeshHelper<HalfEdgeMesh>::insertCavity(HalfEdgeMesh* outputMesh, std::vector<Cavity>& cavities) {
        size_t faceCount = outputMesh->numberOfPolygons();
        size_t edgeCount = outputMesh->numberOfEdges();
        std::vector<uint8_t> presentInBoundary(edgeCount);
        for (const Cavity& cavity : cavities) {
            const std::vector<EdgeIndex>& boundaryEdges = cavity.boundaryEdges;
            std::fill(presentInBoundary.begin(), presentInBoundary.end(), 0);
            for (EdgeIndex boundaryEdge : boundaryEdges) {
                presentInBoundary[boundaryEdge] = 1;
            }
            faceCount -= (cavity.allTriangles.size() - 1);
            edgeCount -= (cavity.allTriangles.size() * 3) + boundaryEdges.size();
            EdgeIndex firstEdge = boundaryEdges.front();
            EdgeIndex currentEdge = firstEdge;
            do {
                EdgeIndex nextEdge = outputMesh->next(currentEdge);
                while (!presentInBoundary[nextEdge]) {
                    nextEdge = outputMesh->CWEdgeToVertex(nextEdge);
                }
                outputMesh->setNext(currentEdge,nextEdge);
                outputMesh->setPrev(nextEdge, currentEdge);
                currentEdge = nextEdge;
            } while( currentEdge != firstEdge);
        }
        outputMesh->updateEdgeCount(edgeCount);
        outputMesh->updatePolygonCount(faceCount);
    }
}