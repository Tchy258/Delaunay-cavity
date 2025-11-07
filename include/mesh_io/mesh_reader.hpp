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
        virtual ~MeshReader() = default;
    protected:
        // Esta fue la unica función ql funciono, porque las weas nativas de c++ funcionan mal
        //https://stackoverflow.com/a/22395635
        // Returns false if the string contains any non-whitespace characters
        // Returns false if the string contains any non-ASCII characters
        static bool isWhitespace(std::string s) {
            for(int index = 0; index < s.length(); index++)
                if(!std::isspace(s[index]))
                    return false;
            return true;
        }
};

#endif