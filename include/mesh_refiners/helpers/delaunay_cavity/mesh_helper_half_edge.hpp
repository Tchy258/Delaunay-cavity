#ifndef MESH_HELPER_HALF_EDGE_DELAUNAY_CAVITY_HPP
#define MESH_HELPER_HALF_EDGE_DELAUNAY_CAVITY_HPP

#ifndef MESH_HELPER_HPP // This is just for intellisense
#include <mesh_refiners/helpers/mesh_helper.hpp>
#endif
#include <mesh_data/half_edge_mesh.hpp>
#include <mesh_refiners/helpers/delaunay_cavity/cavity.hpp>
#include<array>
namespace refiners::helpers::delaunay_cavity {

    template <MeshData MeshType>
    struct MeshHelper;

    template <>
    struct MeshHelper<HalfEdgeMesh> {
        using EdgeIndex = HalfEdgeMesh::EdgeIndex;
        using FaceIndex = HalfEdgeMesh::FaceIndex;
        using VertexIndex = HalfEdgeMesh::VertexIndex;
        using Cavity = Cavity<HalfEdgeMesh>;
        static void removeAndReconnectEdges(HalfEdgeMesh* mesh, EdgeIndex edgeToRemove, size_t& edgeCount) {
            
            // First we check if it wasn't removed already
            if (mesh->origin(edgeToRemove) == -1) return;
            // The previous edge's next edge, is the CW edge to the removed edge
            EdgeIndex prevEdgeIdx = mesh->prev(edgeToRemove);
            EdgeIndex newNext = mesh->CWEdgeToVertex(edgeToRemove);
            while (mesh->origin(newNext) == -1) {
                newNext = mesh->CWEdgeToVertex(newNext);
            }
            if (newNext == edgeToRemove) {
                return;
            }
            mesh->setNext(prevEdgeIdx, newNext);
            mesh->setPrev(newNext, prevEdgeIdx);

            //EdgeIndex nextEdgeIdx = outputMesh->next(edgeToRemove);
            //EdgeIndex newPrev = outputMesh->prev(outputMesh->twin(edgeToRemove));
            //outputMesh->setPrev(nextEdgeIdx, newPrev);
            HEVertex& originOfRemoved = mesh->getVertex(mesh->origin(edgeToRemove));
            // This "removes" a reference to the edge in the mesh
            originOfRemoved.incidentHalfEdge = newNext;
            // We remove this edge by marking its origin as -1
            HalfEdge& removedEdge = mesh->getEdge(edgeToRemove);
            removedEdge.origin = -1;
            edgeCount-= 1;
        }

        /**
         * Selects which edges of the mesh should be kept for each cavity (their borders).
         * This selection is done by checking which edges satisfy one of two conditions:
         * - The edge is a border of the original mesh
         * - The edge is a part of triangle to be deleted by the cavity and its neighbor is not part of the cavity
         */
        static std::unordered_map<int, std::vector<EdgeIndex>> selectCavityEdges(HalfEdgeMesh* mesh, const std::unordered_map<int, std::vector<int>>& cavities) {
            std::unordered_map<int, std::vector<EdgeIndex>> cavityEdges;
            cavityEdges.reserve(cavities.size());
            
            for (const auto& [cavitySeed, cavityPolygons] : cavities) {
                

                auto inCavity = [&cavityPolygons](int cavityFace) {
                    return std::binary_search(cavityPolygons.begin(), cavityPolygons.end(), cavityFace);
                };
                std::vector<EdgeIndex> cavityBorderEdges;
                cavityBorderEdges.reserve(cavityPolygons.size() * 2); // heuristic

                for (int face : cavityPolygons) {
                    EdgeIndex firstEdge = mesh->getPolygon(face);
                    EdgeIndex currentEdge = firstEdge;

                    do {
                        if (mesh->isBorderEdge(currentEdge)) {
                            cavityBorderEdges.push_back(currentEdge);
                        } else {
                            EdgeIndex twin = mesh->twin(currentEdge);
                            int neighborFace = mesh->getFaceOfEdge(twin);

                            if (!inCavity(neighborFace)) {
                                cavityBorderEdges.push_back(currentEdge);
                            }
                        }
                        currentEdge = mesh->next(currentEdge);
                    } while (currentEdge != firstEdge);
                }

                // Deduplicate edges
                std::sort(cavityBorderEdges.begin(), cavityBorderEdges.end());
                cavityBorderEdges.erase(std::unique(cavityBorderEdges.begin(), cavityBorderEdges.end()), cavityBorderEdges.end());

                cavityEdges.emplace(cavitySeed, std::move(cavityBorderEdges));
            }

            return cavityEdges;
        }

