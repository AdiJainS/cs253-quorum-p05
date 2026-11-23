# Project 05: Aggregates a Fenwick Tree Cannot Support

Team: Quorum — CS 253 / MA 253, 2026

## Team and roles

| Name | Roll no. | GitHub | Role | Module folder | Derivation |
|------|----------|--------|------|---------------|------------|
| Tia Mehra | 250041039 | @Tia-917 | Structure A owner | `src/structA/` | D1 |
| Vishruth Agrawal | 250001078 | @Vishruth-create | Structure B owner | `src/structB/` | D2 |
| Anushri Maheshwari | 250041005 | @Apples-17 | Architect | `app/`, `include/`, `INTERFACE.md` | D3 |
| Sushmita Patil | 250001069 | @psush29 | Oracle owner | `oracle/`, differential tests in `tests/` | D4 |
| Vanshika Agrawal | 250001076 | @vanshi040907 | Adversary | `adversary/` | D5 |
| Adi Jain | 250001083 | @AdiJainS | Analyst | `bench/` | D6 |

## Summary

A sensor array of one million values needs range maximum, the number of times the maximum occurs, and range gcd, while single values can be assigned new values that may be lower. A prefix-sum Fenwick tree cannot do this because maximum has no inverse. Structure A is a segment tree over a monoid; Structure B is the two-array Fenwick range-maximum structure of Dima and Ceterchi, with a prefix-maximum-only Fenwick tree as the baseline. Both A and B answer range maximum in logarithmic steps; the prefix-only baseline needs a full rebuild after each decrease. In our timing runs neither structure wins everywhere: B is faster for width 1024 with 10% updates and for difficult queries, and A is faster when updates are frequent. REPORT.md has the full analysis and its limits.

## Requirements

- OS: macOS (tested on macOS with Apple clang 21.0.0) or Linux (CI runs on Ubuntu 22.04). The recorded measurements in `results/raw/` were taken on Windows 11 with Zig 0.13.0 / Clang 18.1.6.
- Compiler: any C++17 compiler (`c++`, with `-std=c++17 -O2`). GCC 6 is too old (`std::gcd`).
- `make`.
- Python 3 with `matplotlib`, only for regenerating figures and running the benchmark scripts (tested with Python 3.13.12 and matplotlib 3.10.9).
- No library containers stand in for a structure under study. Only `std::vector` is used, as storage.

## Build

    make

This builds `bin/app`, `bin/max_benchmark` and `bin/gcd_benchmark`.

## Quick start

    ./bin/app < app/sample.in

Expected output:

    30 2 6
    30 1 6
    30 1 1
    18 1 1
    -1 0 0

Each `Q left right` line prints `maximum count gcd` over the half-open range `[left, right)`. See `INTERFACE.md` for the input format.

## Tests

    make test

This builds and runs every unit test, invariant checker and differential test, then the sample check and the application's own `--test` for both backends (one million seeded operations, seed 253). The oracle tests use seed 42. A passing run ends with:

    ALL TESTS PASSED

What the differential tests run:

- Structure B against the array oracle: 1,000,000 operations (`tests/test_scale_d2.cpp`, seed 42).
- Structure A against the array oracle: 1,000,000 operations (`tests/structA/test_differential.cpp`, seed 20261116), plus the separate 100,000-operation integration test (`tests/test_real_d1.cpp`, seed 42).
- The application, for each backend: 1,000,000 operations (`bin/app --backend=a --test` and `--backend=b-hybrid --test`, seed 253).

### Structure A tests with counters on and off

From the repository root, run:

```sh
python scripts/test_struct_a.py
```

This D1-only command needs Python 3 and a C++17 compiler on PATH; it was tested on
Windows with Python 3.14.8 and GCC 14.2.0. It builds and runs the tests under
`tests/structA/` with counters disabled and enabled, placing executables under
`build/tests/structA/`. To select another compiler, use
`python scripts/test_struct_a.py --cxx clang++`. Counter builds use
`-DSTRUCTA_ENABLE_COUNTERS`; all builds use `-std=c++17 -O2 -Wall -Wextra -pedantic`.

The differential test runs 1,000,000 operations with seed `20261116` in each mode.
The invariant test checks 20,000 updates with seed `253`; the counter-bound test
uses 1,000,000 elements and 20,000 operations with seed `12345`. Query, update,
adversarial, checker-regression and noncommutative-order tests also run. A passing
run ends with `ALL STRUCTURE A TESTS PASSED`; any failed compilation or test exits
with a nonzero status.

The D1 proof is [D1_250041039.pdf](docs/derivations/D1_250041039.pdf), with its
[Markdown source](docs/derivations/sources/D1-structureA-draft.md) alongside it.

## Evaluation interface (pending instructor specification)

The instructor has not yet published the official operation format. The evaluation runner is therefore **pending**: there is no `app/run_ops.cpp` or `bin/run_ops` yet, and the sensor replay is not claimed to satisfy the hidden-test protocol.

