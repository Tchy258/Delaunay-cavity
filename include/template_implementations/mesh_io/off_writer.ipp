#ifndef OFF_WRITER_HPP
#include <mesh_io/off_writer.hpp>
#include "off_writer.hpp"
#endif

template <MeshData Mesh>
inline void OffWriter<Mesh>::writeFaces(std::ifstream& file, HalfEdgeMesh &mesh) requires std::same_as<Mesh, HalfEdgeMesh> {
    std::vector<bool> facesChecked(mesh.numberOfPolygons(),false);
    for (size_t i = 0; i < mesh.numberOfPolygons(); ++i) {
        int face = mesh.getPolygon(i);
        if (!facesChecked[face]) {
            facesChecked[face] = true;
            std::vector<int> vertices;
            using EdgeIndex = int;
            EdgeIndex firstEdge = face;
            vertices.push_back(mesh.origin(firstEdge));
            EdgeIndex nextEdge = mesh.next(firstEdge);
            while (nextEdge != firstEdge) {
                vertices.push_back(mesh.origin(nextEdge));
                nextEdge = mesh.next(nextEdge);
            }
            file << vertices.size();
            for (auto k: vertices) {
                file << " " << k;
            }
            file << std::endl;
        }
    }
}

template <MeshData Mesh>
inline void OffWriter<Mesh>::writeMesh(std::string &filename, Mesh &mesh) {
    std::ifstream out(filename);
    out << "OFF" << std::endl;
    out << mesh.numberOfVertices() << " " << mesh.numberOfPolygons() << " 0" << std::endl;
    for (size_t i = 0; i < mesh.numberOfVertices(); ++i) {
        Vertex v = mesh.getVertex(i);
        out << v.x << " " << v.y << " 0" << std::endl;
    }
    writeFaces(out, mesh);
    out.close();
}