#ifndef DELAUNAY_CAVITY_REFINER_IPP
#define DELAUNAY_CAVITY_REFINER_IPP
#ifndef DELAUNAY_CAVITY_REFINER_HPP
#include <mesh_refiners/delaunay_cavity/delaunay_cavity_refiner.hpp>
#endif

template <MeshData MeshType, RefinementCriterion<MeshType> Criterion, CavityMergingStrategy<MeshType> MergingStrategy>
std::vector<std::pair<typename MeshType::VertexType, typename MeshType::FaceIndex>> DelaunayCavityRefiner<MeshType, Criterion, MergingStrategy>::findMatchingCircumcenters(MeshType *outputMesh, size_t polygonAmount)
{
    std::vector<std::pair<MeshVertex,FaceIndex>> circumcenters;
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

template <MeshData MeshType, RefinementCriterion<MeshType> Criterion, CavityMergingStrategy<MeshType> MergingStrategy>
std::vector<refiners::helpers::delaunay_cavity::Cavity<MeshType>> DelaunayCavityRefiner<MeshType, Criterion, MergingStrategy>::computeCavities(MeshType* outputMesh, const std::vector<std::pair<MeshVertex, FaceIndex>>& circumcenters, std::vector<uint8_t>& visited) {   
    std::vector<Cavity> cavities;
    cavities.reserve(circumcenters.size());
    for (const auto& circumcenterData : circumcenters) {
        const MeshVertex& circumcenter = circumcenterData.first;
        FaceIndex triangleOfCircumcenter = circumcenterData.second;

        std::queue<FaceIndex> bfsNeighborVisitQueue;
        cavities.emplace_back();

        Cavity& cavity = cavities.back();

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
                    if (_MeshHelper::isBorderEdge(outputMesh,e)) {
                        isBoundary = currentTriangle != triangleOfCircumcenter;
                        cavity.boundaryEdges.push_back(e);
                    }
                }
            }
            for (FaceIndex neighbor : neighbors) {
                if (visited[neighbor]) continue;
                bool validNeighbor = true;
                if constexpr (HasPreAddMethod<MergingStrategy,MeshType>) {
                    validNeighbor = MergingStrategy::preAdd(outputMesh,neighbor,cavities);
                }
                Vertex v0, v1, v2;
                outputMesh->getVerticesOfTriangle(neighbor, v0, v1, v2);
                
                if (validNeighbor && Vertex::inCircle(v0, v1, v2, circumcenter)) {
                    visited[neighbor] = 1;
                    bfsNeighborVisitQueue.push(neighbor);
                    cavity.allTriangles.push_back(neighbor);
                } else if (currentTriangle == triangleOfCircumcenter) {
                    std::array<EdgeIndex, 3> neighborEdges = outputMesh->getEdgesOfTriangle(neighbor);
                    for (EdgeIndex e : neighborEdges) {
                        if (!_MeshHelper::isSharedEdge(outputMesh,e,currentTriangle,neighbor)) {
                            cavity.boundaryEdges.push_back(e);
                        }
                    }
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

        _MeshHelper::markInteriorOutputsFromSeeds(outputMesh, this->outputSeeds, cavity.interior);
        resetVisited(visited, cavity);
    }
    return cavities;
}

template <MeshData MeshType, RefinementCriterion<MeshType> Criterion, CavityMergingStrategy<MeshType> MergingStrategy>
MeshType* DelaunayCavityRefiner<MeshType, Criterion, MergingStrategy>::refineMesh(const MeshType* inputMesh) {
    MeshType* outputMesh = new MeshType(*inputMesh);
    size_t polygonAmount = outputMesh->numberOfPolygons();
    outputSeeds = _MeshHelper::generateInitialOutputSeeds(outputMesh);
    std::vector<std::pair<MeshVertex,FaceIndex>> circumcenters = findMatchingCircumcenters(outputMesh, polygonAmount);

    
    // Here we use a vector of uint8_t instead of a vector of bool for better performance at the cost of memory
    std::vector<uint8_t> visited(polygonAmount, 0);
    
    std::vector<Cavity> cavities = computeCavities(outputMesh, circumcenters, visited);
    
    if constexpr (HasPostComputeMethod<MergingStrategy,MeshType>) {
        MergingStrategy::postCompute(outputMesh,cavities);
    }
    _MeshHelper::eraseInteriorOutputsFromSeeds(this->outputSeeds);
    _MeshHelper::insertCavity(outputMesh,cavities);
    return outputMesh;
}


#endif