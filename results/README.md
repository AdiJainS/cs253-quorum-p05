# Layout

- `raw/` holds every CSV, check log and profiler summary. The folder structure inside it is the one the experiments originally wrote.
- `figures/` holds every plot, with the same sub-folder names as in `raw/`. Each plot is made from the CSVs in the matching `raw/` folder by the scripts in `bench/`.
- Figures used only in the report text are in `docs/report-images/`.

# Benchmark results

The 20 November counters-off maximum timings are in `timing-2026-11-20/`. Its README records revisions, compiler flags and checks. These runs disable both A and B counters. Earlier folders retain their original instrumentation. For the newer cache recordings and the difference between B's 4,892 thread-ID samples and 4,891 lifetime-filtered samples, see `cache/README.md`.

The 17 November rerun is in `d1-update/` and `d1-update-decreases/`. It uses D1 `01da510`, D2 `e925e6f`, D4 `14fc835` and D5 `5085c20`, with the same machine, compiler and benchmark settings described below. The nine maximum timing files have matching A/B checksums. D4's D1 differential test and D2 million-operation test passed, as did both D5 scale checks. D1's submission revision still needs confirmation, and B's counters remained active during timing. The cache traces below are from the earlier D1 revision, not this rerun.

Recorded on 16 November 2026. These are preliminary results for the snapshots listed below. The D1 branch was reset after revision `905f6f28b03c0e3c0aa81089e84fe3ad5bb3bffd`; its owner must confirm the version used here. Do not treat the figures as final team results until the structures are agreed and rechecked.

- D1: `905f6f28b03c0e3c0aa81089e84fe3ad5bb3bffd` (earlier snapshot, no longer at the branch tip).
- D2: `e925e6f` from `feature/structB-d2`.
- D4: `b7c1a10` from `feature/oracle-d4`.
- D5: `c754134` from `d5`.
- Computer: 13th Gen Intel Core i9-13900HX; reported L2 32768 KB, L3 36864 KB. Windows 11 Home Single Language, 64-bit, version 10.0.26300.
- Compiler: Zig 0.13.0 `zig c++`, Clang 18.1.6, `-std=c++17 -O2`. A's counters were disabled in timing builds. B's counters run during timing.
- Random seed 253; `n=1000000`, 1000000 operations, five runs each. Query widths 1, 1024, 1000000; update percentages 10, 50, 90. Each pair uses the same operations for both structures.
- D5 CSVs use `n=4096`, generator parameter 1024 for decrease and rebuild workloads, and 2048 queries for the query workload. They are deterministic and have no random seed. Counter runs are separate from timing runs.

`max-n*.csv` files are raw timings; `max-summary.csv` contains means, standard deviations and ranges. `max-width-u*.png` plots replay time against width. `d5-*-time.csv` contains A/B times, `d5-*-prefix.csv` contains baseline times and `d5-*-counts.csv` contains counter totals. The `-1` fields in D5 files mean that fixed random width and update-percentage settings do not apply.

`gcd-n*.csv`, `gcd-summary.csv` and `gcd-width-u*.png` use the same sizes, seed, widths and update shares. Width 1 was checked and timed against the array oracle; the two wider GCD cases measured the tree only. The array oracle is omitted at those widths because scanning every range would be costly.

`gcd-counts/` contains separate instrumented replays for those nine GCD workloads. The GCD checksums match the original timing CSVs. Counters exclude construction; the hypothetical oracle scan count is calculated from query widths rather than measured by running the oracle.

`counts-n*.csv` files contain separate instrumented runs of the random workloads, using the size, width, update percentage and seed in each filename. Prefix counters are omitted from random runs; the D5 files include them for prefix workloads. B's `assignBlocks` includes its preparatory and update steps; A's update counter counts visits, so the two counts describe different operations.

The later `results/raw/d5-scale/` files use D5's workload analysis from `5085c20`; the generators did not change since `c754134`. Each case has 1,048,576 elements and one million operations. `query` has one million difficult queries; `decrease` has 500,000 decreases, each followed by a query. The files include five-run timing data, separate counters and result checks against the expected values of these deterministic workloads. They were run on the same machine and compiler listed above. The prefix baseline was not run at this size. These results still depend on the earlier, unconfirmed D1 snapshot.

A and B returned matching checksums in all full-scale timing runs. Separate small checks compared each query with D4's oracle, including A's maximum count. Checksums do not prove correctness on all possible inputs. The full-scale timing runs did not replay the array oracle because it scans each queried range.

`decrease-sweep/` contains the later random workloads with 0%, 50% and 100% requested decreases among updates. The full-scale files use the same machine, compiler, seed, sizes, widths and update mixes described above; five runs per structure are recorded for each of the 27 configurations. Every chosen update changes its current value by one. `prefix/` contains a separate `n=4096`, 10,000-operation full-prefix comparison; each query was checked against the oracle before timing. Summaries and plots are in the same directory.

`cache/` describes the WPR recordings, sample counts and their limits. The traces themselves are ignored by Git.
