$SEED = 42         # Any positive integer for random_comparator
$CRITERION_ARG = 0.5  # Any double for refinement criteria

$refiners = "delaunay_cavity_refiner", "polylla_refiner"
$mesh_types = "half_edge_mesh"
$comparators = "null_comparator", "edge_length_comparator", "angle_comparator", "area_comparator", "random_comparator"
$merging_strategies = "exclude_previous_cavities_strategy"
$criteria = "null_refinement_criterion", "min_angle_criterion_robust", "min_angle_criterion", "min_area_criterion", "min_area2_criterion"

foreach ($refiner in $refiners) {
  foreach ($mesh in $mesh_types) {
    foreach ($comp in $comparators) {
      foreach ($merge in $merging_strategies) {
        foreach ($crit in $criteria) {

          if ($comp -eq "null_comparator") {
            if ($crit -eq "null_refinement_criterion") {
              python count_edges.py $refiner $mesh $comp $merge $crit 100 10000
            } else {
              python count_edges.py $refiner $mesh $comp $merge $crit 100 10000 "" "" $CRITERION_ARG
            }
          }
          elseif ($comp -eq "random_comparator") {
            if ($crit -eq "null_refinement_criterion") {
              python count_edges.py $refiner $mesh $comp $merge $crit 100 10000 $SEED
            } else {
              python count_edges.py $refiner $mesh $comp $merge $crit 100 10000 $SEED "" $CRITERION_ARG
            }
          }
          else {
            foreach ($order in "ascending", "descending") {
              switch ($comp) {
                "edge_length_comparator" { $keys = "min_edge", "max_edge" }
                "angle_comparator" { $keys = "min_angle", "max_angle" }
                "area_comparator" { $keys = "doubled", "precise" }
              }
              foreach ($key in $keys) {
                if ($crit -eq "null_refinement_criterion") {
                  python count_edges.py $refiner $mesh $comp $merge $crit 100 10000 $order $key
                } else {
                  python count_edges.py $refiner $mesh $comp $merge $crit 100 10000 $order $key $CRITERION_ARG
                }
              }
            }
          }

        }
      }
    }
  }
}
