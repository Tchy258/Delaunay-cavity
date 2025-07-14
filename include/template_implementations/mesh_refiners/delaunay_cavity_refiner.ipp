#ifndef DELAUNAY_CAVITY_REFINER_HPP
#include <mesh_refiners/delaunay_cavity_refiner.hpp>
#endif

template <MeshData MeshType, RefinementCriterion<MeshType> Criterion>
inline std::unordered_map<int, std::unordered_set<HalfEdgeMesh::EdgeIndex>> DelaunayCavityRefiner<MeshType, Criterion>::selectCavityEdges(HalfEdgeMesh& outMesh, std::unordered_map<int, std::vector<int>>& cavities) requires std::same_as<MeshType, HalfEdgeMesh> {
    using EdgeIndex = HalfEdgeMesh::EdgeIndex;
    std::unordered_map<int, std::unordered_set<EdgeIndex>>cavityEdges(cavities.size());
    for (const auto& cavityVecPair : cavities) {
        const std::vector<int>& cavityPolygons = cavityVecPair.second;
        // For all faces
        for (int val : cavityPolygons) {
            EdgeIndex firstEdge = outMesh.getPolygon(val);
            EdgeIndex currentEdge = outMesh.next(firstEdge);
            while (currentEdge != firstEdge) {
                // See if any edge is part of the original border, if so, it's also a border of the cavity
                if (outMesh.isBorderFace(currentEdge)) {
                    cavityEdges.try_emplace(val).first->second.insert(currentEdge);
                } else {
                    // If it wasn't a border of the cavity, then we need to check if its immediate neighbor (twin) is part
                    // of the cavity, and if it's not, then this edge is a border of the cavity
                    EdgeIndex twinEdge = outMesh.twin(currentEdge);
                    EdgeIndex twinNext = outMesh.next(twinEdge);
                    bool partOftheCavity = false;
                    // For all edges of this neighboring face
                    while (twinNext != twinEdge) {
                        // if this edge isn't one that identifies a neighbor, keep looking
                        if (std::find(cavityPolygons.begin(), cavityPolygons.end(), outMesh.getFaceOfEdge(twinNext)) == cavityPolygons.end()) {
                                twinNext = outMesh.next(twinNext);
                        // if it does, then it's part of the cavity and we should proceed
                        } else {
                            partOftheCavity = true;
                            break;
                        }
                    }
                    // check the first twin
                    if (!partOftheCavity) {
                        // If this is true, then none of the neighbor's edges were a part of the cavity, so this edge
                        // must be a border edge
                        if (std::find(cavityPolygons.begin(), cavityPolygons.end(), outMesh.getFaceOfEdge(twinEdge)) == cavityPolygons.end()) {
                                cavityEdges.try_emplace(val).first->second.insert(currentEdge);
                        }
                    }
                }
                currentEdge = outMesh.next(currentEdge);
            }
        }
    }
    return cavityEdges;
}

