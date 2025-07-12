#ifndef DELAUNAY_CAVITY_REFINER_HPP
#include <mesh_refiners/delaunay_cavity_refiner.hpp>
#endif
#define EPSILON 1e-9

template <MeshData MeshType, RefinementCriterion<MeshType> Criterion>
DelaunayCavityRefiner<MeshType, Criterion>::MeshVertex DelaunayCavityRefiner<MeshType, Criterion>::findCircumcenter(DelaunayCavityRefiner<MeshType, Criterion>::MeshVertex vertex0, DelaunayCavityRefiner<MeshType, Criterion>::MeshVertex vertex1, DelaunayCavityRefiner<MeshType, Criterion>::MeshVertex vertex2) {
    MeshVertex origin;
    origin.x = 0;
    origin.y = 0;
    MeshVertex vertex1Displaced = vertex1 - vertex0;
    MeshVertex vertex2Displaced = vertex2 - vertex0;
    double determinant = 2 * vertex1Displaced.cross2d(vertex2Displaced);
    if (std::fabs(determinant) < EPISLON ) {
        throw std::runtime_error("Degenerate triangle found\n Vertices: \n" \
            + std::string(vertex0.x) + "," + std::string(vertex0.y) + "\n " \
            + std::string(vertex1.x) + "," + std::string(vertex1.y) + "\n " \
            + std::string(vertex2.x) + "," + std::string(vertex2.y));
    }
    // 1/D [ C_y * (B_x^2 + B_y^2) - B_y * (C_x^2 + C_y^2)]
    double circumcenterX = (vertex2Displaced.y * vertex1Displaced.dot(vertex1Displaced) - vertex1Displaced.y * vertex2Displaced.dot(vertex2Displaced)) / determinant;
    double circumcenterY = (vertex1Displaced.x * vertex2Displaced.dot(vertex2Displaced) - vertex2Displaced.x * vertex1Displaced.dot(vertex1Displaced)) / determinant;
    circumcenterX += vertex0.x;
    circumcenterY += vertex0.y;
    MeshVertex circumcenter;
    circumcenter.x = circumcenterX;
    circumcenter.y = circumcenterY;
    return circumcenter;
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
            if constexpr (IsHalfEdgeVertex<MeshVertex>) {
                v0 = outMesh.getVertex(outMesh.origin(polygonIndex));
                v1 = outMesh.getVertex(outMesh.target(polygonIndex));
                v2 = outMesh.getVertex(outMesh.target(outMesh.next(polygonIndex)));
            } else {
                // TODO: Logic to find triangle vertices on other, non half-edge meshes
            }
            circumcenters.push_back({findCircumcenter(v0,v1,v2),polygonIndex});
        }
    }
    // Propagation phase, the triangles that are part of the cavity are found
    std::unordered_map<int, std::vector<int>> cavities;
    for (int k = 0; k < circumcenters.size(); ++k) {
        MeshVertex currentCircumcenter = circumcenters[k].first;
        int cavityIndex = circumcenters[k].second;
        std::vector<int> neighbors = outMesh.getNeighbors(circumcenters.at(k).second);
        for (int i = 0; i < neighbors.size(); ++i) {
            if constexpr (IsHalfEdgeVertex<MeshVertex>) {
                MeshEdge polygonIndex = neighbors[i];
                MeshVertex v0 = outMesh.getVertex(outMesh.origin(polygonIndex));
                MeshVertex v1 = outMesh.getVertex(outMesh.target(polygonIndex));
                MeshVertex v2 = outMesh.getVertex(outMesh.target(outMesh.next(polygonIndex)));
                if (Vertex::inCircle(v0,v1,v2,currentCircumcenter)) {
                    cavities.try_emplace(cavityIndex).first->second.push_back(polygonIndex);
                }
            } else {
                // TODO: Logic for non half edge meshes
            }
        }
        // The triangle that has this circumcenter is part of the cavity too
        cavities.try_emplace(cavityIndex).first->second.push_back(circumcenters.at(k).second);
        // Sorted for easier lookups later
        std::sort(cavities[cavityIndex].begin(), cavities[cavityIndex].end());
    }
    using EdgeIndex = typename MeshData::EdgeIndex;
    std::unordered_map<int, std::vector<EdgeIndex>> cavityEdges(cavities.size());
    // Edge selection phase, all edges of each cavity's border is chosen
    if constexpr (IsHalfEdgeVertex<MeshVertex>) {
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
    } else {
        // TODO: logic for non half edge meshes
    }
    outMesh.cavityInsertion(cavityEdges);
    return outMesh;
}

#undef EPSILON