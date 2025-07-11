#ifndef REFINEMENT_CRITERION_HPP
#define REFINEMENT_CRITERION_HPP
#include<concepts>

template <typename Criterion, typename Mesh>
concept RefinementCriterion = requires(Criterion criterion, Mesh inputMesh, int polygonIndex) {
    {criterion(inputMesh,polygonIndex)} -> std::convertible_to<bool>;
};


#endif