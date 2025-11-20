seeds=(138 42 69 67 13)
for s in "${seeds[@]}"; do
    ./count_edges_for_all_combinations.sh "$s"
done