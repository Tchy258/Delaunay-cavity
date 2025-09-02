#ifndef DELAUNAY_CAVITY_REFINER_IPP
#define DELAUNAY_CAVITY_REFINER_IPP
#ifndef DELAUNAY_CAVITY_REFINER_HPP
#include <mesh_refiners/delaunay_cavity_refiner.hpp>
#endif

#define MAXIMUM_SECOND_DEGREE_NEIGHBORS 10

template <MeshData MeshType, RefinementCriterion<MeshType> Criterion>
std::vector<std::pair<typename MeshType::VertexType,int>> DelaunayCavityRefiner<MeshType,Criterion>::findMatchingCircumcenters(MeshType* outputMesh, size_t polygonAmount) {
    std::vector<std::pair<MeshVertex,int>> circumcenters;
    circumcenters.reserve(polygonAmount);
    // Marking phase, first the circumcenters of each matching triangle is found
    for (int i = 0; i < polygonAmount; ++i) {
        if (refinementCriterion(*outputMesh, i)) {
            Vertex v0,v1,v2;
            outputMesh->getVerticesOfTriangle(i, v0, v1, v2);
            circumcenters.push_back(std::pair<MeshVertex,int>(Vertex::findCircumcenter(v0,v1,v2),i));
        }
    }
    return circumcenters;
}

template <MeshData MeshType, RefinementCriterion<MeshType> Criterion>
std::vector<int> DelaunayCavityRefiner<MeshType,Criterion>::computeCavity(MeshType* outputMesh, const std::pair<MeshVertex, int>& circumcenterData, std::vector<uint8_t>& visited) {   
    using TriangleIndex = int;

    const MeshVertex& circumcenter = circumcenterData.first;
    TriangleIndex triangleOfCircumcenter = circumcenterData.second;

    std::queue<TriangleIndex> bfsNeighborVisitQueue;
    std::vector<TriangleIndex> cavity;
    cavity.reserve(MAXIMUM_SECOND_DEGREE_NEIGHBORS);

    bfsNeighborVisitQueue.push(triangleOfCircumcenter);

    visited[triangleOfCircumcenter] = 1;

    while(!bfsNeighborVisitQueue.empty()) {
        TriangleIndex currentTriangle = bfsNeighborVisitQueue.front();
        bfsNeighborVisitQueue.pop();
        cavity.push_back(currentTriangle);

        const std::vector<TriangleIndex>& neighbors = outputMesh->getNeighbors(currentTriangle);
        for (int neighbor : neighbors) {
            if (visited[neighbor]) continue;

            Vertex v0, v1, v2;
            outputMesh->getVerticesOfTriangle(neighbor, v0, v1, v2);

            if (Vertex::inCircle(v0, v1, v2, circumcenter)) {
                visited[neighbor] = 1;
                bfsNeighborVisitQueue.push(neighbor);
            }
        }
    }
    // Sorted for faster lookups later
    std::sort(cavity.begin(), cavity.end());
    return cavity;
}

template <MeshData MeshType, RefinementCriterion<MeshType> Criterion>
MeshType* DelaunayCavityRefiner<MeshType, Criterion>::refineMesh(const MeshType* inputMesh) {
    using TriangleIndex = int;
    MeshType* outputMesh = new MeshType(*inputMesh);
    size_t polygonAmount = outputMesh->numberOfPolygons();
    std::vector<std::pair<MeshVertex,TriangleIndex>> circumcenters = findMatchingCircumcenters(outputMesh, polygonAmount);

    std::unordered_map<TriangleIndex, std::vector<TriangleIndex>> cavities;
    // Here we use a vector of uint8_t instead of a vector of bool for better performance at the cost of memory
    std::vector<uint8_t> visited(polygonAmount, 0);
    
    for (const auto& circumcenterData : circumcenters) {
        std::vector<TriangleIndex> cavity = computeCavity(outputMesh, circumcenterData, visited);
        cavities[circumcenterData.second] = cavity;
        resetVisited(visited, cavity);
    }
    // Edge selection phase, all edges of each cavity's border are chosen
    std::unordered_map<int, std::vector<typename MeshType::EdgeIndex>> cavityEdges = selectCavityEdges(outputMesh, cavities);
    insertCavity(outputMesh, cavityEdges);
    return outputMesh;
}


#undef MAXIMUM_SECOND_DEGREE_NEIGHBORS
#endif