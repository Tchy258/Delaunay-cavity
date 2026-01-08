#ifndef RANDOM_COMPARATOR_HPP
#define RANDOM_COMPARATOR_HPP
#include <concepts/mesh_data.hpp>
#include <random>

template <MeshData Mesh>
struct RandomComparator {
    using FaceIndex = typename Mesh::FaceIndex;
    inline static std::random_device rd;
    inline static std::mt19937 generator{rd()};
    static void setSeed(unsigned int seed) {
        generator = std::mt19937{seed};
    }
};

template<MeshData Mesh>
inline constexpr bool isRandomComparator<RandomComparator<Mesh>,Mesh> = true;

#endif // RANDOM_COMPARATOR_HPP