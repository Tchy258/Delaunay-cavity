#ifndef NULL_REFINEMENT_CRITERION
#define NULL_REFINEMENT_CRITERION
#include <concepts/mesh_data.hpp>
#include <concepts/refinement_criterion.hpp>

template <MeshData Mesh>
struct NullRefinementCriterion {};

template <MeshData Mesh>
inline constexpr bool isNullRefinementCriterion<NullRefinementCriterion<Mesh>,Mesh> = true;

#endif // NULL_REFINEMENT_CRITERION