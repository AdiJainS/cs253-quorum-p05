# Planning

This draft predicts the work needed by each structure. The constants depend on the implementation and machine. The measurements and comparison will go in `REPORT.md`.

## Workload and notation

There are `n` array entries and `m` replayed operations. Query `i` covers `w_i = r_i - l_i + 1` entries. Let `Qmax` and `Qgcd` be the numbers of range-maximum and range-gcd queries. Let `U+` and `U-` be the numbers of point assignments that raise and lower a value; `U0` counts unchanged assignments. Thus `m = Qmax + Qgcd + U+ + U- + U0`. Write `L = ceil(log2 n)` and let `V` bound the magnitude of an input value.

I will compare the same operation and the same generated input across implementations. In particular, a prefix-maximum Fenwick tree does **not** answer an arbitrary `[l, r]` maximum by subtracting two prefix maxima. Its prefix-query timings will be reported separately from arbitrary-range timings.

## Predictions by variant

| Variant | Build | Supported query | Point assignment |
| --- | --- | --- | --- |
| Segment tree, `(maximum, count)` | `O(n)` | Range maximum and its count: `O(log n)` | `O(log n)` |
| Segment tree, gcd | `O(n)` | Range gcd: `O(log n)` visited nodes; target `O(log n + log V)` remainder steps with D3's telescoping argument | `O(log n)` visited nodes; conservatively `O(log n log V)` remainder steps |
| Two-tree Fenwick maximum | `O(n log n)` for the current D2 constructor | Arbitrary range maximum: `O(log(w + 1))` | `O(log n)` amortised, including decreases |
| Prefix-only Fenwick maximum | `O(n)` | Prefix maximum: `O(log n)` | Increase: `O(log n)`; decrease: `O(n)` if rebuilt |
| Plain array | `O(n)` to copy the input | Range maximum/count or gcd: `O(w)` aggregate steps | `O(1)` |

The segment tree combines a bounded number of nodes on each level [4]. A full-array query can be cheaper than a typical range query because the root already stores its answer; the `O(log n)` line is a worst-case bound, not a promise that all widths cost the same. For max/count, each combine is constant work. For gcd, I will count both visited nodes and Euclidean remainder steps: counting each gcd as one unit would hide a value-dependent cost. The simple query bound is `O(log n log V)` remainder steps. D3 is responsible for the tighter `O(log n + log V)` bound; I will state the arithmetic model alongside that proof in the final version [5].

The two-tree Fenwick query walks intervals from the two ends of the requested range. Dima and Ceterchi analyse this as logarithmic in the range length. Their paper presents **minimum**; reversing the comparison gives maximum. Their update method reuses partial results while climbing the trees, giving amortised `O(log n)` even for an assignment that invalidates a stored extremum [3]. Calling a fresh range query for every affected node would instead give an `O(log^2 n)` update. The current D2 constructor scans each stored block: the total scanned length is `O(n log n)`, rather than the paper's linear build. I will measure build separately and record invalidated blocks on updates.

The prefix-only baseline is deliberately limited. The increase-only update rule cannot repair a stored maximum after that maximum is lowered. For a correct baseline under arbitrary assignments, I will model rebuilding after each decrease. Its total work is therefore roughly proportional to `n(U- + 1) + L(Qprefix + U+)`, where `Qprefix` counts only valid prefix queries. A scan fallback for a non-prefix range would be a separate method with `O(w)` query work, not a Fenwick range query.

For the plain-array oracle, total range-query work is proportional to the sum of the requested widths, `sum(w_i)`, while assignments remain constant time. This makes it useful for correctness and short ranges, but predicts a steep cost increase for wide ranges.

## Total-work equations

The table gives per-operation bounds. For a mixed replay, the following equations model total work. The `c` terms are implementation-dependent costs per step.

For the segment tree's maximum/count operations, let `Lq` be the total number of nodes visited by all maximum queries and `Lu` the total nodes visited by all point assignments. A first model is

`Tseg,max = bseg*n + ccombine*Lq + cupdate*Lu`.

The worst-case estimate substitutes `Lq = O(Qmax L)` and `Lu = O((U+ + U-)L)`. The current A counters distinguish selected query blocks from update visits; selected blocks are not a count of every loop iteration. For gcd, let `Lq,gcd` and `Lu,gcd` count query and update visits, and let `Gbuild`, `Gq` and `Gu` count Euclidean remainder steps during construction, queries and updates. Then

`Tseg,gcd = bgcd*n + cvisit*(Lq,gcd + Lu,gcd) + cgcd*(Gbuild + Gq + Gu)`.

For each query, the direct bound is `O(log n log V)` remainder steps. The telescoping bound required by D3 reduces the total query arithmetic to `O(log n + log V)`; the final write-up should show the conditions under which the sequence of gcds telescopes. Updates are reported separately because D3's stated bound is for a query.

For the two-tree Fenwick structure, let `R` be the number of cached intervals recomputed after assignments. The model is

