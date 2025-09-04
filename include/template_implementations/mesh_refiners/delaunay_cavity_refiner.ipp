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
std::vector<std::vector<typename MeshType::FaceIndex>> DelaunayCavityRefiner<MeshType,Criterion>::computeCavity(MeshType* outputMesh, const std::pair<MeshVertex, FaceIndex>& circumcenterData, std::vector<uint8_t>& visited) {   

    const MeshVertex& circumcenter = circumcenterData.first;
    FaceIndex triangleOfCircumcenter = circumcenterData.second;

    std::queue<CavityNode> bfsNeighborVisitQueue;
    std::vector<std::vector<FaceIndex>> cavity(1);
    cavity[0].reserve(3);
    bfsNeighborVisitQueue.push(CavityNode{triangleOfCircumcenter, 0});

    visited[triangleOfCircumcenter] = 1;

    while(!bfsNeighborVisitQueue.empty()) {
        CavityNode currentTriangle = bfsNeighborVisitQueue.front();
        bfsNeighborVisitQueue.pop();
        const std::vector<FaceIndex>& neighbors = outputMesh->getNeighbors(currentTriangle.index);
        for (FaceIndex neighbor : neighbors) {
            if (visited[neighbor]) continue;
            
            Vertex v0, v1, v2;
            outputMesh->getVerticesOfTriangle(neighbor, v0, v1, v2);
            
            if (Vertex::inCircle(v0, v1, v2, circumcenter)) {
                unsigned int newLevel = currentTriangle.level + 1;
                visited[neighbor] = 1;
                bfsNeighborVisitQueue.push(CavityNode{neighbor,newLevel});
                if (cavity.size() <= newLevel) {
                    cavity.push_back(std::vector<FaceIndex>{});
                    size_t maximumPossibleNeighborsOnThisLevel = 1;
                    for (unsigned int i = 0; i < newLevel; ++i) {
                        maximumPossibleNeighborsOnThisLevel *= 3;
                    }
                    cavity[newLevel].reserve(maximumPossibleNeighborsOnThisLevel);
                }
                cavity[newLevel].push_back(neighbor);
            }
        }
    }
    for (int i = 0; i < cavity.size(); ++i) {
        cavity[i].shrink_to_fit();
        std::sort(cavity[i].begin(), cavity[i].end());
    }

    return cavity;
}

template <MeshData MeshType, RefinementCriterion<MeshType> Criterion>
MeshType* DelaunayCavityRefiner<MeshType, Criterion>::refineMesh(const MeshType* inputMesh) {
    MeshType* outputMesh = new MeshType(*inputMesh);
    size_t polygonAmount = outputMesh->numberOfPolygons();
    std::vector<std::pair<MeshVertex,FaceIndex>> circumcenters = findMatchingCircumcenters(outputMesh, polygonAmount);

    std::unordered_map<FaceIndex, std::vector<std::vector<FaceIndex>>> cavities;
    // Here we use a vector of uint8_t instead of a vector of bool for better performance at the cost of memory
    std::vector<uint8_t> visited(polygonAmount, 0);
    
    for (const auto& circumcenterData : circumcenters) {
        std::vector<std::vector<FaceIndex>> cavity = computeCavity(outputMesh, circumcenterData, visited);
        cavities[circumcenterData.second] = cavity;
        resetVisited(visited, cavity);
    }
    // Edge selection phase, all edges of each cavity's border are chosen
    std::vector<MeshEdge> cavityEdges = selectCavityEdges2(outputMesh, cavities);
    //insertCavity(outputMesh, cavityEdges);
    return outputMesh;
}


#undef MAXIMUM_SECOND_DEGREE_NEIGHBORS
#endif