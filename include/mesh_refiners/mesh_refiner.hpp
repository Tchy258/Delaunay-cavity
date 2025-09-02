#ifndef MESH_REFINER_HPP
#define MESH_REFINER_HPP

#include <concepts/mesh_data.hpp>
#include <cstdint>
template <MeshData Mesh>
class MeshRefiner {
    public:
        virtual Mesh* refineMesh(const Mesh* inputMesh) = 0;    
};

#endif