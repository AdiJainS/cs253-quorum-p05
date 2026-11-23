# D5: Decreasing Updates in a Prefix-Maximum Fenwick Tree

Author: [Vanshika Agrawal]
Role: Adversary
Project 5: Aggregates a Fenwick Tree Cannot Support

## 1. Statement

We prove that the increase-only update rule for a prefix-maximum
Fenwick tree cannot correctly handle every point decrease.

We then construct a sequence that causes the team's rebuilding
prefix baseline to rebuild after every assignment.

## 2. Prefix-maximum invariant

Use one-based indexing internally.

Let lowbit(i) = i & (-i). A prefix-maximum Fenwick tree stores

    T[i] = max{A[j] : i - lowbit(i) + 1 <= j <= i}.

A prefix query obtains the maximum over A[1..r] by combining the
stored maxima of disjoint Fenwick blocks covering that prefix.

For an assignment at position p, the increase-only update visits

    p, p + lowbit(p), ...

and applies

    T[i] = max(T[i], newValue).

## 3. Why the rule works for increases

Suppose A[p] changes from x to y, where y >= x.

Consider any affected block. Let M be the maximum of its other
elements, with M = negative infinity if there are no other elements.

Before the assignment, its maximum is

    oldMaximum = max(M, x).

After the assignment, its maximum must be

    newMaximum = max(M, y).

Since y >= x,

    max(oldMaximum, y)
      = max(max(M, x), y)
      = max(M, y).

Therefore the increase-only rule correctly updates every affected
block for a nondecreasing assignment.

Unaffected blocks do not change, so the invariant is preserved.

## 4. Why the rule fails for decreases

Consider

    A = [5, 3].

The tree contains

    T[1] = 5
    T[2] = 5.

Assign A[1] = 1. The correct array becomes

    A = [1, 3],

so the correct maximum over the whole array is 3.

However, the increase-only rule calculates

    T[1] = max(5, 1) = 5
    T[2] = max(5, 1) = 5.

A prefix query ending at position 2 therefore returns 5 instead of 3.

This counterexample proves that the increase-only rule is not
correct for arbitrary point decreases.

The underlying issue is that the rule never lowers a stored maximum,
although the correct maximum of a block can decrease.

Also, the old block maximum and the assigned value do not determine
the new block maximum. For example, the arrays [5, 3] and [5, 2]
have the same old whole-array maximum, 5. After assigning their first
element to 1, their maxima become 3 and 2 respectively.

Repair therefore requires additional information or recomputation;
the increase-only rule alone is insufficient.

## 5. A sequence causing repeated baseline rebuilds

Use zero-based indices for the public operation interface.

For n >= 1, initialise

    A[i] = n - i, for 0 <= i < n.

Thus the initial array is

    [n, n-1, ..., 1].

For j = 0, 1, ..., k-1, where 1 <= k <= n:

1. Assign A[j] = 0.
2. Query the maximum over [0, n-1].

Immediately before assignment j:

- Positions below j contain zero.
- Position j contains n-j.
- Positions above j contain strictly smaller positive values.

Consequently, position j contains the unique global maximum.

After its assignment, the correct global maximum is n-j-1.
This formula also gives zero after the final assignment when j=n-1.

Therefore every assignment changes the whole-array maximum, and
every following query can expose a stale maximum.

## 6. Rebuild count and cost

The team's prefix baseline handles a strict decrease by assigning
the new value and calling rebuild().

Every assignment in the constructed sequence is a strict decrease,
because its previous value is positive and its new value is zero.

Therefore k assignments cause exactly k rebuilds after construction.

The current rebuild implementation makes two linear passes over
the array, with constant work per iteration. A rebuild costs Theta(n).

Hence the total rebuild work is

    Theta(k * n).

For k = n, it is

    Theta(n^2).

The k prefix queries require at most O(k log n) additional work.
Initial construction costs Theta(n). Neither changes the
Theta(kn) total bound for this workload when 1 <= k <= n.

This is a bound for the team's rebuilding baseline. It does not
claim that every Fenwick-based structure must rebuild on decreases.
The two-tree structure uses a different repair algorithm.

## 7. Implementation and verification

The construction is implemented by

    makeRebuildWorkload(n, k)

in adversary/generators.hpp.

The verification program compares:

- Direct array maximum.
- The prefix baseline with rebuilding.
- The deliberately increase-only update method.
- The two-tree Fenwick implementation.

For n=4 and k=4, the expected maxima are

    3, 2, 1, 0.

The rebuilding baseline returns these answers and records four
rebuilds. The increase-only method incorrectly returns 4 for
all four queries.

The two-tree Fenwick implementation also returns the correct
maximum after every assignment in this example.

## 8. References

1. CS 253 / MA 253 laboratory project brief:
   Project 5, D5, and common submission requirements.
2. P. M. Fenwick, "A new data structure for cumulative frequency
   tables," Software: Practice and Experience, 24(3), 1994,
   pages 327-336.
3. Team implementation:
   src/structB/prefix_fenwick_max.hpp.