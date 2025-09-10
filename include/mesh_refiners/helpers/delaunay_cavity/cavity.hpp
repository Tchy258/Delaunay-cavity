#ifndef CAVITY_HPP
#define CAVITY_HPP
#include <concepts/mesh_data.hpp>
#include <vector>

namespace refiners::helpers::delaunay_cavity {
    template <MeshData Mesh>
    struct Cavity {
        private:
            using FaceIndex = typename Mesh::FaceIndex;
            using EdgeIndex = typename Mesh::EdgeIndex;
        public:
            std::vector<FaceIndex> allTriangles;
            std::vector<FaceIndex> interior;
            std::vector<FaceIndex> boundaryTriangles;
            std::vector<EdgeIndex> boundaryEdges;
    };
}

#endif