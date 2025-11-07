#ifndef DELAUNAY_CAVITY_REFINER_IPP
#define DELAUNAY_CAVITY_REFINER_IPP
#ifndef DELAUNAY_CAVITY_REFINER_HPP
#include <mesh_refiners/delaunay_cavity/delaunay_cavity_refiner.hpp>
#endif

#define DELAUNAY_CAVITY_CLASS DelaunayCavityRefiner<MeshType,Criterion,Comparator,MergingStrategy>

DELAUNAY_CAVITY_REFINER_TEMPLATE
std::vector<typename MeshType::FaceIndex> DELAUNAY_CAVITY_CLASS::sortTriangles(MeshType *outputMesh) {
    
    std::vector<FaceIndex> triangles(outputMesh->numberOfPolygons());
    std::iota(triangles.begin(), triangles.end(), 0);

    auto sortOrShuffle = [&](auto beginIt) {
        if constexpr (!isNullComparator<Comparator,MeshType>) {
            if constexpr (isRandomComparator<Comparator,MeshType>) {
                std::shuffle(beginIt,triangles.end(), Comparator::generator);
            } else {
                std::stable_sort(beginIt, triangles.end(), [outputMesh](const FaceIndex& t1, const FaceIndex& t2) { 
                    return Comparator::compare(outputMesh,t1,t2); 
                });
            }
        }
    };
    if constexpr (isNullRefinementCriterion<Criterion,MeshType> && !isNullComparator<Comparator,MeshType>) {
        sortOrShuffle(triangles.begin());
    } else if constexpr (isNullComparator<Comparator,MeshType> && !isNullRefinementCriterion<Criterion,MeshType>) {
        std::stable_partition(triangles.begin(), triangles.end(), [outputMesh, this](const FaceIndex& polygonIndex) { return refinementCriterion(outputMesh,polygonIndex);});
    } else if constexpr (!isNullComparator<Comparator,MeshType> && !isNullRefinementCriterion<Criterion,MeshType>) {
        auto iteratorToFirstNonMatching = std::stable_partition(triangles.begin(), triangles.end(), [outputMesh, this](const FaceIndex& polygonIndex) { return refinementCriterion(outputMesh,polygonIndex);});
        sortOrShuffle(iteratorToFirstNonMatching);
    }
    
    return triangles;
}

DELAUNAY_CAVITY_REFINER_TEMPLATE
std::vector<std::pair<typename MeshType::VertexType, typename MeshType::FaceIndex>> DELAUNAY_CAVITY_CLASS::computeCircumcenters(MeshType *outputMesh, std::vector<FaceIndex> sortedTriangles) {
    std::vector<std::pair<MeshVertex,FaceIndex>> circumcenters;
    circumcenters.reserve(sortedTriangles.size());

    for (FaceIndex triangle : sortedTriangles) {
        Vertex v0,v1,v2;
        outputMesh->getVerticesOfTriangle(triangle, v0, v1, v2);
        circumcenters.push_back(std::pair<MeshVertex,int>(Vertex::findCircumcenter(v0,v1,v2),triangle));
        
    }
    return circumcenters;
}

