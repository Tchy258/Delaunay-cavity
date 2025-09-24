#ifndef POLYLLA_REFINER_HPP
#include <mesh_refiners/polylla/polylla_refiner.hpp>
#endif

template <MeshData MeshType>
MeshType* PolyllaRefiner<MeshType>::refineMesh(const MeshType* inputMesh) {
    MeshType* outputMesh = new MeshType(*inputMesh);
    maxEdges = BinaryVector(outputMesh->numberOfEdges(), false);
    frontierEdges = BinaryVector(outputMesh->numberOfEdges(), false);
    //triangles = outputMesh->get_Triangles(); //Change by triangle list
    seed_bet_mark = BinaryVector(this->outputMesh->numberOfEdges(), false);

    //terminal_edges = bit_vector(outputMesh->halfEdges(), false);
    //seedOutputs = bit_vector(outputMesh->halfEdges(), false);
    
    std::cout<<"Creating Polylla..."<<std::endl;
    _MeshHelper::labelMaxEdges(*this, outputMesh);
    std::cout<<"Labered max edges in "<< timeStats[T_LABEL_MAX_EDGES] <<" ms"<<std::endl;


    std::cout<<"Labeled frontier edges in "<<t_label_frontier_edges<<" ms"<<std::endl;
    
    t_start = std::chrono::high_resolution_clock::now();
    //label seeds edges,
    for (std::size_t e = 0; e < outputMesh->halfEdges(); e++)
        if(outputMesh->is_interior_face(e) && is_seed_edge(e))
            seedOutputs.push_back(e);

        
    t_end = std::chrono::high_resolution_clock::now();
    t_label_seed_edges = std::chrono::duration<double, std::milli>(t_end-t_start).count();
    std::cout<<"Labeled seed edges in "<<t_label_seed_edges<<" ms"<<std::endl;

    //Travel phase: Generate polygon mesh
    int polygon_seed;
    //Foreach seed edge generate polygon
    t_start = std::chrono::high_resolution_clock::now();
    for(auto &e : seedOutputs){
        polygon_seed = travel_triangles(e);
        //output_seeds.push_back(polygon_seed);
        if(!has_BarrierEdgeTip(polygon_seed)){ //If the polygon is a simple polygon then is part of the mesh
            output_seeds.push_back(polygon_seed);
        }else{ //Else, the polygon is send to reparation phase
            auto t_start_repair = std::chrono::high_resolution_clock::now();
            barrieredge_tip_reparation(polygon_seed);
            auto t_end_repair = std::chrono::high_resolution_clock::now();
            t_repair += std::chrono::duration<double, std::milli>(t_end_repair-t_start_repair).count();
        }         
    }    
    t_end = std::chrono::high_resolution_clock::now();
    t_traversal_and_repair = std::chrono::duration<double, std::milli>(t_end-t_start).count();
    t_traversal = t_traversal_and_repair - t_repair;
    
    this->m_polygons = output_seeds.size();

    std::cout<<"Mesh with "<<m_polygons<<" polygons "<<n_frontier_edges/2<<" edges and "<<n_barrier_edge_tips<<" barrier-edge tips."<<std::endl;
    //outputMesh->print_pg(std::to_string(outputMesh->vertices()) + ".pg");             
}