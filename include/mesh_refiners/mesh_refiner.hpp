#ifndef MESH_REFINER_HPP
#define MESH_REFINER_HPP

#include <concepts/mesh_data.hpp>

template <MeshData Mesh>
class MeshRefiner {
    public:
        virtual Mesh refineMesh(Mesh inputMesh) = 0;    
};

#endif