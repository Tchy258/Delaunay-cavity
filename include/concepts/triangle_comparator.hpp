#ifndef TRIANGLE_COMPARATOR_HPP
#define TRIANGLE_COMPARATOR_HPP
#include <concepts>
#include <vector>
#include <concepts/mesh_data.hpp>

template <typename Comparator, typename Mesh>
inline constexpr bool isNullComparator = false;

template <typename Comparator, typename Mesh>
inline constexpr bool isRandomComparator = false;

template <typename Comparator, typename Mesh>
concept TriangleComparatorBase = requires(const Mesh* meshptr, const typename Mesh::FaceIndex& t1, const typename Mesh::FaceIndex& t2) {
    {Comparator::compare(meshptr,t1,t2)} -> std::same_as<bool>;
} && MeshData<Mesh>;

template <typename Comparator, typename Mesh>
concept TriangleComparator = TriangleComparatorBase<Comparator, Mesh> || isNullComparator<Comparator,Mesh> || isRandomComparator<Comparator,Mesh>;

#endif // TRIANGLE_COMPARATOR_HPP