# Project 5

## Structure A: build and tests

Structure A provides maximum-with-count and GCD queries on zero-based half-open
ranges `[l, r)`, together with point assignments. Its implementation is in
`src/structA/`; its unit and invariant tests are in `tests/structA/`.

From the repository root, with Python 3 and a C++17 compiler on PATH, run:

```sh
python scripts/test_struct_a.py
```

The command builds and runs all Structure A tests with counters disabled and
enabled, placing executables under `build/tests/structA/`. To select another
compiler, use `python scripts/test_struct_a.py --cxx clang++`. Counter builds use
`-DSTRUCTA_ENABLE_COUNTERS`; all builds use `-std=c++17 -O2 -Wall -Wextra -pedantic`.

The differential test runs 1,000,000 operations with seed `20261116` in each mode.
The invariant test checks 20,000 updates with seed `253`; the counter-bound test
uses 1,000,000 elements and 20,000 operations with seed `12345`. Query, update,
adversarial, checker-regression and noncommutative-order tests also run. A passing
run ends with `ALL STRUCTURE A TESTS PASSED`; any failed compilation or test exits
with a nonzero status.

The D1 proof is [D1_250041039.pdf](docs/derivations/D1_250041039.pdf), with its
[Markdown source](docs/derivations/D1-structureA-draft.md) alongside it.

## Benchmarks

The `bench/` folder contains the GCD and maximum benchmarks. See [bench/README.md](bench/README.md) for build and run commands. Raw measurements and machine details are in [results/README.md](results/README.md). The current cost predictions are in `docs/derivations/D6-analyst-draft.md`.

The structure and adversarial files are snapshots for the D6 benchmark. Confirm the team's final versions before using them in the release.

`presentation/` contains a draft D6 slide and viva notes. `results/cache/` describes the cache recordings and their limits.

The latest local counters-off runs are in [results/timing-2026-11-20](results/timing-2026-11-20/README.md). The report separates these from earlier timings and cache recordings.
