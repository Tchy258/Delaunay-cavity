#ifndef MESH_WRITER_HPP
#define MESH_WRITER_HPP
#include <string>
#include <concepts/mesh_data.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>

template<MeshData Mesh>
class MeshWriter {
    public:
        /**
         * Writes files with the appropriate format specified on `files` with the mesh `mesh`
         */
        virtual void writeMesh(const std::vector<std::filesystem::path>& files, Mesh& mesh, std::vector<typename Mesh::OutputIndex> outputSeeds = {}) = 0;
        virtual ~MeshWriter() = default;
};

#endif