Once the format is released, implement `app/run_ops.cpp` over the shared adapters in `include/`, add a Makefile target producing `bin/run_ops`, and support these required selector forms:

```text
./bin/run_ops --structure A
./bin/run_ops --structure B
```

Before calling it complete, match the instructor's operation syntax, indexing, value limits, output and error rules; add the published examples as fixtures; and test the requested capabilities against the oracle. B currently supports maximum only: count/gcd must follow the official specification, without silently substituting a hybrid.

For the **existing local sensor replay**, use:

    ./bin/app --backend=a < app/sample.in
    ./bin/app --backend=b-hybrid < app/sample.in

These commands follow the local format in `INTERFACE.md`, section 4; sample output is `app/sample.out`. The hybrid takes maximum from B and count/gcd from A and is not the planned pure-B evaluation selection.

## D3 derivation

The D3 proof is `docs/derivations/D3_250041005.pdf`: two pages, with Anushri Maheshwari's name and roll number 250041005. Its editable LaTeX source is `docs/derivations/sources/D3-latex/main.tex`.

To rebuild it with Tectonic (optional; not required for the C++ build):

```sh
mkdir -p build/d3-proof
tectonic --outdir build/d3-proof docs/derivations/sources/D3-latex/main.tex
cp build/d3-proof/main.pdf docs/derivations/D3_250041005.pdf
```

## Benchmarks and figures

    ./scripts/run_all.sh

This builds, runs the tests and the sample, and regenerates figures 4 to 9 and 13 in `results/figures/timing-2026-11-20/` from the CSVs in `results/raw/timing-2026-11-20/`. It takes about 20 seconds on a warm build. `FULL=1 ./scripts/run_all.sh` also reruns the full-scale experiments into `results/raw/rerun-<date>/`; that is much slower and its running time has not been measured. See `bench/README.md` for every benchmark argument.

## Repository map

| Path | Contents |
| --- | --- |
| `include/` | Shared `Operation` type, the shared A/B interface and adapters, the sensor replay header |
| `src/structA/` | Segment tree, maximum/count and gcd wrappers (Structure A) |
| `src/structB/` | Two-array Fenwick maximum and the prefix-only baseline (Structure B) |
| `app/` | Sensor replay application, sample input and output |
| `oracle/` | Array oracle, operation generator, gcd reference (headers) |
| `adversary/` | Adversarial workload generators and analysis |
| `bench/` | Benchmark harness, counters, plotting and summary scripts |
| `tests/` | Unit tests and invariant checks for A, B and the adversary; the oracle owner's differential tests are directly in `tests/` |
| `scripts/` | `run_all.sh` |
| `results/raw/` | Raw CSV logs and check outputs |
| `results/figures/` | Figures generated from `results/raw/` |
| `docs/derivations/` | D1 to D6 PDFs, plus `sources/` (drafts) and `extra/` |
| `docs/slides/` | Slide material and speaker notes |
| `docs/report-images/` | Diagrams used in REPORT.md |

## Results at a glance

Five-run means ± standard deviation in ms, one million values and operations, seed 253, counters off (20 November runs, Windows machine described in `REPORT.md`):

| Workload | Segment tree A | Two-array Fenwick B |
| --- | ---: | ---: |
| Random, width 1, 10% updates | 53.41 ± 0.66 | 68.88 ± 0.81 |
| Random, width 1024, 10% updates | 215.51 ± 6.58 | 143.16 ± 4.87 |
| Random, width 1024, 90% updates | 341.94 ± 44.03 | 624.30 ± 92.48 |
| D5 difficult queries, n = 1,048,576 | 76.12 ± 1.01 | 37.47 ± 0.53 |
| D5 500,000 decreases + 500,000 queries | 45.59 ± 0.09 | 84.92 ± 4.28 |

The prefix-only baseline (n = 4096, 10,000 operations, 50% updates) took 0.23, 14.12 and 27.84 ms with 0, 2,532 and 5,051 decreases. Full analysis is in `REPORT.md`.

## Known limitations

- The official evaluation format is not published; `app/run_ops.cpp` and `./bin/run_ops --structure A|B` remain pending. `bin/app` has `--backend=a` and `--backend=b-hybrid` but the hybrid is not a pure-B aggregate implementation, since B has no count or gcd.
- The timing data was recorded on a Windows machine without background-load control. Some configurations have large spread. The cache profiling uses Windows tracing tools and is sampled, not exact miss counts.
- Structure B's build scans its blocks and has an O(n log n) upper bound, whereas the paper describes a linear build.
- Some figures in `results/figures/` come from earlier runs that used different counter settings; REPORT.md names which run each belongs to.

## Tools and references

AI and other tools are declared in `TOOLS.md`. Main references: Fenwick (1994); Dima and Ceterchi, "Efficient range minimum queries using binary indexed trees", Olympiads in Informatics 9 (2015); de Berg et al., *Computational Geometry*, ch. 10; Knuth, *TAOCP* vol. 2, §4.5.3. The full list is in `REPORT.md`.