`Tbit = bbit*n*L + cq*sum(log2(w_i + 1)) + cu*(U+ + U-)L + cr*R`.

The update bound is amortised `O(log n)` per assignment. D2 reports `assignBlocks`, `changedBlocks` and `loweredBlocks`; the latter two count blocks whose stored values changed or fell, not every recomputation. When fitting or comparing a model, avoid counting the same work twice in both the base update term and `R`. Fenwick's original structure stores cumulative frequencies using the binary representation of an index [2]. Dima and Ceterchi use two directional trees for range minimum queries and arbitrary assignments; maximum requires reversing the comparison [3]. Their Fenwick implementation was faster on their tested workload, but our results will depend on our implementation and machine.

For the prefix-only maximum baseline, let `Qprefix` count valid prefix queries. If each decrease triggers a rebuild, then

`Tprefix = bprefix*n + cq*Qprefix*L + cu*U+*L + crebuild*n*U-`.

This deliberately makes the cost of decreases visible. It does not give that structure credit for arbitrary-range maximum or max counts, which it does not provide under the stated interface.

For the plain-array oracle, the corresponding model is

`Tarray = barray*n + cscan*sum(w_i) + cassign*(U+ + U- + U0)`.

These equations predict the scale and workload trends, not exact elapsed seconds. I will compare the predicted step counts with measured time per operation. A claim that one implementation has better cache behaviour needs profiler evidence; the access patterns alone are only a reason to investigate it.

## What should separate the curves

- Increasing range width should hurt the array scan roughly in proportion to `w`. The two-tree Fenwick query should grow logarithmically in `w`. The segment tree stays logarithmic in `n`, but alignment can make individual widths unusually cheap.
- A decrease-heavy workload should punish the rebuilding prefix baseline by about `n` work per decrease. For the two-tree structure, I expect the *number of stored maxima actually invalidated* to explain more than the decrease percentage alone.
- Segment-tree maximum and two-tree Fenwick maximum have the same logarithmic worst-case scale for general ranges. I cannot justify a numeric crossover before seeing their constants. I will test whether the two-tree query's shorter paths on narrow ranges outweigh its update work. A full-range segment-tree query may favour the segment tree.
- Repeated equal maxima need their own max/count checks. A scalar prefix-maximum baseline has no count result, so it cannot be included in a max/count timing comparison without a separately specified extension.

These predictions were written before measurement.

## Workloads and measurement plan

I will keep the workload file fixed for every structure in one comparison and record its seed. The size sweep will include smaller inputs for trend plots and the required full-scale point `n = 10^6`, `m = 10^6`. For range widths, I will include point queries, short ranges, medium ranges, and the full array. I will report the actual width distribution, not only its average.

The first matrix will use query/update mixes of 90/10, 50/50, and 10/90. For the update-heavy cases, I will vary the fraction of assignments that are decreases. I will also include repeated equal maxima, mostly distinct values, and D5's sequence that repeatedly lowers a stored maximum. These cases distinguish the cost of ordinary traversal from the work caused by invalidating a cached extremum.

Every implementation will receive the same operations and initial values. I will check the answers against the array oracle on manageable sizes and on full-scale inputs where the scan cost is practical. I will report any oracle runs stopped for runtime rather than treating them as completed measurements. Build time will be measured separately from replay time. Each timing point will use at least five runs; the report will state whether spread means standard deviation or min-to-max range.

For each implementation I will record elapsed time and the operation counters that explain it: segment-tree nodes visited and combines, Fenwick nodes visited and interval recomputations, and array elements scanned. Counter collection will run separately if it changes timing. The report will name the processor, cache sizes, compiler and flags, operating system, and profiler used for any cache claim. These controls follow the project's requirement to state the machine, record seeds, and report spread.

Before benchmarking, D2 and D3 need to settle which interface supplies maximum counts and gcd when the application selects Structure B. The brief specifies range maximum for B. Add any agreed extension to the model before measuring it.

## References

1. CS 253 / MA 253 Lab Project Descriptions, Project 5 and sections 2-6, 2026.
2. P. M. Fenwick, [A New Data Structure for Cumulative Frequency Tables](https://doi.org/10.1002/spe.4380240306), *Software: Practice and Experience* 24(3) (1994), 327-336.
3. M. Dima and R. Ceterchi, [Efficient Range Minimum Queries using Binary Indexed Trees](https://ioinformatics.org/oi/pdf/v9_2015_39_44.pdf), *Olympiads in Informatics* 9 (2015), 39-44, especially sections 2.3-2.4.
4. M. de Berg, O. Cheong, M. van Kreveld, and M. Overmars, *Computational Geometry: Algorithms and Applications*, 3rd ed., Springer, 2008, chapter 10 (segment trees).
5. D. E. Knuth, *The Art of Computer Programming*, vol. 2, 3rd ed., Addison-Wesley, 1997, section 4.5.3 (analysis of Euclid's algorithm).
