#ifndef NULL_SELECTION_CRITERION
#define NULL_SELECTION_CRITERION
#include <concepts/mesh_data.hpp>
#include <concepts/selection_criterion.hpp>

template <MeshData Mesh>
struct NullSelectionCriterion {};

template <MeshData Mesh>
inline constexpr bool isNullSelectionCriterion<NullSelectionCriterion<Mesh>,Mesh> = true;

#endif // NULL_SELECTION_CRITERION