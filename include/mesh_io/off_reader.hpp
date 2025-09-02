#ifndef OFF_READER_HPP
#define OFF_READER_HPP
#include <mesh_io/mesh_reader.hpp>


template <MeshData Mesh>
class OffReader : public MeshReader<Mesh> {
    public:
        Mesh* readMesh(const std::vector<std::filesystem::path>& files) override;
};

#include <template_implementations/mesh_io/off_reader.ipp>

#endif