template <MeshData MeshType, RefinementCriterion<MeshType> Criterion>
void DelaunayCavityRefiner<MeshType, Criterion>::insertCavity(HalfEdgeMesh &outputMesh, std::unordered_map<int, std::unordered_set<HalfEdgeMesh::EdgeIndex>>& cavityMap) requires std::same_as<MeshType, HalfEdgeMesh> {
    using EdgeIndex = int;
    size_t faceCount = outputMesh.numberOfPolygons();
    size_t edgeCount = outputMesh.numberOfEdges();
    std::unordered_map<int,EdgeIndex> faceToEdge;
    faceToEdge.reserve(cavityMap.size());
    for (const auto& [faceIdx, _] : cavityMap) {
        faceToEdge[faceIdx] = outputMesh.getPolygon(faceIdx);
    }
    for (const auto& [faceIdx, edgesToKeep] : cavityMap) {
        EdgeIndex currentEdge = faceToEdge[faceIdx];
        EdgeIndex nextEdge = outputMesh.next(currentEdge);
        EdgeIndex lastEdge = outputMesh.next(nextEdge);
        
        auto contains = [&](EdgeIndex e) {
            return std::find(edgesToKeep.begin(), edgesToKeep.end(), e) != edgesToKeep.end();
        };
        
        
        auto reconnectPreviousAndNextEdge = [&](EdgeIndex edgeToRemove) {
            // First we check if it wasn't removed already
            if (outputMesh.origin(edgeToRemove) == -1) return;
            // The previous edge's next edge, is the CW edge to the removed edge
            EdgeIndex prevEdgeIdx = outputMesh.prev(edgeToRemove);
            EdgeIndex newNext = outputMesh.CWEdgeToVertex(edgeToRemove);
            while (outputMesh.origin(newNext) == -1) {
                newNext = outputMesh.CWEdgeToVertex(newNext);
            }
            if (newNext == edgeToRemove) {
                return;
            }
            outputMesh.setNext(prevEdgeIdx, newNext);
            outputMesh.setPrev(newNext, prevEdgeIdx);

            //EdgeIndex nextEdgeIdx = outputMesh.next(edgeToRemove);
            //EdgeIndex newPrev = outputMesh.prev(outputMesh.twin(edgeToRemove));
            //outputMesh.setPrev(nextEdgeIdx, newPrev);
            HEVertex& originOfRemoved = outputMesh.getVertex(outputMesh.origin(edgeToRemove));
            // This "removes" a reference to the edge in the mesh
            originOfRemoved.incidentHalfEdge = newNext;
            // We remove this edge by marking its origin as -1
            HalfEdge& removedEdge = outputMesh.getEdge(edgeToRemove);
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
                reconnectPreviousAndNextEdge(outputMesh.twin(edge));
            }
        }
        // Now assign `.face` to all edges still part of this face
        std::unordered_set<EdgeIndex> seen;
        if (!edgesToKeep.empty()) {
            EdgeIndex startEdge = *edgesToKeep.begin();
            EdgeIndex walker = startEdge;
            do {
                outputMesh.getEdge(walker).face = startEdge;
                if (seen.contains(walker)) {
                    break;
                }
                seen.insert(walker);
                walker = outputMesh.next(walker);
            } while (walker != startEdge);
        } else {
            faceCount--; // Entire face was deleted
        }
    }
    outputMesh.updateEdgeCount(edgeCount);
    outputMesh.updatePolygonCount(faceCount);
}

template <MeshData MeshType, RefinementCriterion<MeshType> Criterion>
MeshType DelaunayCavityRefiner<MeshType, Criterion>::refineMesh(const MeshType& inputMesh) {
    MeshType outMesh = MeshType(inputMesh);
    size_t polygonAmount = outMesh.numberOfPolygons();
    std::vector<std::pair<MeshVertex,int>> circumcenters;
    // Marking phase, first the circumcenters of each matching triangle is found
    for (int i = 0; i < polygonAmount; ++i) {
        if (refinementCriterion(outMesh, i)) {
            Vertex v0,v1,v2;
            outMesh.getVerticesOfTriangle(i, v0, v1, v2);
            circumcenters.push_back(std::pair<MeshVertex,int>(Vertex::findCircumcenter(v0,v1,v2),i));
        }
    }
    // Propagation phase, the triangles that are part of the cavity are found
    std::unordered_map<int, std::vector<int>> cavities;
    for (int k = 0; k < circumcenters.size(); ++k) {
        MeshVertex currentCircumcenter = circumcenters[k].first;
        int cavityIndex = circumcenters[k].second;
        std::vector<int> neighbors = outMesh.getNeighbors(circumcenters.at(k).second);
        for (int i = 0; i < neighbors.size(); ++i) {
            int polygonIndex = neighbors[i];
            MeshVertex v0,v1,v2;
            outMesh.getVerticesOfTriangle(polygonIndex, v0,v1,v2);
            if (Vertex::inCircle(v0,v1,v2,currentCircumcenter)) {
                cavities.try_emplace(cavityIndex).first->second.push_back(polygonIndex);
            }
        }
        // The triangle that has this circumcenter is part of the cavity too
        cavities.try_emplace(cavityIndex).first->second.push_back(circumcenters.at(k).second);
        // Sorted for easier lookups later
        std::sort(cavities[cavityIndex].begin(), cavities[cavityIndex].end());
    }
    // Edge selection phase, all edges of each cavity's border is chosen
    std::unordered_map<int, std::unordered_set<typename MeshType::EdgeIndex>> cavityEdges = selectCavityEdges(outMesh, cavities);
    insertCavity(outMesh, cavityEdges);
    return outMesh;
}
