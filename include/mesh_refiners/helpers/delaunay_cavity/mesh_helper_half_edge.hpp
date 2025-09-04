#ifndef MESH_HELPER_HALF_EDGE_DELAUNAY_CAVITY_HPP
#define MESH_HELPER_HALF_EDGE_DELAUNAY_CAVITY_HPP

#ifndef MESH_HELPER_HPP // This is just for intellisense
#include <mesh_refiners/helpers/mesh_helper.hpp>
#endif
#include <mesh_data/half_edge_mesh.hpp>

namespace refiners::helpers::delaunay_cavity {

    template <MeshData MeshType>
    struct MeshHelper;

    template <>
    struct MeshHelper<HalfEdgeMesh> {
        using EdgeIndex = HalfEdgeMesh::EdgeIndex;
        using FaceIndex = HalfEdgeMesh::FaceIndex;
        using VertexIndex = HalfEdgeMesh::VertexIndex;

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

        static std::vector<HalfEdge> selectCavityEdges2(HalfEdgeMesh* mesh, const std::unordered_map<FaceIndex, std::vector<std::vector<FaceIndex>>>& cavities) {
            std::vector<HalfEdge> newEdges = mesh->getEdges();
            size_t edgeCount = mesh->numberOfEdges();
            size_t faceCount = mesh->numberOfPolygons();
            size_t vertexCount = mesh->numberOfVertices();
            for (const auto& [cavitySeed, cavityPolygons] : cavities) {
                std::vector<EdgeIndex> edgesToKeep;
                int outerMostLevel = cavityPolygons.size() - 1;
                edgesToKeep.reserve(outerMostLevel * 6);
                auto inCavity = [&cavityPolygons](FaceIndex cavityFace, int level) {
                    return std::binary_search(cavityPolygons[level].begin(), cavityPolygons[level].end(), cavityFace);
                };

                auto isCavityBorder = [&cavityPolygons, &mesh, &inCavity](EdgeIndex edge, int level) {
                    if (mesh->isBorderEdge(edge)) return true;
                    FaceIndex faceOfTwin = mesh->getFaceOfEdge(mesh->twin(edge));
                    for (int i = level; i >= 0; --i) {
                        if (inCavity(faceOfTwin,i)) {
                            return false;
                        }
                    }
                    return true;
                };

                // Retrieve some edge of a triangle that is at the end of the cavity
                FaceIndex last = cavityPolygons.at(outerMostLevel)[0];
                EdgeIndex firstEdge = mesh->getPolygon(last);
                EdgeIndex currentEdge = firstEdge;
                do {
                    // If this is a border edge or its twin isn't part of the cavity, then this will be the first edge
                    // Since this edge comes from the outermost level of the cavity, this is guaranteed to happen at some point
                    if (isCavityBorder(currentEdge,outerMostLevel)) {
                        firstEdge = currentEdge;
                        break;
                    }
                    currentEdge = mesh->next(currentEdge);
                } while(currentEdge != firstEdge);
                edgesToKeep.push_back(firstEdge);
                FaceIndex newCavityFace = mesh->getFaceOfEdge(firstEdge);
                // Building the cavity
                do {
                    EdgeIndex newNext = mesh->next(currentEdge);
                    while (!isCavityBorder(newNext,outerMostLevel)) {
                        newNext = mesh->CWEdgeToVertex(newNext);
                    }
                    if (newNext != newEdges[currentEdge].next) {
                        newEdges[newEdges[currentEdge].next].origin = -1;
                        newEdges[newEdges[currentEdge].next].face = -1;
                        newEdges[currentEdge].next = newNext;
                        newEdges[newNext].prev = currentEdge;
                        newEdges[newNext].face = newCavityFace;
                        --edgeCount;
                    }
                    currentEdge = newNext;
                    edgesToKeep.push_back(newNext);
                } while(currentEdge != firstEdge);
                std::sort(edgesToKeep.begin(), edgesToKeep.end());
                // At this point, newEdges should have all edges of the cavity properly connected
                // So now we start destroying the inner levels
                for (int i = outerMostLevel - 1; i >=0; --i) {
                    const std::vector<FaceIndex>& innerTriangles = cavityPolygons[i];
                    for (FaceIndex face : innerTriangles) {
                        uint8_t edgesRemaining = 3;
                        EdgeIndex edge1 = mesh->getPolygon(face);
                        EdgeIndex edge2 = mesh->next(edge1);
                        EdgeIndex edge3 = mesh->next(edge2);
                        for (EdgeIndex e : {edge1, edge2, edge3}) {
                            if (mesh->isBorderEdge(e)) continue;
                            if (!std::binary_search(edgesToKeep.begin(), edgesToKeep.end(), e)) {
                                newEdges[e].origin = -1;
                                newEdges[e].face = -1;
                                --edgesRemaining;
                                --edgeCount;
                            }
                        }
                        if (edgesRemaining == 0) {
                            --faceCount;
                        }
                    }
                }
            }
            mesh->updateEdgeCount(edgeCount);
            mesh->updatePolygonCount(faceCount);
            mesh->setEdges(newEdges);
            return newEdges;
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