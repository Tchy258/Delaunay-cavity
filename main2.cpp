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
#include <mesh_generators/mesh_generator_header.hpp>
#include <mesh_generators/delaunay_cavity/triangle_comparators/triangle_comparators.hpp>
#include <mesh_generators/delaunay_cavity/cavity_merger_strategy/cavity_merging_strategy.hpp>
#include <mesh_generators/selection_criteria/selection_criteria.hpp>
#include <CLI/CLI.hpp>
#include <mesh_io/off_reader.hpp>
#include <polygonal_mesh.hpp>

int main(int argc, char **argv) {
    CLI::App app{std::string{"CLI Tool to refine a triangular mesh of arbitrary polygons using: "} + std::string{TOSTRING((MESH_GENERATOR))}};
    bool readFromOff{false};
    bool writeOff{false};
    bool writeJson{false};
    bool writeAle{false};
    bool writeBeforePost{false};
    std::string input1, input2, input3, output;
    
    CLI::Option* offOpt = app.add_flag("--off-input",readFromOff, "Read input from an off file");
    #ifdef DELAUNAY_GENERATOR
        unsigned int randomSeed{0};
        double selectionCriterionThreshold{20.0};
        if constexpr (!isNullSelectionCriterion<SELECTION_CRITERION , MESH_TYPE>) {
            CLI::Option* selectionArgOpt = app.add_option("--threshold", selectionCriterionThreshold, "Threshold to use for selection criterion");
        }
        if constexpr (isRandomComparator<TRIANGLE_COMPARATOR , MESH_TYPE>) {
            CLI::Option* seedOpt = app.add_option("--seed", randomSeed, "Seed to use to sort triangles");
        }
        if constexpr (HasPostInsertionMethod<MERGING_STRATEGY , MESH_TYPE>) {
            CLI::Option* writeBeforePostOpt = app.add_flag("--write-intermediate", writeBeforePost, "Write the mesh before any post processing is done");
        }
    #endif
    std::string configFilenameBase = std::string{TOSTRING(GENERATOR_T)};
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

    app.allow_extras();

    CLI11_PARSE(app,argc,argv);
    #ifdef DELAUNAY_GENERATOR
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
    #ifdef SELECTION_CRITERION_WITH_ARG
        polygonalMesh.setGenerator(std::make_unique<MESH_GENERATOR>(SELECTION_CRITERION_CONSTRUCTOR(selectionCriterionThreshold), writeBeforePost));
    #else
        #ifdef DELAUNAY_GENERATOR
            polygonalMesh.setGenerator(std::make_unique<MESH_GENERATOR>(writeBeforePost));
        #else
            polygonalMesh.setGenerator(std::make_unique<MESH_GENERATOR>());
        #endif
    #endif
    
    polygonalMesh.readMeshFromFiles({input1, input2, input3})
        .generateMesh();
    
    if (writeOff || writeAle) {
        auto writeMesh = [&](auto writerCreator, const std::string& ext) {
            std::unique_ptr<MeshWriter<MESH_TYPE>> writer = writerCreator();
            polygonalMesh.setWriter(std::move(writer));
            polygonalMesh.writeOutputMesh({output + ext});
            if (writeBeforePost) {
                polygonalMesh.writeMeshBeforePostProcess({output + "_intermediate" + ext});
            }
        };

        if (writeOff)
            writeMesh([] { return std::make_unique<OffWriter<MESH_TYPE>>(); }, ".off");

        if (writeAle)
            writeMesh([] { return std::make_unique<AleWriter<MESH_TYPE>>(); }, ".ale");
    }


    if (writeJson) {
        polygonalMesh.writeStatsToJson({output + ".json"});
    }

	return 0;
}
