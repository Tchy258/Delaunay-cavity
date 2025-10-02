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
    data.timeStats[T_LABEL_MAX_EDGES] = std::chrono::duration<double, std::milli>(t_end-t_start).count();
    std::cout<<"Labeled max edges in "<< data.timeStats[T_LABEL_MAX_EDGES] <<" ms"<<std::endl;

    t_start = std::chrono::high_resolution_clock::now();
    _MeshHelper::labelFrontierEdges(data, outputMesh);
    t_end = std::chrono::high_resolution_clock::now();
    data.timeStats[T_LABEL_FRONTIER_EDGES] = std::chrono::duration<double, std::milli>(t_end-t_start).count();
    std::cout<<"Labeled frontier edges in "<<data.timeStats[T_LABEL_FRONTIER_EDGES]<<" ms"<<std::endl;
    
    t_start = std::chrono::high_resolution_clock::now();
    data.seedCandidates = _MeshHelper::generateSeedCandidates(data, outputMesh);
    t_end = std::chrono::high_resolution_clock::now();
    data.timeStats[T_LABEL_SEED_EDGES] = std::chrono::duration<double, std::milli>(t_end-t_start).count();
    std::cout<<"Labeled seed edges in "<< data.timeStats[T_LABEL_SEED_EDGES] <<" ms"<<std::endl;

    data.outputSeeds = _MeshHelper::generateOutputSeeds(data, inputMesh, outputMesh);
    
    data.meshStats[N_POLYGONS] = data.outputSeeds.size();

    std::cout<<"Mesh with "<< data.meshStats[N_POLYGONS] <<" polygons "<< data.meshStats[N_FRONTIER_EDGES] / 2 <<" edges and "<< data.meshStats[N_BARRIER_EDGE_TIPS] <<" barrier-edge tips."<<std::endl;
    //outputMesh->print_pg(std::to_string(outputMesh->vertices()) + ".pg");
    
    return outputMesh;
}