#ifndef DELAUNAY_CAVITY_REFINER_HPP
#include <mesh_refiners/delaunay_cavity_refiner.hpp>
#include "delaunay_cavity_refiner.hpp"
#endif

template <MeshData MeshType, RefinementCriterion<MeshType> Criterion>
inline std::unordered_map<int, std::vector<HalfEdgeMesh::EdgeIndex>> DelaunayCavityRefiner<MeshType, Criterion>::selectCavityEdges(HalfEdgeMesh& outMesh, std::unordered_map<int, std::vector<int>>& cavities) requires std::same_as<MeshType, HalfEdgeMesh> {
    using EdgeIndex = HalfEdgeMesh::EdgeIndex;
    std::unordered_map<int, std::vector<EdgeIndex>>cavityEdges(cavities.size());
    for (const auto& cavityVecPair : cavities) {
        const std::vector<int>& cavityPolygons = cavityVecPair.second;
        // For all faces
        for (int val : cavityPolygons) {
            EdgeIndex firstEdge = val;
            EdgeIndex currentEdge = outMesh.next(firstEdge);
            while (currentEdge != firstEdge) {
                // See if any edge is part of the original border, if so, it's also a border of the cavity
                if (outMesh.isBorderFace(currentEdge)) {
                    cavityEdges.try_emplace(val).first->second.push_back(currentEdge);
                } else {
                    // If it wasn't a border of the cavity, then we need to check if its immediate neighbor (twin) is part
                    // of the cavity, and if it's not, then this edge is a border of the cavity
                    EdgeIndex twinEdge = outMesh.twin(currentEdge);
                    EdgeIndex twinNext = outMesh.next(twinEdge);
                    bool partOftheCavity = false;
                    // For all edges of this neighboring face
                    while (twinNext != twinEdge) {
                        // if this edge isn't one that identifies a neighbor, keep looking
                        if (std::find(cavityPolygons.begin(), cavityPolygons.end(), twinNext) == cavityPolygons.end()) {
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
                        if (std::find(cavityPolygons.begin(), cavityPolygons.end(), twinEdge) == cavityPolygons.end()) {
                                cavityEdges.try_emplace(val).first->second.push_back(currentEdge);
                        }
                    }
                }
            }
        }
    }
    return cavityEdges;
}

template <MeshData MeshType, RefinementCriterion<MeshType> Criterion>
void DelaunayCavityRefiner<MeshType, Criterion>::insertCavity(HalfEdgeMesh &outputMesh, std::unordered_map<int, std::vector<HalfEdgeMesh::EdgeIndex>> cavityMap) requires std::same_as<MeshType, HalfEdgeMesh> {
    using EdgeIndex = int;
    size_t faceCount = outputMesh.numberOfPolygons();
    size_t edgeCount = outputMesh.numberOfEdges();
    for (const auto& faceEdgesPair : cavityMap) {
        int polygonIndex = faceEdgesPair.first;
        EdgeIndex currentEdge = polygonIndex;
        EdgeIndex nextEdge = outputMesh.next(currentEdge);
        EdgeIndex lastEdge = outputMesh.next(nextEdge);
        
        bool has_current = false, has_next = false, has_last = false;
        // It is still a triangle so it has 3 edges
        std::vector<EdgeIndex> edgesToKeep = faceEdgesPair.second;
        for (int i = 0; i < 3; ++i) {
            if (edgesToKeep[i] == currentEdge) {
                has_current = true;
            } else if (edgesToKeep[i] == nextEdge) {
                has_next = true;
            } else if (edgesToKeep[i] == lastEdge) {
                has_last = true;
            }
        }
        auto reconnectPreviousAndNextEdge = [=](EdgeIndex edgeToRemove) {
            // The previous edge's next edge, is the CW edge to the removed edge
            EdgeIndex prevEdgeIdx = outputMesh.prev(edgeToRemove);
            EdgeIndex newNext = outputMesh.CWEdgeToVertex(edgeToRemove);
            outputMesh.setNext(prevEdgeIdx, newNext);
            // And the next edge's previous edge, is the next edge of the removed edge's CW edge
            EdgeIndex nextEdgeIdx = outputMesh.next(edgeToRemove);
            EdgeIndex newPrev = outputMesh.next(newNext);
            outputMesh.setPrev(nextEdgeIdx, newPrev);
            HEVertex originOfRemoved = outputMesh.origin(edgeToRemove);
            // This "removes" a reference to the edge in the mesh
            originOfRemoved.incidentHalfEdge = newNext;
            // We remove this edge and its twin
            edgeCount-= 2;
        };
        if (!has_current) {
            reconnectPreviousAndNextEdge(currentEdge);
            outputMesh.setFace(polygonIndex, outputMesh.CWEdgeToVertex(currentEdge));
            faceCount--;
        }
        if (!has_next) {
            reconnectPreviousAndNextEdge(nextEdge);
        }
        if (!has_last) {
            reconnectPreviousAndNextEdge(lastEdge);
        }
    }
    outputMesh.updateEdgeCount(edgeCount);
    outputMesh.updatePolygonCount(faceCount);
    return outputMesh;
}

template <MeshData MeshType, RefinementCriterion<MeshType> Criterion>
MeshType DelaunayCavityRefiner<MeshType, Criterion>::refineMesh(const MeshType& inputMesh) {
    MeshType outMesh = MeshType(inputMesh);
    size_t polygonAmount = outMesh.numberOfPolygons();
    std::vector<std::pair<MeshVertex,int>> circumcenters;
    // Marking phase, first the circumcenters of each matching triangle is found
    for (int i = 0; i < polygonAmount; ++i) {
        int polygonIndex = outMesh.getPolygon(i);
        if (refinementCriterion(outMesh, polygonIndex)) {
            MeshVertex v0,v1,v2;
            outMesh.getVerticesOfTriangle(polygonIndex, v0, v1, v2);
            circumcenters.push_back({Vertex::findCircumcenter(v0,v1,v2),polygonIndex});
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
    std::unordered_map<int, std::vector<typename MeshData::EdgeIndex>> cavityEdges = selectCavityEdges(outMesh, cavities);
    insertCavity(outMesh, cavityEdges);
    return outMesh;
}
