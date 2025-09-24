#ifndef OFF_READER_HPP
#include<mesh_io/off_reader.hpp>
#endif

template <MeshData Mesh>
Mesh* OffReader<Mesh>::readMesh(const std::vector<std::filesystem::path>& filepaths) {
    //Read the OFF file
    std::vector<typename Mesh::VertexType> vertices;
    std::vector<typename Mesh::EdgeType> edges;
    std::vector<int> faces;
    size_t nVertices, nFaces, nEdges;
    std::string line;
    std::ifstream offFile(filepaths[0]);
    double a1, a2, a3;
    std::string tmp;
    if (offFile.is_open()) {
        //Check first line is a OFF file
        while (std::getline(offFile, line)) { //add check boundary vertices flag
            std::istringstream(line) >> tmp;
            //std::cout<<"tmp: "<<tmp<<std::endl;
            if (tmp[0] != '#' && !isWhitespace(line)) {
                if(tmp[0] == 'O' && tmp[1] == 'F' && tmp[2] == 'F') { //Check if the format is OFF
                    break;
                } else {
                    throw std::runtime_error("File provided to OffReader is not in OFF format");
                }
            }
        }

        //Read the number of vertices and faces
        while (std::getline(offFile, line)) { //add check boundary vertices flag
            std::istringstream(line) >> tmp;
            // std::cout<<"tmp: "<<tmp<<std::endl;
            if (tmp[0] != '#' && !isWhitespace(line)) { 
                std::istringstream(line) >> nVertices >> nFaces;
                vertices.reserve(nVertices);
                faces.reserve(3*nFaces);
                break;  
            }
        }
        //Read vertices
        int index = 0;
        while (index < nVertices && std::getline(offFile, line)) {
            std::istringstream(line) >> tmp;
            // std::cout<<"tmp: "<<tmp<<std::endl;
            if (tmp[0] != '#' && !isWhitespace(line)) {
                std::istringstream(line) >> a1 >> a2 >> a3;
                typename Mesh::VertexType ve;
                ve.x =  a1;
                ve.y =  a2;
                vertices.push_back(ve);
                index++;
            }
        }
        //Read faces
        int length, t1, t2, t3;
        index = 0;
        while (index < nFaces && std::getline(offFile, line)) {
            std::istringstream(line) >> tmp;
            // std::cout<<"tmp: "<<tmp<<std::endl;
            if (tmp[0] != '#' && !isWhitespace(line)) {
                std::istringstream(line) >> length >> t1 >> t2 >> t3;
                // CHECK ORIENTATION!!
                if (vertices[t1].cross2d(vertices[t2],vertices[t3]) > 0) {
                    faces.push_back(t1);
                    faces.push_back(t2);
                    faces.push_back(t3);
                } else {
                    faces.push_back(t3);
                    faces.push_back(t2);
                    faces.push_back(t1);
                }
                //std::cout<<"face "<<index<<": "<<t1<<" "<<t2<<" "<<t3<<std::endl;
                index++;
            }
        }
    } else {
        std::cout << "Unable to open node file"; 
    }
    offFile.close();
    edges.reserve(3* nVertices);
    Mesh* resultMesh = new Mesh(vertices, edges, faces);
    return resultMesh;
}
