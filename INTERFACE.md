# Common Structure A/B interface

Author: Anushri Maheshwari (Apples-17), D3 Architect.

## 1. Shared C++ contract

The common contract is `aggregates::RangeMax` in `include/range_max.hpp`.
Both backends implement these operations:

```cpp
int size() const;
void assign(int index, long long value);
long long queryMax(int left, int right) const;
```

Construct `aggregates::StructureAAdapter` or `aggregates::StructureBAdapter` from a `vector<long long>`. Both are declared in `include/structure_adapters.hpp` and can be used through a `RangeMax&`. A fully constructed adapter is ready for queries; there is no unbuilt state in this shared interface. The interface has a virtual destructor.

- Size: 0 through 1,000,000 elements. It stays fixed after construction.
- Shared values: the full `long long` domain, including negative values and `LLONG_MIN`.
- Assignment: replace the value at `0 <= index < size()`. Both increases and decreases are supported. No output is produced.
- Ranges: zero-based **[left,right)**, with `0 <= left <= right <= size()`.
- Empty maximum: `LLONG_MIN`, exposed as `aggregates::emptyMaximum`. This also may be a real value; range length distinguishes empty from nonempty maximum-only queries.
- Invalid indices/ranges: throw `std::out_of_range` before changing state. Oversized construction throws `std::length_error`. Allocation failures remain standard allocation exceptions.
- Operations are sequential; concurrent calls on the same instance are not supported. Even const queries may update a backend's diagnostic counters.

Example: `queryMax(1,4)` covers indices 1, 2 and 3. `queryMax(0,size())` covers the whole array. `queryMax(k,k)` is empty.

## 2. Native adapters and capability boundary

| Feature | A adapter | B adapter |
|---|---|---|
| Fixed-size construction | Yes | Yes |
| Point assignment, including decreases | Yes | Yes |
| Half-open range maximum | Yes | Yes |
| Maximum and its count | `queryMaxCount(left,right)` | Not provided |
| Range gcd | `queryGCD(left,right)` | Not provided |

A delegates to `structA::StructureA`, which already uses half-open ranges. `queryMaxCount` returns `aggregates::MaximumCount {maximum,count}`. Its empty identity is `{LLONG_MIN,0}`; a nonempty all-`LLONG_MIN` range has a positive count. `queryGCD` returns an `unsigned long long` magnitude, with empty identity 0. The native A implementation handles the magnitude of `LLONG_MIN` without signed negation overflow.

B delegates to `FenwickMax`, whose native API uses inclusive ranges. For a nonempty shared query [left,right), its adapter calls `queryMax(left,right-1)`. Empty queries return the identity without calling B. The native B implementation and existing Oracle/adversary APIs retain their current indexing; only the shared adapters normalize the public A/B contract. Existing oracle tests and benchmark adapters are not silently reinterpreted.

Count and gcd are deliberately absent from the common maximum-only interface: the specified B structure does not implement them. A `b-hybrid` sensor replay uses A for count and gcd and must not be reported as a pure-B aggregate implementation or a timing comparison between equivalent backends.

## 3. Structural invariants

### Structure A

1. A real leaf stores the current value's `{maximum,1}` and its unsigned gcd magnitude.
2. Padding leaves hold the monoid identities `{LLONG_MIN,0}` and 0.
3. Every internal node equals the associative merge of its children: select the larger maximum, add counts on equal maxima, and apply gcd for calibration values.
4. Assignment recomputes the affected ancestor paths before returning.
5. Query blocks partition [left,right), and ordered accumulators combine their stored results.

### Structure B

Using B's internal one-based indices and `lowbit(i) = i & -i`:

1. `a[i]` is the current array value at external index i-1.
2. `bit1[i]` is the maximum over [i-lowbit(i)+1,i], inclusive.
3. `bit2[i]` is the maximum over [i,min(n,i+lowbit(i)-1)], inclusive.
4. Assignment repairs every affected block, including when its old maximum decreases. The increase-only prefix maximum rule is not a valid replacement.
5. The two query walks and their pivot cover the requested inclusive range. The wrapper's right-1 conversion therefore covers exactly the shared half-open interval.

