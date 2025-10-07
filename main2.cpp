#include <algorithm>

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <polylla.hpp>
#include <triangulation.hpp>
#include <mesh_data/half_edge_mesh.hpp>
#include <mesh_io/node_ele_reader.hpp>
#include <mesh_io/off_writer.hpp>
#include <mesh_refiners/delaunay_cavity/delaunay_cavity_refiner.hpp>
#include <mesh_refiners/delaunay_cavity/cavity_merger_strategy/exclude_previous_cavities_strategy.hpp>
#include <mesh_refiners/refinement_criteria/min_area_criterion.hpp>
#include <mesh_refiners/refinement_criteria/min_angle_criterion.hpp>
#include <mesh_refiners/refinement_criteria/null_refinement_criterion.hpp>
#include <mesh_refiners/delaunay_cavity/triangle_comparators/null_comparator.hpp>
#include <mesh_refiners/delaunay_cavity/triangle_comparators/random_comparator.hpp>
#include <mesh_refiners/delaunay_cavity/triangle_comparators/area_comparator.hpp>
#include <mesh_refiners/delaunay_cavity/triangle_comparators/edge_length_comparator.hpp>
#include <mesh_io/off_reader.hpp>
#include <polygonal_mesh.hpp>

int main(int argc, char **argv) {

    if(argc == 5)
    {
        std::string node_file = std::string(argv[1]);
        std::string ele_file = std::string(argv[2]);
        std::string neigh_file = std::string(argv[3]);
        std::string output = std::string(argv[4]);

        if(node_file.substr(node_file.find_last_of(".") + 1) != "node"){
            std::cout<<"Error: node file must be .node"<<std::endl;
            return 0;
        }
        if(ele_file.substr(ele_file.find_last_of(".") + 1) != "ele"){
            std::cout<<"Error: ele file must be .ele"<<std::endl;
            return 0;
        }
        if(neigh_file.substr(neigh_file.find_last_of(".") + 1) != "neigh"){
            std::cout<<"Error: neigh file must be .neigh"<<std::endl;
            return 0;
        }

        //Polylla mesh(node_file, ele_file, neigh_file);
        PolygonalMesh<HalfEdgeMesh> polygonalMesh(std::make_unique<NodeEleReader<HalfEdgeMesh>>(), std::make_unique<OffWriter<HalfEdgeMesh>>());
        RandomComparator<HalfEdgeMesh>::setSeed(1);
        polygonalMesh.setRefiner(
        std::make_unique<
            DelaunayCavityRefiner<
                HalfEdgeMesh, 
                NullRefinementCriterion<HalfEdgeMesh>,
                RandomComparator<HalfEdgeMesh>, 
                ExcludePreviousCavitiesStrategy<HalfEdgeMesh>
            >
        >())
        .readMeshFromFiles({node_file, ele_file, neigh_file})
        .refineMesh()
        .writeOutputMesh({output + ".off"});
        
        
        //mesh.print_stats(output + ".json");
        //std::cout<<"output json in "<<output<<".json"<<std::endl;
        //mesh.print_OFF(output+".off");
        //std::cout<<"output off in "<<output<<".off"<<std::endl;
        //mesh.print_ALE(output+".ale");
        //std::cout<<"output ale in "<<output<<".ale"<<std::endl;
    }else if (argc == 3){
        std::string off_file = std::string(argv[1]);
        std::string output = std::string(argv[2]);
	    //Polylla mesh(off_file);
        PolygonalMesh<HalfEdgeMesh> polygonalMesh(std::make_unique<OffReader<HalfEdgeMesh>>(), std::make_unique<OffWriter<HalfEdgeMesh>>());
        //RandomComparator<HalfEdgeMesh>::setSeed(42);
        polygonalMesh.setRefiner(
        std::make_unique<
            DelaunayCavityRefiner<
                HalfEdgeMesh, 
                NullRefinementCriterion<HalfEdgeMesh>,
                DescendingMinEdgeLengthComparator<HalfEdgeMesh>, 
                ExcludePreviousCavitiesStrategy<HalfEdgeMesh>
            >
        >())
        .readMeshFromFiles({off_file})
        .refineMesh()
        .writeOutputMesh({output + ".off"});
        
        //mesh.print_stats(output + ".json");
        //std::cout<<"output json in "<<output<<".json"<<std::endl;
        //mesh.print_OFF(output+".off");
        //std::cout<<"output off in "<<output<<".off"<<std::endl;
        //mesh.print_ALE(output+".ale");
        //std::cout<<"output ale in "<<output<<".ale"<<std::endl;
    } else{
        std::cout<<"Usage: "<<argv[0]<<" <off file .off> <output name>"<<std::endl;
        std::cout<<"Usage: "<<argv[0]<<" <node_file .node> <ele_file .ele> <neigh_file .neigh> <output name>"<<std::endl;
    }
    

    
    
	return 0;
}
