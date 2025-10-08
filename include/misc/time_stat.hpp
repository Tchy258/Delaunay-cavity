#ifndef TIME_STAT_HPP
#define TIME_STAT_HPP

enum TimeStat {
    T_TRIANGULATION_GENERATION,
    T_LABEL_MAX_EDGES,
    T_LABEL_FRONTIER_EDGES,
    T_LABEL_SEED_EDGES,
    T_TRAVERSAL_AND_REPAIR,
    T_TRAVERSAL,
    T_REPAIR,
    T_TRIANGLE_SORTING,
    T_CIRCUMCENTER_COMPUTATION,
    T_CAVITY_COMPUTATION,
    T_CAVITY_INSERTION
};

inline constexpr unsigned int timeStatAmount = 11;

inline constexpr const char* TimeStatNames[timeStatAmount] = {
    "t_triangulation_generation",
    "t_label_max_edges",
    "t_label_frontier_edges",
    "t_label_seed_edges",
    "t_traversal_and_repair",
    "t_traversal",
    "t_repair",
    "t_triangle_sorting",
    "t_circumcenter_computation",
    "t_cavity_computation",
    "t_cavity_insertion"
};

#endif