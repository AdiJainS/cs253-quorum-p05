# D3 Architect: sensor replay

Author: Anushri Maheshwari

Build and run from the repository root:

```sh
c++ -std=c++17 -O2 -Wall -Wextra -pedantic app/gcd_segment_tree.cpp -o /tmp/sensor-replay
/tmp/sensor-replay < app/sample.in
```

Run the common-interface checks, samples, million-operation tests and scale replay against both backends in one command:

```sh
sh app/run_checks.sh
```

Queries use zero-based [left,right): left included, right excluded. Input begins with n and the operation count, followed by n reading/calibration pairs. Use `U index reading calibration` to replace a sensor and `Q left right` to print maximum, its count, and the calibration gcd. An empty query prints `-1 0 0`. See the root INTERFACE.md for the full contract.

`--test` performs one million seeded small-array operations, with plain-array query comparisons, internal parent checks, deterministic limits and boundary cases, and rejection of invalid API arguments. Seed: 253. The seed generator uses unsigned long long wrapping arithmetic; exact generated values assume the usual 64-bit unsigned long long implementation.

`--scale` runs one million operations on one million sensors (500,000 updates and 500,000 queries), prints a CSV checksum, and validates 50 sampled queries using an array scan. This is a scale smoke test, not a complete full-scale differential test. The checksum wraps modulo the unsigned type's range.

The replay no longer prints counters from its removed reference tree. Use the separate benchmark harness for native operation counters and timing. Five-run timing means/spread, cache profiling, the Structure B comparison, failure shrinking, adversarial generators, and the final tagged team release remain separate team deliverables.

The application now uses the native team implementations through the shared interface in `include/`. The default `--backend=a` uses A for reading maximum/count and calibration gcd. Use `--backend=b-hybrid` for B maximum with A count/gcd support; this hybrid is not a pure-B count/gcd implementation. The D3 proof is in `docs/derivations/D3_250041005.pdf`. The official `run_ops` entry point is pending the instructor’s format; the local replay format is unchanged.

## Short replay example

The following input starts with three sensors, then runs four operations:

```text
3 4
10 6
99 35
7 10
Q 0 1
U 1 4 14
Q 0 3
Q 1 1
```

Expected output:

```text
10 1 6
10 1 2
-1 0 0
```

`Q 0 1` includes only sensor 0. The assignment lowers sensor 1's reading from 99 to 4 and replaces its calibration factor with 14. The next query covers all three sensors: the maximum is 10, it occurs once, and gcd(6,14,10) is 2. The last query is empty because its endpoints are equal. Assignments do not print output.

Sensor indices run from `0` to `n - 1`. A query may use `right = n` to include the last sensor, while `left = right` selects an empty range.

## Shared A/B entry points

`include/range_max.hpp` defines `aggregates::RangeMax`. `include/structure_adapters.hpp` provides A and B implementations with identical zero-based half-open maximum queries and point assignments. Count and gcd are A-only extensions. `include/sensor_replay.hpp` composes the two-field application. See the root `INTERFACE.md` for the complete contract.

```sh
/tmp/sensor-replay --backend=b-hybrid < app/sample.in
/tmp/sensor-replay --backend=b-hybrid --test
/tmp/sensor-replay --backend=a --scale
```
