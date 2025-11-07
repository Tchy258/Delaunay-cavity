#ifndef ALE_WRITER_HPP
#define ALE_WRITER_HPP
#include <mesh_io/mesh_writer.hpp>
#include <mesh_data/half_edge_mesh.hpp>

template <MeshData Mesh>
class AleWriter : public MeshWriter<Mesh> {
    private:
        void writeOutputSeeds(std::ofstream& file, HalfEdgeMesh& mesh, std::vector<HalfEdgeMesh::OutputIndex> outputSeeds) requires std::same_as<Mesh, HalfEdgeMesh>;
    public:
        void writeMesh(const std::vector<std::filesystem::path>& files, Mesh& mesh, std::vector<typename Mesh::OutputIndex> outputSeeds = {}) override;
};

#include <mesh_io/ale_writer.ipp>

#endif