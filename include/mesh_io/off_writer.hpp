#ifndef OFF_WRITER_HPP
#define OFF_WRITER_HPP
#include<mesh_io/mesh_writer.hpp>
#include<mesh_data/half_edge_mesh.hpp>

template <MeshData Mesh>
class OffWriter : public MeshWriter<Mesh> {
    private:
        void writeFacesRecursive();
        /**
         * Method to write the faces of this half edge based `mesh` to a file
         * 
         * Different overloads should be made for other types of meshes with the appropriate `requires`
         * @param file The output off file stream
         * @param mesh The half edge based mesh whose faces we need to write
         */
        void writeFaces(std::ofstream& file, HalfEdgeMesh& mesh) requires std::same_as<Mesh, HalfEdgeMesh>;
    public:
        void writeMesh(const std::vector<std::filesystem::path>& files, Mesh& mesh) override;
};

#include<template_implementations/mesh_io/off_writer.ipp>

#endif
