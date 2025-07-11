#ifndef NODE_ELE_READER_HPP
#define NODE_ELE_READER_HPP
#include <mesh_io/mesh_reader.hpp>
#include <mesh_data/half_edge_mesh.hpp>


template <MeshData Mesh>
class NodeEleReader : public MeshReader<Mesh> {
    private:
        unsigned int readNeighFile(std::string& filepath, std::vector<int>& neighbors);
        unsigned int readEleFile(std::string& filepath, std::vector<int>& elements);
        unsigned int readNodeFile(std::string& filepath, std::vector<typename Mesh::VertexType>& vertices);
    public:
        Mesh readMesh(std::string& filename) override;
};

#include <template_implementations/mesh_io/node_ele_reader.ipp>

template class NodeEleReader<HalfEdgeMesh>;

#endif