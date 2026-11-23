# Benchmarks

Build from the repository root with a C++17 compiler (GCC 7 or newer, or Clang). `make all` builds the timing binaries into `bin/`. The counter builds are separate:

```sh
make all
make bin/max_counts bin/gcd_counts
```

`bin/max_benchmark` is built with `-DSTRUCTB_DISABLE_COUNTERS`. The counter builds use `-DSTRUCTA_ENABLE_COUNTERS` and leave B's counters on. The measurements already in `results/raw/` were taken on Windows with a Zig C++ toolchain; the commands here are for macOS and Linux.

Run a correctness check before timing. Arguments are `N OPERATIONS WIDTH UPDATE_PERCENT SEED RUNS WORKLOAD MODE [DECREASE_PERCENT]`:

```sh
./bin/max_benchmark 1000 10000 64 20 253 5 random check
./bin/max_benchmark 1000 10000 64 20 253 5 random check 75
./bin/max_benchmark 4096 1024 1 0 0 5 rebuild check
./bin/max_counts 4096 1024 1 0 0 5 rebuild counts
python3 bench/test_decrease.py bin/max_benchmark
```

Workloads are `random`, `rebuild`, `decrease` and `query`. The last three use D5's generators. For D5, `OPERATIONS` is the generator parameter: number of decreases or queries. The generator may produce more operations than that parameter. Its query endpoints are inclusive; the A adapter changes them to half-open ranges. The prefix-only baseline is checked and measured only when all queries start at index 0.

For random workloads, the optional `DECREASE_PERCENT` chooses the percentage of updates intended to decrease the current value, from 0 to 100. Every selected update changes its current value by exactly one. The actual numbers of increases, decreases and unchanged assignments are written to the timing and counter CSVs. The selection is random, so the actual percentage may differ. When the argument is omitted, the original generator is used, preserving the old seeded workloads; the CSV records `-1` as the requested decrease percentage. D5 workloads also use `-1`.

`check` compares every maximum query with D4's oracle, including A's count. `time` measures the max-only A tree and B. `prefix` times the baseline separately; it rejects non-prefix queries. `counts` requires the counter build. Build maximum timing runs with `-DSTRUCTB_DISABLE_COUNTERS` and without `STRUCTA_ENABLE_COUNTERS`, so A and B both have counters disabled. Build counter runs with `-DSTRUCTA_ENABLE_COUNTERS` and without `STRUCTB_DISABLE_COUNTERS`, so both collect counters. The harness rejects timing or counter modes built with the wrong settings. The switch is local to the D6 copy of B; the data-structure algorithm is unchanged.

`time-a` and `time-b` run only A or B, respectively. These modes help record separate profiler traces while using the same seed and arguments. See `results/raw/cache/README.md` for the cache recordings and instructions.

For D5's full-scale query and decrease cases, `check-scale` checks every result against the known expected maximum and count. `counts-ab` collects A and B counters without running the prefix baseline, whose repeated full rebuilds are too expensive at this size:

```sh
./bin/max_benchmark 1048576 1000000 1 0 0 5 query check-scale
./bin/max_benchmark 1048576 500000 1 0 0 5 decrease check-scale
./bin/max_benchmark 1048576 500000 1 0 0 5 decrease time
./bin/max_counts 1048576 500000 1 0 0 5 decrease counts-ab
python3 bench/plot_d5.py results/raw/d5-scale/summary.csv results/figures/d5-scale/timings.png
```

The decrease generator creates one query after each assignment, so its parameter of 500,000 produces one million operations. `check-scale` is specific to these two D5 generators; the general `check` mode uses D4's array oracle and is best kept to manageable sizes.

Run the full-scale random matrix with five repetitions:

```sh
python3 bench/run_matrix.py bin/max_benchmark results/raw/rerun
python3 bench/summarize.py results/raw/max-n*.csv > results/raw/max-summary.csv
python3 bench/plot_width.py results/raw/max-summary.csv results/figures/max-width-u10.png --n 1000000 --operations 1000000 --update-percent 10 --seed 253 --workload random
python3 bench/run_matrix.py bin/gcd_benchmark results/raw/rerun-gcd --metric gcd
python3 bench/run_matrix.py bin/max_benchmark new-results --decrease-percent 75
python3 bench/plot_decreases.py results/raw/decrease-sweep/prefix-summary.csv results/figures/decrease-sweep/prefix-decreases.png --n 4096 --operations 10000 --width 4096 --update-percent 50 --seed 253 --log-y
```

The matrix uses widths 1, 1024 and 1000000 and update shares 10, 50 and 90 percent. The `run_matrix.py --help` options change the sizes and seed. Files generated with `--decrease-percent` have a `-d` suffix. The script refuses to overwrite existing files. For plots from these files, pass the same `--decrease-percent` to `plot_width.py`. The summary groups different requested decrease percentages separately; older CSVs without the new fields remain supported. For GCD, the matrix checks and times the oracle at width 1 and runs only the tree for wider queries. CSV timings include build and mixed replay separately. Summary spread is sample standard deviation. D5 is deterministic; its CSV uses seed 0 and -1 for random-workload fields that do not apply.

To repeat the whole decrease study, including its checks, prefix runs, summaries and plots, choose a new folder and run:

```sh
python3 bench/run_decrease_study.py bin/max_benchmark new-decrease-results
```

The output folder must not already exist. The full-scale portion runs 27 configurations with five repetitions per structure. Record the machine and compiler details with the new results.

GCD benchmark arguments are `N OPERATIONS WIDTH UPDATE_PERCENT SEED RUNS MODE`, where mode is `check`, `tree` or `oracle`:

```sh
./bin/gcd_benchmark 1000 10000 64 20 253 5 check
./bin/gcd_counts 1000 10000 64 20 253 1 counts
python3 bench/test_gcd_counts.py bin/gcd_benchmark bin/gcd_counts
```

The GCD benchmark uses D1's GCD tree and D4's half-open GCD reference. Its separate `counts` build reports query blocks and merges, update visits and merges, and Euclidean remainder steps during replay. `oracle_elements_if_scanned` is calculated from query widths; the counter run does not execute the array oracle. Construction is excluded from those counters. The maximum benchmark checks D4's general oracle, which uses inclusive ranges. Neither benchmark measures cache misses. Store profiler output before drawing cache conclusions.

`src/structA`, `src/structB` and `adversary` contain snapshots of teammates' code used for these experiments. Their revisions and the important D1 branch reset are recorded in `results/README.md`. Replace them with the team's final versions before the final release, then rerun the checks and measurements.
