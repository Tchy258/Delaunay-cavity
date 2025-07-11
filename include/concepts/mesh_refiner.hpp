#ifndef MESH_REFINER_CONCEPT_HPP
#define MESH_REFINER_CONCEPT_HPP
#include <concepts>
#include <string>
#include <unordered_map>
#include <misc/mesh_stat.hpp>
#include <misc/time_stat.hpp>
#include <concepts/mesh_data.hpp>

template<typename Refiner, typename Mesh>
concept MeshRefinerConcept = requires(Refiner refiner, Mesh& mesh) {
    { refiner.refineMesh(mesh) } -> std::same_as<Mesh>;
    { refiner.getRefinementStats() } -> std::same_as<std::unordered_map<MeshStat,int>>;
    { refiner.getRefinementTimes() } -> std::same_as<std::unordered_map<TimeStat,double>>;
    { refiner.writeStatsToJson(std::string{}) } -> std::same_as<void>;
};

#endif