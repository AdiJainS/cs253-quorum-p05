# D5: Adversarial workloads

This module generates deterministic workloads for Project 5,
"Aggregates a Fenwick tree cannot support."

## Files

- `generators.hpp`: initial arrays and operation sequences.
- `test_adversary.cpp`: prefix-baseline failure and rebuild checks,
  plus comparison with the two-tree Fenwick.
- `test_queries.cpp`: difficult-query checks against all ranges
  for small power-of-two sizes.
- `test_decreases.cpp`: repeated decreases and update-position checks.

The code reuses D4's `Operation` type and D2's Fenwick implementations.

## Operation conventions

Indices are zero-based. Query endpoints are inclusive.

An operation has four fields:

    {type, l, r, value}

For assignment, `l` is the updated index and `r` is unused.
For queries, `l` and `r` are the endpoints and `value` is unused.

## Workload 1: repeated prefix-baseline rebuilds

Function:

    makeRebuildWorkload(n, k)

Requires `n >= 1` and `1 <= k <= n`.

The initial array is:

    [n, n-1, ..., 1]

Positions 0 through k-1 are assigned zero, from left to right.
Each assignment is followed by a whole-array maximum query.

Each assignment removes the current unique global maximum.
The increase-only prefix update leaves stale maxima.

D2's correct prefix baseline rebuilds on every strict decrease.
Therefore, k decreases cause k rebuilds. Each rebuild costs
Theta(n), giving Theta(kn) total rebuild work.

This statement concerns the rebuilding baseline, not every
possible Fenwick-based implementation.

The workload contains 2k operations.

## Workload 2: difficult range queries

Function:

    makeQueryWorkload(n, queryCount)

Requires a power-of-two `n >= 4` and `queryCount >= 1`.

The initial array is:

    [1, 2, ..., n]

Every query covers [0, n-2], whose maximum is n-1.

For n = 2^h, the current two-tree query implementation takes:

    (h-1) left steps + (h-1) right steps + 1 final step
    = 2h-1 steps

Exhaustive checks over every range confirmed that this candidate
attains the maximum queryBlocks count for n = 4, 8, ..., 256.

These finite checks support the construction; they do not replace
a general maximality proof.

The workload contains queryCount operations.

## Workload 3: repeated decreases of a unique maximum

Function:

    makeDecreaseWorkload(n, decreases)

Requires a power-of-two `n >= 4` and `decreases >= 1`.

Initially, all values are zero except:

    a[0] = decreases + 1

Index 0 is assigned decreases, decreases-1, ..., 1.
Each assignment is followed by a whole-array maximum query.

Index 0 remains the unique maximum. Every affected stored block
must therefore lower its maximum.

For n = 2^h, each assignment in the current implementation records:

    assignBlocks = 2h + 2
    loweredBlocks = h + 2

For sizes 4 through 256, testing every possible update position
confirmed that index 0 attains both maximum counts.

The workload contains 2 * decreases operations.

## Build and run

Run from the repository root using a C++17 compiler.

Dependencies:

    include/operations.hpp
    src/structB/prefix_fenwick_max.hpp
    src/structB/fenwick_max.hpp

Compile:

```powershell
g++ -std=c++17 -O2 adversary/test_adversary.cpp -o adversary/test_adversary.exe
g++ -std=c++17 -O2 adversary/test_queries.cpp -o adversary/test_queries.exe
g++ -std=c++17 -O2 adversary/test_decreases.cpp -o adversary/test_decreases.exe
```

Run on Windows PowerShell:

```powershell
.\adversary\test_adversary.exe
.\adversary\test_queries.exe
.\adversary\test_decreases.exe
```

The query test exhaustively checks small arrays only.
It must not be extended to million-element exhaustive testing.

## Full-scale verification

Compile from the repository root:

```powershell
g++ -std=c++17 -O2 adversary/run_scale.cpp -o adversary/run_scale.exe
```

Set `test = 1` for difficult queries or `test = 2` for repeated
decreases. Save and recompile after changing the selector.

Run:

```powershell
.\adversary\run_scale.exe
```

The expected totals apply to the fixed sizes in run_scale.cpp
and the current D2 implementation.

## Reproducibility and measurements

All three generators are deterministic and use no random seed.
Record the generator name, parameters and code revision when
saving experimental results.

Counters measure implementation steps, not elapsed time.
Timing experiments and figures belong to D6's benchmark harness.

Full-scale correctness and counter verification passed for both
two-tree workloads at 1,048,576 elements and 1,000,000 operations.

Results are saved in results/raw/d5-queries-scale.txt and
results/raw/d5-decreases-scale.txt.

Integration into D4's differential harness and D6's benchmark
harness is coordinated with their respective owners.

## Using these workloads in team tools

Include:

    #include "../adversary/generators.hpp"

Adjust the relative include path if necessary.

Each generator returns a Workload containing:

- initial: the initial array of long long values.
- operations: a vector of the shared Operation type.

Indices are zero-based and query endpoints are inclusive.

Available generators:

    makeRebuildWorkload(n, k)
    makeQueryWorkload(n, queryCount)
    makeDecreaseWorkload(n, decreases)

Operation counts:

- Rebuild workload: 2*k operations.
- Query workload: queryCount operations.
- Decrease workload: 2*decreases operations.

The query and decrease generators require power-of-two n >= 4.
The rebuild generator requires 1 <= k <= n.

All workloads are deterministic; no random seed is used.

### D4: correctness testing

Replay the initial array and operations through the existing
correctness harness.

Use manageable sizes for direct-array checks. The generated
operations use inclusive query endpoints; convert endpoints when
calling a structure with half-open ranges.

### D6: measurement

Use identical initial arrays and operations for each supported
comparison. Workload generation should be outside the timed region.

The prefix baseline supports prefix queries only. All queries in
the rebuild workload start at zero, so they are valid prefix queries.

The query and decrease workloads target the current two-tree
Fenwick implementation. See ANALYSIS.md for the constructions
and expected counters.

Full-scale correctness and counter results are recorded in:

    results/raw/d5-queries-scale.txt
    results/raw/d5-decreases-scale.txt

These logs are not timing measurements. The rebuilding prefix
baseline was not replayed at million-decrease scale.

Timing repetitions, machine details, figures and predicted-versus-
measured comparisons belong to the Analyst's benchmark work.