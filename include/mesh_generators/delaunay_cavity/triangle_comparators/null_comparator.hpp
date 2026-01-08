#ifndef NULL_COMPARATOR_HPP
#define NULL_COMPARATOR_HPP
#include <concepts/triangle_comparator.hpp>

template <MeshData Mesh>
struct NullComparator {};

template<MeshData Mesh>
inline constexpr bool isNullComparator<NullComparator<Mesh>,Mesh> = true;

#endif // NULL_COMPARATOR_HPP