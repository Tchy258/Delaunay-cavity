$seeds = '19', '39', '42', '59', '89'

foreach ($seed in $seeds) {
	python count_edges.py "delaunay_cavity_refiner" "half_edge_mesh" "random_comparator" "exclude_previous_cavities_strategy" "null_refinement_criterion" 100 10000 $seed
}