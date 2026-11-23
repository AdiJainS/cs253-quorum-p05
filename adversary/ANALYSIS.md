# Why these workloads are adversarial

These arguments apply to the current D2 implementation and its
counter definitions. Let n = 2^h, where h >= 2.

Public indices are zero-based. Internal Fenwick indices are one-based.

## 1. Difficult queries

Our generated query is [0, n-2], or [1, n-1] internally.

The left traversal visits:

    1, 2, 4, ..., 2^(h-1)

It performs h-1 loop iterations.

The right traversal starts at n-1 and repeatedly removes its lowest
set bit. Its indices are:

    n-1, n-2, n-4, ..., n-2^(h-1)

It also performs h-1 loop iterations.

The implementation then counts one final element.

Therefore:

    queryBlocks = (h-1) + (h-1) + 1
                = 2h-1.

### Why no query takes more steps

First consider an internal right endpoint r < n.

Each left jump strictly increases the position of the lowest set
bit. The left traversal cannot reach n because it must stay at or
below r. It therefore performs at most h-1 iterations.

Each right jump removes one set bit. Since r < 2^h, r has at most
h set bits. The traversal cannot remove its last set bit because
the resulting index would be zero, below the valid left endpoint.
It therefore performs at most h-1 iterations.

Including the final element gives at most 2h-1 steps.

If r = n, the right traversal performs no iterations. The left
traversal performs at most h iterations, followed by the final
element. This gives at most h+1 steps.

For h >= 2:

    h+1 <= 2h-1.

Thus every query takes at most 2h-1 counted steps, and our generated
query attains that bound.

## 2. Difficult decreasing updates

The initial array is zero except for a unique positive maximum at
public index 0.

We repeatedly decrease this value while keeping it positive.
Every affected stored block must therefore lower its maximum.

Public index 0 corresponds to internal position p=1.

For this position:

- Right preparation takes h steps.
- Left preparation takes zero steps.
- The first tree updates h+1 blocks.
- The second tree updates one block.

Therefore each assignment records:

    assignBlocks = 2h+2
    loweredBlocks = h+2.

Blocks in the two separate trees are counted separately.

### Why the assignment-step count is maximal

Let popcount(x) mean the number of set bits in the binary
representation of x.

For any internal position p < n, the current loops take:

    Left preparation:  popcount(p-1)
    Right preparation: h - popcount(p) + 1
    First-tree update: h - popcount(p-1) + 1
    Second-tree update: popcount(p)

A downward Fenwick walk removes one set bit per iteration.
An upward walk sets successive zero-bit positions until it reaches
the power-of-two endpoint. These give the counts above.

Adding the four counts gives:

    assignBlocks = 2h+2.

For p=n, the total is h+2, which is smaller.

Thus p=1 attains the maximum assignment-step count. It is not the
only position that attains it.

### Why the lowered-block count is maximal

Let t be the number of trailing zero bits in p.

The number of affected blocks across both trees is:

    h - popcount(p-1) + 1 + popcount(p).

Binary subtraction gives:

    popcount(p) - popcount(p-1) = 1-t.

Therefore:

    affected blocks = h+2-t <= h+2.

No more blocks can lower than are affected.

At p=1, t=0, so h+2 blocks are affected. Our unique maximum decreases,
so all h+2 affected block maxima actually decrease.

Hence our construction also maximises loweredBlocks per assignment.

## 3. Full-scale verification

Parameters:

    n = 1,048,576 = 2^20
    total operations = 1,000,000

Difficult-query workload:

    Queries:          1,000,000
    Query steps:     39,000,000
    Assignments:             0
    Result: PASS

Repeated-decrease workload:

    Assignments:       500,000
    Queries:           500,000
    Assignment steps: 21,000,000
    Lowered blocks:   11,000,000
    Query steps:      10,500,000
    Result: PASS

The decrease workload uses whole-array queries. In this implementation,
each such query takes h+1 = 21 steps.

Every query answer and the expected counter totals were checked.

These are correctness and operation-count results, not timing
measurements. Timing experiments belong to the Analyst's harness.

## 4. Interpretation

The decreasing workload maximises the counted assignment work and
the number of lowered block maxima.

However, this implementation traverses its update paths even when
few stored maxima change. A larger loweredBlocks count does not
automatically imply a larger assignBlocks count.

The two counters describe different properties and are reported
separately.