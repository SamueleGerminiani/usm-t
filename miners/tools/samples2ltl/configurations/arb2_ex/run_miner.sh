python3 samples2ltl.py --test_sat_method --start_depth 3 --max_num_formulas 1000 --timeout 1800 --max_depth 4 --force_always_implication 1 --traces /input/$LOG_FILE --max_trace_length $MAX_TRACE_LENGTH --dump_to /output/$MINED_SPECIFICATIONS_FILE && cat /output/$MINED_SPECIFICATIONS_FILE

