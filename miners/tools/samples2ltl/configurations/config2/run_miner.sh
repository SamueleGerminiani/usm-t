python3 samples2ltl.py --test_sat_method --max_num_formulas 1000 --timeout 1800 --start_depth 3 --max_depth 4 --traces /input/golden.txt --force_always_implication 1 --dump_to /output/$MINED_ASSERTIONS_FILE && cat /output/$MINED_ASSERTIONS_FILE