DELAUNAY_CAVITY_REFINER_TEMPLATE
std::vector<refiners::helpers::delaunay_cavity::Cavity<MeshType>> DELAUNAY_CAVITY_CLASS::computeCavities(const MeshType* inputMesh, const std::vector<std::pair<MeshVertex, FaceIndex>>& circumcenters, std::vector<uint8_t>& visited) {   
    std::vector<Cavity> cavities;
    cavities.reserve(circumcenters.size());
    for (const auto& circumcenterData : circumcenters) {
        const MeshVertex& circumcenter = circumcenterData.first;
        FaceIndex triangleOfCircumcenter = circumcenterData.second;
        if (inCavity[triangleOfCircumcenter]) continue;
        std::queue<FaceIndex> bfsNeighborVisitQueue;
        cavities.emplace_back();

        Cavity& cavity = cavities.back();

        bfsNeighborVisitQueue.push(triangleOfCircumcenter);

        visited[triangleOfCircumcenter] = 1;

        cavity.allTriangles.push_back(triangleOfCircumcenter);
        while(!bfsNeighborVisitQueue.empty()) {
            FaceIndex currentTriangle = bfsNeighborVisitQueue.front();
            inCavity[currentTriangle] = true;
            bfsNeighborVisitQueue.pop();
            const std::vector<FaceIndex>& neighbors = inputMesh->getNeighbors(currentTriangle);
            std::array<EdgeIndex, 3> triangleEdges = inputMesh->getEdgesOfTriangle(currentTriangle);
            bool isBoundary = false;

            if (neighbors.size() < 3) {
                for (EdgeIndex e : triangleEdges) {
                    if (_MeshHelper::isBorderEdge(inputMesh,e)) {
                        isBoundary = currentTriangle != triangleOfCircumcenter;
                        cavity.boundaryEdges.push_back(e);
                    }
                }
            }
            for (FaceIndex neighbor : neighbors) {
                if (visited[neighbor]) continue;
                bool validNeighbor = true;
                if constexpr (HasPreAddMethodPerCavity<MergingStrategy,MeshType>) {
                    validNeighbor = MergingStrategy::preAdd(inputMesh,neighbor,cavities);
                }
                if constexpr (HasPreAddMethodByPresence<MergingStrategy,MeshType>) {
                    validNeighbor = MergingStrategy::preAdd(neighbor,inCavity);
                }
                Vertex v0, v1, v2;
                inputMesh->getVerticesOfTriangle(neighbor, v0, v1, v2);
                
                if (validNeighbor && Vertex::inCircle(v0, v1, v2, circumcenter)) {
                    visited[neighbor] = 1;
                    bfsNeighborVisitQueue.push(neighbor);
                    cavity.allTriangles.push_back(neighbor);
                } else if (currentTriangle == triangleOfCircumcenter) {
                    for (EdgeIndex e : triangleEdges) {
                        if (_MeshHelper::isSharedTriangleEdge(inputMesh,e,currentTriangle,neighbor)) {
                            cavity.boundaryEdges.push_back(e);
                        }
                    }
                } else {
                    isBoundary = true;

                    EdgeIndex boundaryEdge = inputMesh->getTriangleSharedEdge(currentTriangle,neighbor);
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

        resetVisited(visited, cavity);
    }
    return cavities;
}

DELAUNAY_CAVITY_REFINER_TEMPLATE
MeshType* DELAUNAY_CAVITY_CLASS::refineMesh(const MeshType* inputMesh) {
    MeshType* outputMesh = new MeshType(*inputMesh);
    size_t polygonAmount = outputMesh->numberOfPolygons();
    inCavity = std::vector<uint8_t>(polygonAmount, 0);
    auto t_start = std::chrono::high_resolution_clock::now();
    std::vector<FaceIndex> sortedTriangles = sortTriangles(outputMesh);
    auto t_end = std::chrono::high_resolution_clock::now();
    timeStats[T_TRIANGLE_SORTING] = std::chrono::duration<double, std::milli>(t_end-t_start).count();
    t_start = std::chrono::high_resolution_clock::now();
    std::vector<std::pair<MeshVertex,FaceIndex>> circumcenters = computeCircumcenters(outputMesh, sortedTriangles);
    t_end = std::chrono::high_resolution_clock::now();
    timeStats[T_CIRCUMCENTER_COMPUTATION] = std::chrono::duration<double, std::milli>(t_end-t_start).count();
    
    // Here we use a vector of uint8_t instead of a vector of bool for better performance at the cost of memory
    std::vector<uint8_t> visited(polygonAmount, 0);
    t_start = std::chrono::high_resolution_clock::now();
    std::vector<Cavity> cavities = computeCavities(outputMesh, circumcenters, visited);
    if constexpr (HasPostComputeMethod<MergingStrategy,MeshType>) {
        MergingStrategy::postCompute(outputMesh,cavities);
    }
    t_end = std::chrono::high_resolution_clock::now();
    timeStats[T_CAVITY_COMPUTATION] = std::chrono::duration<double, std::milli>(t_end-t_start).count();
    t_start = std::chrono::high_resolution_clock::now();
    outputSeeds = _MeshHelper::insertCavity(inputMesh, outputMesh, cavities, inCavity);
    t_end = std::chrono::high_resolution_clock::now();
    timeStats[T_CAVITY_INSERTION] = std::chrono::duration<double, std::milli>(t_end-t_start).count();
    if constexpr (HasPostInsertionMethod<MergingStrategy, MeshType>) {
        t_start = std::chrono::high_resolution_clock::now();
        MergingStrategy::postInsertion(inputMesh, outputMesh, outputSeeds, inCavity);
        t_end = std::chrono::high_resolution_clock::now();
        timeStats[T_CAVITY_MERGING] = std::chrono::duration<double, std::milli>(t_end-t_start).count();
    }
    meshStats[N_POLYGONS] = outputSeeds.size();
    meshStats[N_VERTICES] = polygonAmount;
    meshStats[N_EDGES] = outputMesh->numberOfEdges();
    return outputMesh;
}

#undef DELAUNAY_CAVITY_CLASS

#endif