`validAgainst(values)` is a debug helper outside the common production contract. A audits against expected leaves; B compares its persistent blocks against a rebuild. It is intended for small tests, not per-operation full-scale timing.

## 4. Sensor application

`include/sensor_replay.hpp` defines `Sensor`, `Answer`, `Backend` and `SensorReplay`. The runnable entry point remains `app/gcd_segment_tree.cpp`.

Each sensor has a separate reading and calibration factor. The application restricts both fields to [0,1,000,000,000], validates constructor and assignment inputs, and allows at most 1,000,000 sensors. It uses:

- `--backend=a` (default): native Structure A for reading maximum/count and a native A gcd tree for calibration factors.
- `--backend=b-hybrid`: native Structure B through the shared interface for reading maximum, A for the reading maximum's count, and an A gcd tree for calibration factors.

The A reading wrapper also maintains its native gcd aggregate; it is unused by this replay. The hybrid maintains both reading structures. This additional storage and update work must be included if measuring the whole application. Use the existing maximum-only benchmark for an A/B data-structure comparison.

Input format for the **local sensor replay**:

```text
n operations
reading_0 calibration_0
... n pairs ...
U index new_reading new_calibration
Q left right
```

`U` replaces both fields at one index. `Q` prints `maximum count gcd` over [left,right). Empty queries print `-1 0 0`, an application-level display convention for nonnegative readings; the common maximum-only identity remains `LLONG_MIN`. A nonempty all-zero reading range reports maximum 0 and the correct positive count.

Malformed input, invalid commands or invalid indices/ranges produce an error on stderr and exit status 1. Valid queries print only their results on stdout. Earlier output is not rolled back if a later command fails. Extra tokens after the declared operation count are ignored. The operation count is a nonnegative int.

Optional `--test` runs one million seeded small-array operations with differential and invariant checks. `--scale` runs one million operations on one million sensors and checks 50 sampled range queries against an array scan. Either mode can be combined with either backend selector; duplicate or unknown arguments are rejected.

The sensor wrapper's invariants are that A's reading view, optional B reading view, the calibration tree, and its debug snapshot describe the same sensors. Validation occurs before either field is assigned. Queries use the same [left,right) for both fields.

## 5. Complexity and arithmetic model

Let H=1+log2(n+1), L=1+log2(V+1), and V bound all calibration/gcd magnitudes throughout the replay. Both backends use O(n) storage. A construction has the safe bound O(nL), A maximum/count queries cost O(H), and gcd queries cost O(H+L) by Euclidean telescoping; A assignments retain the safe O(HL) bound. B's current construction scans its lowbit blocks, costing O(nH); its maximum queries and current assignments cost O(H). The constant-time abstract-call overhead does not alter these bounds.

The gcd bound counts Euclidean remainder steps plus traversal. It is a time bound on a word RAM only when values and indices fit machine words and division/remainder is constant-cost. It is not a bit-complexity claim for arbitrary-precision integers. The `b-hybrid` full query includes A's count query and calibration gcd query and therefore costs O(H+L); a hybrid assignment includes A's update work and retains O(HL). Debug audits and test-oracle scans are excluded from these production-operation bounds. See `docs/derivations/D3_250041005.pdf` for the proof.

## 6. Instructor evaluation entry point

The instructor has not yet published the official `run_ops` input/output format (confirmed by the Architect). The existing sensor replay format above is not asserted to be the examiner's format. There is no placeholder `run_ops` binary claiming compliance.

When the official format is provided, add `app/run_ops.cpp` as its parser/serializer over the shared adapters, map its index convention explicitly, implement exactly its specified capabilities, add official fixtures, add the Makefile target for `bin/run_ops`, and support `./bin/run_ops --structure A` and `./bin/run_ops --structure B`. Unsupported B count/gcd requests must be handled according to that format rather than fabricated or silently supplied by A.
