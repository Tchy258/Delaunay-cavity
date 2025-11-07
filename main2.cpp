#include "configs.hpp"
#include <algorithm>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include MESH_TYPE_HEADER
#include <mesh_io/node_ele_reader.hpp>
#include <mesh_io/off_writer.hpp>
#include <mesh_io/ale_writer.hpp>
#include <mesh_refiners/mesh_refiner_header.hpp>
#include <mesh_refiners/delaunay_cavity/triangle_comparators/triangle_comparators.hpp>
#include <mesh_refiners/delaunay_cavity/cavity_merger_strategy/cavity_merging_strategy.hpp>
#include <mesh_refiners/refinement_criteria/refinement_criteria.hpp>
#include <CLI/CLI.hpp>
#include <mesh_io/off_reader.hpp>
#include <polygonal_mesh.hpp>

int main(int argc, char **argv) {
    CLI::App app{std::string{"CLI Tool to refine a triangular mesh of arbitrary polygons using: "} + std::string{TOSTRING((MESH_REFINER))}};

    bool readFromOff{false};
    bool writeOff{false};
    bool writeJson{false};
    bool writeAle{false};
    std::string input1, input2, input3, output;
    
    CLI::Option* offOpt = app.add_flag("--off-input",readFromOff, "Read input from an off file");
    #ifdef DELAUNAY_REFINER
        unsigned int randomSeed{0};
        double refinementCriterionThreshold{20.0};
        if constexpr (!isNullRefinementCriterion<REFINEMENT_CRITERION , MESH_TYPE>) {
            CLI::Option* refinementArgOpt = app.add_option("--threshold", refinementCriterionThreshold, "Threshold to use for refinement criterion");
        }
        if constexpr (isRandomComparator<TRIANGLE_COMPARATOR , MESH_TYPE>) {
            CLI::Option* seedOpt = app.add_option("--seed", randomSeed, "Seed to use to sort triangles");
        }
    #endif
    std::string configFilenameBase = std::string{TOSTRING(REFINER_T)};
    std::stringstream configFilenameSS{};
    configFilenameSS << (char) (std::tolower(configFilenameBase[0]));
    for (int i = 1; i < configFilenameBase.length(); ++i) {
        if (std::isupper(configFilenameBase[i])) {
            configFilenameSS << "_" << (char) (std::tolower(configFilenameBase[i]));
        } else {
            configFilenameSS << configFilenameBase[i];
        }
    }
    CLI::Option* configFileOpt = app.set_config("--config", configFilenameSS.str() + ".toml", "Read inputs from a .toml file");
    CLI::Option* writeOffOpt = app.add_flag("--off-output", writeOff, "Write to off file");
    CLI::Option* writeAleOpt = app.add_flag("--ale-output", writeAle, "Write to ale file");
    CLI::Option* writeJsonOpt = app.add_flag("--json-output", writeJson, "Write stats to json file");
    CLI::Option* input1Opt = app.add_option("--input1", input1, "First input file, must be either .node or .off")->required();
    input1Opt->check(CLI::ExistingFile);
    auto additionalInputGroup = app.add_option_group("Input groups");
    additionalInputGroup->add_option("--input2", input2, ".ele file")->check(CLI::ExistingFile);
    additionalInputGroup->add_option("--input3", input3, ".neigh file")->check(CLI::ExistingFile);
    CLI::Option* outputOpt = app.add_option("-o,--output",output, "Output base filename/path for outputs if any, defaults to output at same path of input");

    CLI11_PARSE(app,argc,argv);
    #ifdef DELAUNAY_REFINER
    if constexpr (isRandomComparator<TRIANGLE_COMPARATOR , MESH_TYPE>) {
        if (randomSeed != 0) {
            RandomComparator<MESH_TYPE>::setSeed(randomSeed);
        }
    }
    #endif
    if (!readFromOff) {
        if (input2.empty()) {
            throw CLI::ValidationError("At least a .node and .ele file are required");
        }
    }

    if (output.empty()) {
        output = input1.substr(0, input1.find_last_of('.')) + "_output";
    }
    std::unique_ptr<MeshReader<MESH_TYPE>> reader;
    
    if (readFromOff) {
        reader = std::make_unique<OffReader<MESH_TYPE>>();
    } else {
        reader = std::make_unique<NodeEleReader<MESH_TYPE>>();
    }
    
    std::vector<std::filesystem::path> inputPaths{input1,input2,input3};
    
    PolygonalMesh<MESH_TYPE> polygonalMesh(std::move(reader));
    #ifdef REFINEMENT_CRITERION_WITH_ARG
        polygonalMesh.setRefiner(std::make_unique<MESH_REFINER>(REFINEMENT_CRITERION_CONSTRUCTOR(refinementCriterionThreshold)));
    #else
        polygonalMesh.setRefiner(std::make_unique<MESH_REFINER>());
    #endif
    
    polygonalMesh.readMeshFromFiles({input1, input2, input3})
        .refineMesh();
    
    if (writeOff || writeAle) {
        std::unique_ptr<MeshWriter<MESH_TYPE>> writer;
        if (writeOff) {
            writer = std::make_unique<OffWriter<MESH_TYPE>>();
            polygonalMesh.setWriter(std::move(writer));
            polygonalMesh.writeOutputMesh({output + ".off"});
        }
        if (writeAle) {
            writer = std::make_unique<AleWriter<MESH_TYPE>>();
            polygonalMesh.setWriter(std::move(writer));
            polygonalMesh.writeOutputMesh({output + ".ale"});
        }
    }
    if (writeJson) {
        polygonalMesh.writeStatsToJson({output + ".json"});
    }


    
    /*
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
        //RandomComparator<HalfEdgeMesh>::setSeed(1);
        polygonalMesh.setRefiner(
        std::make_unique<
            DelaunayCavityRefiner<
                HalfEdgeMesh, 
                NullRefinementCriterion<HalfEdgeMesh>,
                AscendingMinAngleComparator<HalfEdgeMesh>, 
                ExcludePreviousCavitiesStrategy<HalfEdgeMesh>
            >
        >())
        .readMeshFromFiles({node_file, ele_file, neigh_file})
        .refineMesh()
        .writeOutputMesh({output + ".off"})
        .writeStatsToJson({output + ".json"});
        
        
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
        .writeOutputMesh({output + ".off"})
        .writeStatsToJson({output + ".json"});
        
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
    

    
    */
	return 0;
}
