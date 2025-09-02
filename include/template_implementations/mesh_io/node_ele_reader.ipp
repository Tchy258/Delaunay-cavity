#ifndef NODE_ELE_READER_HPP
#include <mesh_io/node_ele_reader.hpp>
#endif

template <MeshData Mesh>
std::vector<std::filesystem::path> NodeEleReader<Mesh>::parsePaths(const std::vector<std::filesystem::path>& filepaths) {
    size_t amountOfPaths = filepaths.size();
    if (amountOfPaths == 3) {
        return filepaths;    
    }
    std::vector<std::filesystem::path> parsedPaths;
    parsedPaths.reserve(3);
    parsedPaths.emplace_back(filepaths[0]);
    if (amountOfPaths == 1) {
        parsedPaths.emplace_back(NodeEleReader::changeExtension(filepaths[0], ".ele"));
        parsedPaths.emplace_back(NodeEleReader::changeExtension(filepaths[0], ".neigh"));
    } else if (amountOfPaths == 2) {
        parsedPaths.emplace_back(filepaths[1]);
        parsedPaths.emplace_back(NodeEleReader::changeExtension(filepaths[0],".neigh"));
    }
    return parsedPaths;
}

template <MeshData Mesh>
unsigned int NodeEleReader<Mesh>::readNodeFile(const std::filesystem::path& filepath, std::vector<typename Mesh::VertexType> &vertices)
{
    unsigned int n_vertices = 0;
    unsigned int discard = 0; // to skip values
    unsigned int n_attributes = 0;
    unsigned int hasBoundaryMarkers = 0;
    std::ifstream nodefile(filepath);
    if (nodefile.is_open()) {
        std::string line;
        std::getline(nodefile, line);
        std::istringstream(line) >> n_vertices >> discard >> n_attributes >> hasBoundaryMarkers;
        vertices.reserve(n_vertices);
        while (std::getline(nodefile, line))
        {
            std::istringstream stream(line);
            int nodeNumber, boundary;
            double xCoord, yCoord;
            unsigned int attributesOfThisNode = n_attributes;
            stream >> nodeNumber >> xCoord >> yCoord;
            while (attributesOfThisNode > 0) {
                stream >> discard;
                attributesOfThisNode--;
            }
            stream >> boundary;
            typename Mesh::VertexType ve;
            ve.x =  xCoord;
            ve.y =  yCoord;
            if constexpr (IsHalfEdgeVertex<typename Mesh::VertexType>) {
                ve.isBorder = (boundary == 1) ? true : false;
            }
            vertices.push_back(ve);
        }
    } else {
        throw std::runtime_error("File: " + filepath.string() + " cannot be opened (does it exist?)");
    }
    nodefile.close();
    return n_vertices;
}

template <MeshData Mesh>
Mesh* NodeEleReader<Mesh>::readMesh(const std::vector<std::filesystem::path>& files) {
    std::vector<std::filesystem::path> parsedPaths = parsePaths(files);

    // Read vertices
    std::vector<typename Mesh::VertexType> vertices;
    unsigned int n_vertices = readNodeFile(parsedPaths[0], vertices);

    // Read faces
    std::vector<int> faces;
    unsigned int n_elements = readEleFile(parsedPaths[1], faces);

    // Prepare empty edge container
    std::vector<typename Mesh::EdgeType> edges;

    Mesh* resultMesh;
    // Handle adjacency construction if supported
    if constexpr (HasAdjacencyConstructor<Mesh>) {
        std::vector<int> neighbors;
        if (std::filesystem::exists(parsedPaths[2])) {
            unsigned int n_border_edges = readNeighFile(parsedPaths[2], neighbors);
            edges.reserve(3 * n_vertices - 3 - n_border_edges);
            resultMesh = new Mesh(vertices, edges, faces, neighbors);
            return resultMesh;
        }
    }
    resultMesh = new Mesh(vertices, edges, faces);
    // Default mesh construction
    return resultMesh;
}


template <MeshData Mesh>
unsigned int NodeEleReader<Mesh>::readNeighFile(const std::filesystem::path &filepath, std::vector<int> &neighbors) {
    
        std::string line;
        std::ifstream neighfile(filepath);
        unsigned int n_border_edges;
        if (neighfile.is_open()) {
            unsigned int n_faces;
            neighfile >> n_faces;

            neighbors.reserve(3*n_faces);
            std::getline(neighfile, line); //skip the first line
            while (std::getline(neighfile, line)) {
                int triangleNumber, neighbor1, neighbor2, neighbor3;
                if(line[0] == '#'){
                    std::cout<<line<<std::endl;
                    continue;
                }
                std::istringstream(line) >> triangleNumber >> neighbor1 >> neighbor2 >> neighbor3;
                
                neighbors.push_back(neighbor1);
                neighbors.push_back(neighbor2);
                neighbors.push_back(neighbor3);

                // -1 Means no neighbor at some side, thus it has a border edge
                if(neighbor1 < 0){
                    n_border_edges++;
                }
                if(neighbor2 < 0){
                    n_border_edges++;
                }
                if(neighbor3 < 0){
                    n_border_edges++;
                }
            }
        } else {
            throw std::runtime_error("File: " + filepath.string() + " cannot be opened (does it exist?)");
        }
        neighfile.close();
        return n_border_edges;
}

template <MeshData Mesh>
unsigned int NodeEleReader<Mesh>::readEleFile(const std::filesystem::path &filepath, std::vector<int> &faces) {
    std::string line;
    std::ifstream elefile(filepath);
    unsigned int n_elements;

    if (elefile.is_open()) {
        elefile >> n_elements;
        
        faces.reserve(3*n_elements);
        std::getline(elefile, line); //skip the first line
        while (std::getline(elefile, line)) {
            int triangleNumber, node1, node2, node3;
            std::istringstream(line) >> triangleNumber >> node1 >> node2 >> node3;
            faces.push_back(node1);
            faces.push_back(node2);
            faces.push_back(node3);
        }
    }
    else {
        throw std::runtime_error("File: " + filepath.string() + " cannot be opened (does it exist?)");
    }
    elefile.close();
    return n_elements;
}

