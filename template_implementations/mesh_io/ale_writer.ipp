#ifndef ALE_WRITER_HPP
#include <mesh_io/ale_writer.hpp>
#endif

template <MeshData Mesh>
inline void AleWriter<Mesh>::writeOutputSeeds(std::ofstream &file, HalfEdgeMesh &mesh, std::vector<HalfEdgeMesh::OutputIndex> outputSeeds) requires std::same_as<Mesh, HalfEdgeMesh> {
    for(HalfEdgeMesh::EdgeIndex firstEdge : outputSeeds){
        HalfEdgeMesh::EdgeIndex currentEdge = firstEdge;
        std::vector<HalfEdgeMesh::VertexIndex> vertices;
        vertices.reserve(3);
        do {
            vertices.push_back(mesh.origin(currentEdge));
            currentEdge = mesh.next(currentEdge);
        } while(firstEdge != currentEdge);

        file << vertices.size() << " ";            

        for (HalfEdgeMesh::VertexIndex vertIndex : vertices) {
            file << vertIndex << " ";
        }
        file << '\n'; 
    }
    //Print borderedges
    file <<"# indices of nodes located on the Dirichlet boundary\n";
    ///Find borderedges
    HalfEdgeMesh::EdgeIndex borderCurrent, borderInitial = 0;
    for(HalfEdgeMesh::EdgeIndex edgeIndex = mesh.numberOfEdges() - 1; edgeIndex != 0; --edgeIndex) {
        if(mesh.isBorderEdge(edgeIndex)){
            borderInitial = edgeIndex;
            break;
        }
        if(mesh.isBorderEdge(mesh.twin(edgeIndex))){
            borderInitial = mesh.twin(edgeIndex);
            break;
        }
    }
    file << mesh.origin(borderInitial) << " ";
    borderCurrent = mesh.prev(borderInitial);
    while(borderInitial != borderCurrent) {
        file << mesh.origin(borderCurrent) << " ";
        borderCurrent = mesh.prev(borderCurrent);
    }
    file << std::endl;
}

template <MeshData Mesh>
void AleWriter<Mesh>::writeMesh(const std::vector<std::filesystem::path>& files, Mesh& mesh, std::vector<typename Mesh::OutputIndex> outputSeeds) {
    std::ofstream file(files[0]);
    size_t numberOfVertices = mesh.numberOfVertices();    
    file << "# domain type\n";
    file << "Custom\n";
    file << "# nodal coordinates: number of nodes followed by the coordinates\n";
    file << numberOfVertices << '\n';
    file << std::setprecision(15);
    //print nodes
    double xmax = mesh.getVertex(0).x;
    double xmin = xmax;
    double ymax = mesh.getVertex(0).y;
    double ymin = ymax;
    for(typename Mesh::VertexIndex i = 0; i < numberOfVertices; ++i) {
        typename Mesh::VertexType& vert = mesh.getVertex(i);
        xmax = vert.x > xmax ? vert.x : xmax;
        xmin = vert.x < xmin ? vert.x : xmin;
        ymax = vert.y > ymax ? vert.y : ymax;
        ymin = vert.y < ymin ? vert.y : ymin;
        file << vert.x << " " << vert.y << '\n';
    }
    file << "# element connectivity: number of elements followed by the elements\n";
    if (!outputSeeds.empty()) {
        file << outputSeeds.size() << '\n';
    } else {
        file << mesh.numberOfPolygons() << '\n';
    }
    //print polygons
    writeOutputSeeds(file, mesh, outputSeeds);
    file << "# indices of nodes located on the Neumann boundary\n";
    file << "0\n";
    file << "# xmin, xmax, ymin, ymax of the bounding box\n";
    file << xmin << " " << xmax << " " << ymin << " " << ymax << std::endl;
    file.close();
}