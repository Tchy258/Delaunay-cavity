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
        file << edges.size() << " ";
        int firstEdge = edges[0];
        int currentEdge = firstEdge;
        do {
            file << mesh.origin(currentEdge) << " ";
            currentEdge = mesh.next(currentEdge);
        } while (currentEdge != firstEdge);
        file << std::endl;
    }
}

template <MeshData Mesh>
inline void OffWriter<Mesh>::writeMesh(const std::vector<std::filesystem::path>& filepaths, Mesh &mesh, std::vector<typename Mesh::OutputIndex> outputSeeds) {
    std::ofstream out(filepaths[0]);
    out << "OFF" << std::endl;
    out << mesh.numberOfVertices() << " " << mesh.numberOfPolygons() << " 0" << std::endl;
    for (size_t i = 0; i < mesh.numberOfVertices(); ++i) {
        Vertex v = mesh.getVertex(i);
        out << v.x << " " << v.y << " 0" << std::endl;
    }
    if (outputSeeds.size() == 0) {
        writeFaces(out, mesh);
    } else {
        writeOutputSeeds(out,mesh, outputSeeds);
    }
    out.close();
}

template <MeshData Mesh>
inline void OffWriter<Mesh>::writeOutputSeeds(std::ofstream &file, HalfEdgeMesh &mesh, std::vector<HalfEdgeMesh::OutputIndex> outputSeeds) requires std::same_as<Mesh, HalfEdgeMesh> {
    using EdgeIndex = HalfEdgeMesh::EdgeIndex;
    using VertexIndex = HalfEdgeMesh::VertexIndex;
    for (EdgeIndex firstEdge : outputSeeds) {
        std::vector<VertexIndex> vertices;
        vertices.reserve(3);
        EdgeIndex currentEdge = firstEdge;
        do {
            vertices.push_back(mesh.origin(currentEdge));
            currentEdge = mesh.next(currentEdge);
        } while (currentEdge != firstEdge);

        file << vertices.size();
        for (VertexIndex v : vertices) {
            file << " " << v;
        }
        file << std::endl;
    }
}