        static void formCavityLoop(HalfEdgeMesh* mesh, std::vector<EdgeIndex>& cavityEdges) {
            std::vector<uint8_t> isCavityBoundary(mesh->numberOfEdges(),0);
            for (EdgeIndex e : cavityEdges) {
                isCavityBoundary[e] = 1;
            }
            std::vector<EdgeIndex> loopedBoundary;
            loopedBoundary.reserve(cavityEdges.size());
            EdgeIndex firstEdge = cavityEdges[0];
            FaceIndex cavityFace = mesh->getFaceOfEdge(firstEdge);
            //mesh->setFaceToEdge(cavityFace, firstEdge);
            EdgeIndex currentEdge = firstEdge;
            loopedBoundary.push_back(currentEdge);
            do {
                EdgeIndex nextEdge = mesh->next(currentEdge);
                while (!isCavityBoundary[nextEdge]) {
                    nextEdge = mesh->CWEdgeToVertex(nextEdge);
                }
                mesh->setPrev(nextEdge,currentEdge);
                mesh->setNext(currentEdge,nextEdge);
                mesh->setEdgeToFace(nextEdge, cavityFace);
                loopedBoundary.push_back(nextEdge);
                currentEdge = nextEdge;
            } while (firstEdge != currentEdge);
            cavityEdges = std::move(loopedBoundary);
        }

        static std::array<EdgeIndex,3> getEdges(HalfEdgeMesh* outputMesh, FaceIndex triangle) {
            EdgeIndex edge1 = outputMesh->getPolygon(triangle);
            EdgeIndex edge2 = outputMesh->next(edge1);
            EdgeIndex edge3 = outputMesh->next(edge2);
            return {edge1, edge2, edge3};
        }

        static void insertCavity(HalfEdgeMesh* outputMesh, std::unordered_map<int, std::vector<EdgeIndex>>& cavityMap) {
            size_t faceCount = outputMesh->numberOfPolygons();
            size_t edgeCount = outputMesh->numberOfEdges();
            std::unordered_map<int,EdgeIndex> faceToEdge;
            faceToEdge.reserve(cavityMap.size());
            for (const auto& [faceIdx, _] : cavityMap) {
                faceToEdge[faceIdx] = outputMesh->getPolygon(faceIdx);
            }
            for (const auto& [faceIdx, edgesToKeep] : cavityMap) {
                EdgeIndex currentEdge = faceToEdge[faceIdx];
                EdgeIndex nextEdge = outputMesh->next(currentEdge);
                EdgeIndex lastEdge = outputMesh->next(nextEdge);
                
                auto contains = [&edgesToKeep](EdgeIndex e) {
                    return std::binary_search(edgesToKeep.begin(), edgesToKeep.end(), e);
                };
                
                
                auto reconnectPreviousAndNextEdge = [&](EdgeIndex edgeToRemove) {
                    // First we check if it wasn't removed already
                    if (outputMesh->origin(edgeToRemove) == -1) return;
                    // The previous edge's next edge, is the CW edge to the removed edge
                    EdgeIndex prevEdgeIdx = outputMesh->prev(edgeToRemove);
                    EdgeIndex newNext = outputMesh->CWEdgeToVertex(edgeToRemove);
                    while (outputMesh->origin(newNext) == -1) {
                        newNext = outputMesh->CWEdgeToVertex(newNext);
                    }
                    if (newNext == edgeToRemove) {
                        return;
                    }
                    outputMesh->setNext(prevEdgeIdx, newNext);
                    outputMesh->setPrev(newNext, prevEdgeIdx);

                    //EdgeIndex nextEdgeIdx = outputMesh->next(edgeToRemove);
                    //EdgeIndex newPrev = outputMesh->prev(outputMesh->twin(edgeToRemove));
                    //outputMesh->setPrev(nextEdgeIdx, newPrev);
                    HEVertex& originOfRemoved = outputMesh->getVertex(outputMesh->origin(edgeToRemove));
                    // This "removes" a reference to the edge in the mesh
                    originOfRemoved.incidentHalfEdge = newNext;
                    // We remove this edge by marking its origin as -1
                    HalfEdge& removedEdge = outputMesh->getEdge(edgeToRemove);
                    removedEdge.origin = -1;
                    edgeCount-= 1;
                };

                for (EdgeIndex edge : {currentEdge, nextEdge, lastEdge}) {
                    if (!contains(edge)) {
                        reconnectPreviousAndNextEdge(edge);
                    }
                }
                for (EdgeIndex edge : {currentEdge, nextEdge, lastEdge}) {
                    if (!contains(edge)) {
                        reconnectPreviousAndNextEdge(outputMesh->twin(edge));
                    }
                }
                // Now assign `.face` to all edges still part of this face
                std::unordered_set<EdgeIndex> seen;
                if (!edgesToKeep.empty()) {
                    EdgeIndex startEdge = *edgesToKeep.begin();
                    EdgeIndex walker = startEdge;
                    do {
                        outputMesh->getEdge(walker).face = startEdge;
                        if (seen.contains(walker)) {
                            break;
                        }
                        seen.insert(walker);
                        walker = outputMesh->next(walker);
                    } while (walker != startEdge);
                } else {
                    faceCount--; // Entire face was deleted
                }
            }
            outputMesh->updateEdgeCount(edgeCount);
            outputMesh->updatePolygonCount(faceCount);
        }
    };
}

#endif