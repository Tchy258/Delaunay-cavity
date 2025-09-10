#ifndef DELAUNAY_CAVITY_REFINER_IPP
#define DELAUNAY_CAVITY_REFINER_IPP
#ifndef DELAUNAY_CAVITY_REFINER_HPP
#include <mesh_refiners/delaunay_cavity_refiner.hpp>
#include "delaunay_cavity_refiner.hpp"
#endif

template <MeshData MeshType, RefinementCriterion<MeshType> Criterion>
inline bool DelaunayCavityRefiner<MeshType, Criterion>::isCavityBoundaryEdge(MeshType *outputMesh, EdgeIndex edge, const std::vector<FaceIndex> &cavityFaces) {
    int countInside = 0;
    auto [face1, face2] = outputMesh->getFacesAssociatedWithEdge(edge);
    if (std::find(cavityFaces.begin(), cavityFaces.end(), face1) != cavityFaces.end()) {
        ++countInside;
    }
    if (std::find(cavityFaces.begin(), cavityFaces.end(), face2) != cavityFaces.end()) {
        ++countInside;
    }
     

    return (countInside == 1);
}

template <MeshData MeshType, RefinementCriterion<MeshType> Criterion>
std::vector<std::pair<typename MeshType::VertexType, int>> DelaunayCavityRefiner<MeshType, Criterion>::findMatchingCircumcenters(MeshType *outputMesh, size_t polygonAmount)
{
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
refiners::helpers::delaunay_cavity::Cavity<MeshType> DelaunayCavityRefiner<MeshType,Criterion>::computeCavity(MeshType* outputMesh, const std::pair<MeshVertex, FaceIndex>& circumcenterData, std::vector<uint8_t>& visited) {   

    const MeshVertex& circumcenter = circumcenterData.first;
    FaceIndex triangleOfCircumcenter = circumcenterData.second;

    std::queue<FaceIndex> bfsNeighborVisitQueue;
    Cavity cavity;

    bfsNeighborVisitQueue.push(triangleOfCircumcenter);

    visited[triangleOfCircumcenter] = 1;

    cavity.allTriangles.push_back(triangleOfCircumcenter);

    while(!bfsNeighborVisitQueue.empty()) {
        FaceIndex currentTriangle = bfsNeighborVisitQueue.front();
        bfsNeighborVisitQueue.pop();
        const std::vector<FaceIndex>& neighbors = outputMesh->getNeighbors(currentTriangle);
        std::array<EdgeIndex, 3> triangleEdges = outputMesh->getEdgesOfTriangle(currentTriangle);
        bool isBoundary = false;

        if (neighbors.size() < 3) {
            for (EdgeIndex e : triangleEdges) {
                if (outputMesh->isBorderEdge(e)) {
                    isBoundary = true;
                    cavity.boundaryEdges.push_back(e);
                }
            }
        }
        for (FaceIndex neighbor : neighbors) {
            if (visited[neighbor]) continue;
            
            Vertex v0, v1, v2;
            outputMesh->getVerticesOfTriangle(neighbor, v0, v1, v2);
            
            if (Vertex::inCircle(v0, v1, v2, circumcenter)) {
                visited[neighbor] = 1;
                bfsNeighborVisitQueue.push(neighbor);
                cavity.allTriangles.push_back(neighbor);
            } else {
                isBoundary = true;

                EdgeIndex boundaryEdge = outputMesh->getSharedEdge(currentTriangle,neighbor);
                cavity.boundaryEdges.push_back(boundaryEdge);
            }
        }

        if (isBoundary) {
            cavity.boundaryTriangles.push_back(currentTriangle);
        } else {
            cavity.interior.push_back(currentTriangle);
        }
    }


    std::sort(cavity.boundaryEdges.begin(), cavity.boundaryEdges.end());
    cavity.boundaryEdges.erase(
        std::unique(cavity.boundaryEdges.begin(), cavity.boundaryEdges.end()),
        cavity.boundaryEdges.end()
    );

    auto dedupSort = [](auto& vec) {
        std::sort(vec.begin(), vec.end());
        vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
    };
    dedupSort(cavity.allTriangles);
    dedupSort(cavity.boundaryTriangles);
    dedupSort(cavity.interior);

    return cavity;
}

template <MeshData MeshType, RefinementCriterion<MeshType> Criterion>
inline std::vector<refiners::helpers::delaunay_cavity::Cavity<MeshType>> DelaunayCavityRefiner<MeshType, Criterion>::mergeCavities(MeshType *outputMesh, std::vector<Cavity> &cavities) {
    // Suppose each cavity has its allTriangles filled
    int amountOfCavitySeeds = cavities.size();
    MeshUnionFind uf(amountOfCavitySeeds);

    // map triangle -> cavity index
    std::unordered_map<FaceIndex,int> triToCavity;
    for (int cavityIndex = 0; cavityIndex < amountOfCavitySeeds; ++cavityIndex) {
        for (FaceIndex cavityTriangle : cavities[cavityIndex].allTriangles) {
            auto [insertedPairIterator, wasJustInserted] = triToCavity.emplace(cavityTriangle, cavityIndex);
            if (!wasJustInserted) {
                uf.unite(cavityIndex, insertedPairIterator->second);
            }
        }
    }

    // merge groups of triangles
    std::unordered_map<int, std::vector<FaceIndex>> mergedTriangles;
    for (int i = 0; i < amountOfCavitySeeds; i++) {
        int root = uf.find(i);
        mergedTriangles[root].insert(
            mergedTriangles[root].end(),
            cavities[i].allTriangles.begin(),
            cavities[i].allTriangles.end()
        );
    }

    // build merged cavities
    std::vector<Cavity> merged;
    for (auto& [root, tris] : mergedTriangles) {
        Cavity cav;
        cav.allTriangles = std::move(tris);

        // boundary edges = edges of tris not shared inside the set
        std::unordered_set<EdgeIndex> edgeSet;
        for (FaceIndex f : cav.allTriangles) {
            for (EdgeIndex e : outputMesh->getEdgesOfTriangle(f)) {
                if (outputMesh->isBorderEdge(e) || isCavityBoundaryEdge(outputMesh,e,cav.allTriangles)) {
                    edgeSet.insert(e);
                }
            }
        }
        cav.boundaryEdges.assign(edgeSet.begin(), edgeSet.end());

        // form ordered boundary loop
        _MeshHelper::formCavityLoop(outputMesh, cav.boundaryEdges);

        merged.push_back(std::move(cav));
    }

    return merged;
}
template <MeshData MeshType, RefinementCriterion<MeshType> Criterion>
MeshType* DelaunayCavityRefiner<MeshType, Criterion>::refineMesh(const MeshType* inputMesh) {
    MeshType* outputMesh = new MeshType(*inputMesh);
    size_t polygonAmount = outputMesh->numberOfPolygons();
    std::vector<std::pair<MeshVertex,FaceIndex>> circumcenters = findMatchingCircumcenters(outputMesh, polygonAmount);

    std::vector<Cavity> cavities;
    cavities.reserve(circumcenters.size());
    // Here we use a vector of uint8_t instead of a vector of bool for better performance at the cost of memory
    std::vector<uint8_t> visited(polygonAmount, 0);
    
    for (const auto& circumcenterData : circumcenters) {
        Cavity cavity = computeCavity(outputMesh, circumcenterData, visited);
        cavities.emplace_back(std::move(cavity));
        resetVisited(visited, cavity);
    }
    std::vector<Cavity> mergedCavities = mergeCavities(outputMesh,cavities);
    // Edge selection phase, all edges of each cavity's border are chosen
    //insertCavity(outputMesh, cavityEdges);
    return outputMesh;
}


#endif