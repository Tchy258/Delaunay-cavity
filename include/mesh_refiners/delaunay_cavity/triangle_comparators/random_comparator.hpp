#ifndef RANDOM_COMPARATOR_HPP
#define RANDOM_COMPARATOR_HPP
#include <concepts/mesh_data.hpp>
#include <random>

template <MeshData Mesh>
struct RandomComparator {
    using FaceIndex = typename Mesh::FaceIndex;
    inline static std::random_device rd;
    inline static std::mt19937 generator{rd()};
    inline static std::uniform_int_distribution<> dist{0,1};
    static void setSeed(unsigned int seed) {
        generator = std::mt19937{seed};
    }

    static bool compare(const Mesh* meshptr, const FaceIndex& t1, const FaceIndex& t2) {
        return dist(generator) == 1;
    }
};

#endif // RANDOM_COMPARATOR_HPP