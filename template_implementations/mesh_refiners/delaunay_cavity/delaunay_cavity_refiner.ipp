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
std::vector<refiners::helpers::delaunay_cavity::Cavity<MeshType>> DELAUNAY_CAVITY_CLASS::computeCavities(const MeshType* inputMesh, const std::vector<FaceIndex>& sortedTriangles) {   
    std::vector<_Cavity> cavities;
    // Here we use a vector of uint8_t instead of a vector of bool for better performance at the cost of memory
    std::vector<uint8_t> visited(inputMesh->numberOfPolygons(), 0);
    cavities.reserve(sortedTriangles.size());
    for (const FaceIndex triangleOfCircumcenter : sortedTriangles) {
        if (data.inCavity[triangleOfCircumcenter]) continue;
        Vertex seedV1,seedV2,seedV3;
        inputMesh->getVerticesOfTriangle(triangleOfCircumcenter,seedV1,seedV2,seedV3);
        Vertex circumcenter = Vertex::findCircumcenter(seedV1,seedV2,seedV3);
        std::queue<FaceIndex> bfsNeighborVisitQueue;
        cavities.emplace_back();

        _Cavity& cavity = cavities.back();

        bfsNeighborVisitQueue.push(triangleOfCircumcenter);

        visited[triangleOfCircumcenter] = 1;

        cavity.allTriangles.push_back(triangleOfCircumcenter);
        while(!bfsNeighborVisitQueue.empty()) {
            FaceIndex currentTriangle = bfsNeighborVisitQueue.front();
            data.inCavity[currentTriangle] = true;
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
                    validNeighbor = MergingStrategy::preAdd(neighbor,data.inCavity);
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

        resetVisited(visited, cavity);
    }
    data.memoryStats[M_VISITED_ARRAY] = sizeof(decltype(visited.back())) * visited.capacity();
    return cavities;
}

DELAUNAY_CAVITY_REFINER_TEMPLATE
MeshType* DELAUNAY_CAVITY_CLASS::refineMesh(const MeshType* inputMesh) {
    MeshType* outputMesh = new MeshType(*inputMesh);
    size_t polygonAmount = outputMesh->numberOfPolygons();
    data.inCavity = std::vector<uint8_t>(polygonAmount, 0);
    auto startTime = std::chrono::high_resolution_clock::now();
    std::vector<FaceIndex> sortedTriangles = sortTriangles(outputMesh);
    auto endTime = std::chrono::high_resolution_clock::now();
    data.timeStats[T_TRIANGLE_SORTING] = std::chrono::duration<double, std::milli>(endTime-startTime).count();
    startTime = std::chrono::high_resolution_clock::now();
    std::vector<_Cavity> cavities = computeCavities(outputMesh, sortedTriangles);
    if constexpr (HasPostComputeMethod<MergingStrategy,MeshType>) {
        MergingStrategy::postCompute(outputMesh,cavities);
    }
    endTime = std::chrono::high_resolution_clock::now();
    data.timeStats[T_CAVITY_COMPUTATION] = std::chrono::duration<double, std::milli>(endTime-startTime).count();
    startTime = std::chrono::high_resolution_clock::now();
    data.outputSeeds = _MeshHelper::insertCavity(inputMesh, outputMesh, cavities, data.inCavity);
    endTime = std::chrono::high_resolution_clock::now();
    data.timeStats[T_CAVITY_INSERTION] = std::chrono::duration<double, std::milli>(endTime-startTime).count();
    if constexpr (HasPostInsertionMethod<MergingStrategy, MeshType>) {
        if (storeMeshBeforePostProcess) {
            outputSeedsBeforePostProcess = std::vector<OutputIndex>{data.outputSeeds.begin(), data.outputSeeds.end()};
            meshBeforePostProcess = new MeshType(*outputMesh);
            data.meshStats[N_POLYGONS_BEFORE_POST_PROCESS] = outputSeedsBeforePostProcess.size();
        }
        startTime = std::chrono::high_resolution_clock::now();
        MergingStrategy::postInsertion(inputMesh, outputMesh, data);
        endTime = std::chrono::high_resolution_clock::now();
        data.timeStats[T_CAVITY_MERGING] = std::chrono::duration<double, std::milli>(endTime-startTime).count();
    }
    data.meshStats[N_POLYGONS] = data.outputSeeds.size();
    data.meshStats[N_VERTICES] = outputMesh->numberOfVertices();
    data.meshStats[N_EDGES] = outputMesh->numberOfEdges();
    data.memoryStats[M_CAVITY_ARRAY] = sizeof(decltype(cavities.back())) * cavities.capacity();
    data.memoryStats[M_VERTICES_INPUT] = inputMesh->getVertexMemoryUsage();
    data.memoryStats[M_EDGES_INPUT] = inputMesh->getEdgesMemoryUsage();
    data.memoryStats[M_VERTICES_OUTPUT] = outputMesh->getVertexMemoryUsage();
    data.memoryStats[M_EDGES_OUTPUT] = outputMesh->getEdgesMemoryUsage();
    data.computeTotalMemoryUsage();
    data.computeTotalTime();
    return outputMesh;
}

#undef DELAUNAY_CAVITY_CLASS

#endif