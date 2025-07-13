#ifndef MESH_WRITER_HPP
#define MESH_WRITER_HPP
#include <string>
#include <concepts/mesh_data.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

template<MeshData Mesh>
class MeshWriter {
    public:
        /**
         * Writes a file of the appropriate format in `filename` with the mesh `mesh`
         */
        virtual void writeMesh(std::string& filename, Mesh& mesh) = 0;
};

#endif