# D6 speaking notes

20 November update: the latest counters-off timing table is at the start of `REPORT.md`, with raw data in `results/raw/timing-2026-11-20/`. Earlier slides and the script below refer to their original runs. Do not combine those numbers with the new build. For the newer cache traces, quote 1,568 for A and 4,892 for B with a thread-ID filter; the stricter lifetime filter gives B=4,891. These recordings predate the counters-off change. The Canva deck and existing PPTX have not been edited in this update.

The current counters-off timings are in `results/raw/timing-2026-11-20/`; earlier runs remain in their dated folders. Use results from one run set at a time when presenting.

## Suggested 3-4 minute explanation

**Problem and model (about 45 seconds)**

Our project compares range maximum structures under point assignments. I modeled the segment tree and the two-tree Fenwick structure with logarithmic query and update work. For the prefix-only baseline, a decrease may force a full rebuild, so a workload with many decreases can include a term proportional to `n` times the number of decreases. The array oracle scans the requested range.

**Benchmark design (about 45 seconds)**

I generate the initial array and operations once from a recorded seed. Each structure receives the same operations. Small runs compare every query with the oracle. I measure construction separately from replay, repeat timing five times, and report the mean and sample standard deviation. I collect operation counters in separate runs. B currently keeps its counters active even during timing, so the A/B comparison has that limitation.

**Example result (about 60 seconds)**

The chart shows the prefix baseline on 4,096 values, 10,000 operations, and a 50% update mix. The three cases request 0%, 50% and 100% decreases among updates. The actual decrease counts were 0, 2,532 and 5,051. Mean replay times were 0.21, 13.80 and 27.69 milliseconds; standard deviations were 0.007, 0.032 and 0.900 milliseconds. The result is consistent with rebuilding after each decrease. All queries in this example are full-array prefix queries, which the baseline supports. These are replay times for the entire workload, not per-operation latency.

**Limits and conclusion (about 30 seconds)**

The 20 November full-scale timings use one million values and one million operations, with counters disabled for both structures. The 17 November WPR traces recorded 1,568 A and 4,892 B samples by thread ID; strict lifetime filtering gives B 4,891. Those sampled events include setup and construction and are not exact cache-miss totals or rates.

## Viva questions

**Why not subtract two prefix maxima?**
Maximum has no inverse: knowing the maximum of two prefixes does not determine the maximum of the range between them. For example, arrays `[10, 2]` and `[10, 7]` have the same prefix maxima at both positions but different maxima in the second position.

**Why does the prefix tree rebuild after a decrease?**
A stored block maximum may have come from the lowered element. The old maximum alone does not reveal the next largest value, so this baseline recomputes its blocks from the array.

**How do I explain the main cost formulas?**
For a segment tree, building is proportional to `n`, and each query or update visits at most a logarithmic number of levels. For the prefix baseline, each decrease rebuilds the structure, giving a term proportional to `n` times the number of decreases. An array query scans its range, so its total query work depends on the sum of the range widths. The two-tree Fenwick model includes the current constructor's block scans and the query and update work. These models predict work, not exact milliseconds.

**What does the GCD bound mean?**
The tree visits logarithmically many blocks, but each GCD calculation may take multiple remainder steps. A direct query bound is `O(log n log V)`. The tighter bound in the project requires a telescoping argument whose assumptions still need D3's confirmation; the benchmark counters do not prove it.

**Why use the same seed?**
The benchmark creates one workload and replays it for each structure. This gives both implementations the same initial values, query ranges and assignments. The seed records how the workload was generated; code revision and compiler details must be recorded too.

**Why check against the oracle before timing?**
A fast answer is useful only if it is correct. The harness checks each result on manageable workloads, outside the timed section. Full-scale timing checksums help detect disagreements but do not prove all results are correct.

**Why can a full-range query be faster than a medium range?**
The segment tree may return a full array from one stored root. A medium range can require several blocks. The actual work depends on the range and its alignment, even when both have the same worst-case bound.

**What do the decrease percentages mean?**
They are percentages of updates, not of all operations. Each selected update changes the current value by one. Random selection means the actual percentage may differ; the CSV records the actual counts.

**Does the result prove one structure has better cache behavior?**
No. WPR recorded sampled cache events, not exact miss totals or rates. The recordings include workload generation and construction, and there is only one trace for each structure.

**What limits the A/B timing comparison?**
The 20 November build disables counters in both structures, but measurements came from an interactive Windows machine without controlled background load or thermal conditions. The team should identify the release revisions before treating these figures as final.

**What do you own?**
The D6 cost model, workload and benchmark harness, measurement and counter scripts, figures, and the comparison in the report. The data structures and oracle belong to the other team roles.
