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
        virtual void writeMesh(std::string& filename, Mesh& mesh) = 0;
};

#endif