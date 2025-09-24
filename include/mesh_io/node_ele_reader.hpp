#ifndef NODE_ELE_READER_HPP
#define NODE_ELE_READER_HPP
#include <mesh_io/mesh_reader.hpp>
#include <mesh_data/half_edge_mesh.hpp>


template <MeshData Mesh>
class NodeEleReader : public MeshReader<Mesh> {
    private:
        static inline std::filesystem::path changeExtension(const std::filesystem::path& filepath, const std::string& targetExtension) {
            auto newPath = filepath;
            newPath.replace_extension(targetExtension);
            return newPath;
        }
        std::vector<std::filesystem::path> parsePaths(const std::vector<std::filesystem::path>& filepaths);
        unsigned int readNeighFile(const std::filesystem::path& filepaths, std::vector<int>& neighbors);
        unsigned int readEleFile(const std::filesystem::path& filepaths, std::vector<int>& elements);
        unsigned int readNodeFile(const std::filesystem::path& filepaths, std::vector<typename Mesh::VertexType>& vertices);
    public:
        /**
         * Make sure triangles are 0 indexed
         */
        Mesh* readMesh(const std::vector<std::filesystem::path>& files) override;
};

#include <mesh_io/node_ele_reader.ipp>

#endif