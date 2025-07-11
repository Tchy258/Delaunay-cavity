#ifndef DELAUNAY_CAVITY_REFINER_HPP
#include <mesh_refiners/delaunay_cavity_refiner.hpp>
#endif
#define EPSILON 1e-9

template <MeshData MeshType, RefinementCriterion<MeshType> Criterion>
DelaunayCavityRefiner<MeshType, Criterion>::MeshVertex DelaunayCavityRefiner<MeshType, Criterion>::findCircumcircle(DelaunayCavityRefiner<MeshType, Criterion>::MeshVertex vertex0, DelaunayCavityRefiner<MeshType, Criterion>::MeshVertex vertex1, DelaunayCavityRefiner<MeshType, Criterion>::MeshVertex vertex2) {
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
    double circumcenterRadius = std::sqrt(std::pow(circumcenterX,2) + std::pow(circumcenterY,2));
    circumcenterX += vertex0.x;
    circumcenterY += vertex0.y;
    MeshVertex circumcenter;
    circumcenter.x = circumcenterX;
    circumcenter.y = circumcenterY;
    return circumcenter;
}
template <MeshData MeshType, RefinementCriterion<MeshType> Criterion>
MeshType DelaunayCavityRefiner<MeshType, Criterion>::refineMesh(MeshType inputMesh) {
    size_t polygonAmount = inputMesh.numberOfPolygons();
    std::vector<std::pair<MeshVertex,int>> circumcenters;
    for (int i = 0; i < polygonAmount; ++i) {
        int polygonIndex = inputMesh.getPolygon(i);
        if (refinementCriterion(inputMesh, polygonIndex)) {
            if constexpr (IsHalfEdgeVertex<MeshVertex>) {
                MeshVertex v0 = inputMesh.getVertex(inputMesh.origin(polygonIndex));
                MeshVertex v1 = inputMesh.getVertex(inputMesh.target(polygonIndex));
                MeshVertex v2 = inputMesh.getVertex(inputMesh.target(inputMesh.next(polygonIndex)));
                circumcenters.push_back({findCircumcircle(v0,v1,v2),polygonIndex});
            } else {
                // TODO: Logic to find triangle vertices on other, non half-edge meshes
            }
        }
    }
    std::unordered_map<MeshVertex, std::vector<int>> cavities;
    for (int k = 0; k < circumcenters.size(); ++k) {
        MeshVertex currentCircumcenter = circumcenters[k].first;
        std::vector<int> neighbors = inputMesh.getNeighbors(circumcenters.at(k).second);
        for (int i = 0; i < neighbors.size(); ++i) {
            if constexpr (IsHalfEdgeVertex<MeshVertex>) {
                MeshEdge polygonIndex = neighbors[i];
                MeshVertex v0 = inputMesh.getVertex(inputMesh.origin(polygonIndex));
                MeshVertex v1 = inputMesh.getVertex(inputMesh.target(polygonIndex));
                MeshVertex v2 = inputMesh.getVertex(inputMesh.target(inputMesh.next(polygonIndex)));
                if (Vertex::inCircle(v0,v1,v2,currentCircumcenter)) {
                    cavities.try_emplace(currentCircumcenter).first->second.push_back(polygonIndex);
                }
            } else {
                // TODO: Logic for non half edge meshes
            }
        }
        // The triangle that has this circumcenter is part of the cavity too
        cavities.try_emplace(currentCircumcenter).first->second.push_back(circumcenters.at(k).second);
        // Sorted for easier lookups later
        std::sort(cavities[currentCircumcenter].begin(), cavities[currentCircumcenter].end());
    }
    std::vector<MeshVertex> cavityVertices;
    std::vector<MeshEdge> cavityEdges;
    for (auto it = myMap.begin(); it != myMap.end(); ++it) {
        const std::vector<int>& cavityPolygons = it->second;
        if constexpr (IsHalfEdgeVertex<MeshVertex>) {
            // For all faces
            for (int val : cavityPolygons) {
                MeshData::EdgeIndex firstEdge = val;
                MeshData::EdgeIndex currentEdge = inputMesh.next(firstEdge);
                while (currentEdge != firstEdge) {
                    // See if any edge is part of the original border, if so, it's also a border of the cavity
                    if (inputMesh.isBorderFace(currentEdge)) {
                        cavityEdges.push_back(currentEdge);
                    } else {
                        // If it wasn't a border of the cavity, then we need to check if its immediate neighbor (twin) is part
                        // of the cavity, and if it's not, then this edge is a border of the cavity
                        MeshData::EdgeIndex twinEdge = inputMesh.twin(currentEdge);
                        MeshData::EdgeIndex twinNext = inputMesh.next(twinEdge);
                        bool partOftheCavity = false;
                        // For all edges of this neighboring face
                        while (twinNext != twinEdge) {
                            // if this edge isn't one that identifies a neighbor, keep looking
                            if (std::find(cavityPolygons.begin(), cavityPolygons.end(), twinNext) == cavityPolygons.end()) {
                                    twinNext = inputMesh.next(twinNext);
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
                                    cavityEdges.push_back(currentEdge);
                            }
                        }
                    }
                }
                
            }
        } else {
            // TODO: logic for non half edge meshes
        }
    }
    for (int i = 0; i < cavityEdges.size(); ++i) {
        MeshVertex origin = inputMesh.getVertex(inputMesh.origin(cavityEdges[i]));
        MeshVertex copy = MeshVertex(origin);
        // TODO: complete the logic for making the final polygon described by this cavity
    }
}

#undef EPSILON