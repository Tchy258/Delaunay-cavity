#ifndef OFF_WRITER_HPP
#include <mesh_io/off_writer.hpp>
#endif

template <MeshData Mesh>
inline void OffWriter<Mesh>::writeFaces(std::ofstream& file, HalfEdgeMesh &mesh) requires std::same_as<Mesh, HalfEdgeMesh> {
    std::unordered_map<int, std::vector<int>> edgesOfFace;
    for (int i = 0; i < mesh.numberOfEdges(); ++i) {
        const HalfEdge& he = mesh.getEdge(i);
        if (he.origin == -1) continue;

        edgesOfFace.try_emplace(he.face).first->second.push_back(i);
    }
    for (const auto& faceEdgesPair : edgesOfFace) {
        const std::vector<int>& edges = faceEdgesPair.second;
        file << edges.size();
        int firstEdge = edges[0];
        int currentEdge = firstEdge;
        do {
            file << mesh.origin(currentEdge);
            currentEdge = mesh.next(currentEdge);
        } while (currentEdge != firstEdge);
    }
}

template <MeshData Mesh>
inline void OffWriter<Mesh>::writeMesh(const std::string &filename, Mesh &mesh) {
    std::ofstream out(filename);
    out << "OFF" << std::endl;
    out << mesh.numberOfVertices() << " " << mesh.numberOfPolygons() << " 0" << std::endl;
    for (size_t i = 0; i < mesh.numberOfVertices(); ++i) {
        Vertex v = mesh.getVertex(i);
        out << v.x << " " << v.y << " 0" << std::endl;
    }
    writeFaces(out, mesh);
    out.close();
}