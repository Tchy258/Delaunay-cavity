#ifndef POLYLLA_REFINER_HPP
#include <mesh_refiners/polylla/polylla_refiner.hpp>
#endif

template <MeshData MeshType>
MeshType* PolyllaRefiner<MeshType>::refineMesh(const MeshType* inputMesh) {
    MeshType* outputMesh = new MeshType(*inputMesh);
    data.maxEdges = BinaryVector(outputMesh->numberOfEdges(), false);
    data.frontierEdges = BinaryVector(outputMesh->numberOfEdges(), false);
    //triangles = outputMesh->get_Triangles(); //Change by triangle list
    data.seedBarrierEdgeTipMark = BinaryVector(outputMesh->numberOfEdges(), false);

    //terminal_edges = bit_vector(outputMesh->halfEdges(), false);
    //outputSeeds = bit_vector(outputMesh->halfEdges(), false);
    
    std::cout<<"Creating Polylla..."<<std::endl;
    auto t_start = std::chrono::high_resolution_clock::now();
    _MeshHelper::labelMaxEdges(data, outputMesh);
    auto t_end = std::chrono::high_resolution_clock::now();
    double edgeLabelTime = std::chrono::duration<double, std::milli>(t_end-t_start).count();
    data.timeStats[T_LABEL_MAX_EDGES] = edgeLabelTime;
    std::cout<<"Labeled max edges in "<< edgeLabelTime <<" ms"<<std::endl;

    t_start = std::chrono::high_resolution_clock::now();
    _MeshHelper::labelFrontierEdges(data, outputMesh);
    t_end = std::chrono::high_resolution_clock::now();
    double frontierEdgeLabelTime = std::chrono::duration<double, std::milli>(t_end-t_start).count();
    data.timeStats[T_LABEL_FRONTIER_EDGES] = frontierEdgeLabelTime;
    std::cout<<"Labeled frontier edges in "<< frontierEdgeLabelTime <<" ms"<<std::endl;
    
    t_start = std::chrono::high_resolution_clock::now();
    data.seedCandidates = _MeshHelper::generateSeedCandidates(data, outputMesh);
    t_end = std::chrono::high_resolution_clock::now();
    double seedEdgeLabelTime = std::chrono::duration<double, std::milli>(t_end-t_start).count();
    data.timeStats[T_LABEL_SEED_EDGES] = seedEdgeLabelTime;
    std::cout<<"Labeled seed edges in "<< seedEdgeLabelTime <<" ms"<<std::endl;

    data.outputSeeds = _MeshHelper::generateOutputSeeds(data, inputMesh, outputMesh);
    
    data.meshStats[N_POLYGONS] = data.outputSeeds.size();
    data.meshStats[N_VERTICES] = outputMesh->numberOfVertices();
    data.meshStats[N_EDGES] = outputMesh->numberOfEdges();
    std::cout<<"Mesh with "<< data.outputSeeds.size() <<" polygons "<< data.frontierEdgeAccumulator / 2 <<" edges and "<< data.barrierEdgeTipAccumulator <<" barrier-edge tips."<<std::endl;
    data.meshStats[N_FRONTIER_EDGES] = data.frontierEdgeAccumulator;
    data.meshStats[N_BARRIER_EDGE_TIPS] = data.barrierEdgeTipAccumulator;
    //outputMesh->print_pg(std::to_string(outputMesh->vertices()) + ".pg");
    data.memoryStats[M_MAX_EDGES] = sizeof(decltype(data.maxEdges.back())) * data.maxEdges.capacity();
    data.memoryStats[M_FRONTIER_EDGES] = sizeof(decltype(data.frontierEdges.back())) * data.frontierEdges.capacity();
    data.memoryStats[M_SEED_EDGES] = sizeof(decltype(data.seedCandidates.back())) * data.seedCandidates.capacity();
    data.memoryStats[M_SEED_BARRIER_EDGE_TIP_MARK] = sizeof(decltype(data.seedBarrierEdgeTipMark.back())) * data.seedBarrierEdgeTipMark.capacity();
    data.memoryStats[M_TRIANGLE_LIST] = data.triangleListMaxSize;
    data.memoryStats[M_VERTICES_INPUT] = inputMesh->getVertexMemoryUsage();
    data.memoryStats[M_EDGES_INPUT] = inputMesh->getEdgesMemoryUsage();
    data.memoryStats[M_VERTICES_OUTPUT] = outputMesh->getVertexMemoryUsage();
    data.memoryStats[M_EDGES_OUTPUT] = outputMesh->getEdgesMemoryUsage();
    data.computeTotalMemoryUsage();
    data.computeTotalTime();

    return outputMesh;
}