param(
    [int]$InputSeed,
    [switch]$SkipExec,
    [int]$RandomSeed = 42,
    [double]$Threshold = 0.5
)

exit 0

$generators         = "delaunay_cavity_generator", "polylla_generator"
$mesh_types         = "half_edge_mesh"
$comparators        = "null_comparator", "edge_length_comparator", "angle_comparator", "area_comparator", "random_comparator"
$merging_strategies = "exclude_previous_cavities_strategy", "merge_triangles_with_best_convexity_strategy", "merge_triangles_into_smallest_neighbor", "merge_triangles_into_biggest_neighbor"
$criteria           = "null_selection_criterion", "min_angle_criterion_robust", "min_angle_criterion", "min_area_criterion", "min_area2_criterion"


foreach ($generator in $generators) {
    foreach ($mesh in $mesh_types) {
        foreach ($comp in $comparators) {
            foreach ($merge in $merging_strategies) {
                foreach ($crit in $criteria) {

                    # Base args common to all cases
                    $args = @(
                        "--generator", $generator
                        "--mesh", $mesh
                        "--comparator", $comp
                        "--merging", $merge
                        "--criterion", $crit
                        "--start", "10"
                        "--end", "1000000"
                        "--seed", $InputSeed
                    )

                    if ($SkipExec) {
                        $args += "--skip-exec"
                    }

                    #
                    # CASE 1: null_comparator
                    #
                    if ($comp -eq "null_comparator") {

                        if ($crit -ne "null_selection_criterion") {
                            $args += @("--threshold", $Threshold)
                        }

                        python count_edges.py @args
                        continue
                    }


                    #
                    # CASE 2: random_comparator
                    #
                    if ($comp -eq "random_comparator") {

                        $args += @("--random-seed", $RandomSeed)

                        if ($crit -ne "null_selection_criterion") {
                            $args += @("--threshold", $Threshold)
                        }

                        python count_edges.py @args
                        continue
                    }


                    #
                    # CASE 3: other comparators (need order + key)
                    #
                    switch ($comp) {
                        "edge_length_comparator" { $keys = "min_edge", "max_edge" }
                        "angle_comparator"       { $keys = "min_angle", "max_angle" }
                        "area_comparator"        { $keys = "doubled", "precise" }
                    }

                    foreach ($order in "ascending", "descending") {
                        foreach ($key in $keys) {

                            $localArgs = $args + @("--order", $order, "--key", $key)

                            if ($crit -ne "null_selection_criterion") {
                                $localArgs += @("--threshold", $Threshold)
                            }

                            python count_edges.py @localArgs
                        }
                    }

                }
            }
        }
    }
}