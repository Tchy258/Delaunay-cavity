#ifndef MESH_READER_HPP
#define MESH_READER_HPP
#include <string>
#include <concepts/mesh_data.hpp>
#include <concepts/has_adjacency_constructor.hpp>
#include <concepts/is_half_edge_vertex.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>

template<MeshData Mesh>
class MeshReader {
    public:
        virtual Mesh* readMesh(const std::vector<std::filesystem::path>& files) = 0;
};

#endif