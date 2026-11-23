# D1: Structure A — Maximum with Count as a Monoid and Segment Tree Range-Query Correctness

**Author:** Tia Mehra

**Roll number:** 250041039

**Role:** Structure A owner

## What we prove

The project statement is:

> (maximum, count of maximum) is a monoid with identity (-infinity, 0), and a Segment Tree answers range queries correctly for any monoid, with no inverse needed. Prove both.

The proof has two parts:

1. Prove that the summary `(maximum, count of maximum)`, with its merge operation, satisfies the monoid properties.
2. Prove that a Segment Tree built over any monoid returns the correct aggregate for every requested range, without using an inverse operation.

---

## 1. Definitions and notation

### 1.1 Summary of an interval

For a non-empty interval, let `m` be its maximum value and let `c` be the number of occurrences of that maximum. We represent the interval by the pair `(m, c)`.

The empty interval is represented by the identity summary:

\[
E=(-\infty,0).
\]

Let \(S\) be the set consisting of this empty summary and all valid non-empty summaries `(m, c)`, where `m` is a value and `c` is a positive integer.

### 1.2 Binary operation and closure

A binary operation on a set \(S\) takes any two elements of \(S\) and returns an element of \(S\). The operation is *closed* when its result always belongs to \(S\).

We denote the merge operation by \(\oplus\).

### 1.3 Associativity

An operation \(\oplus\) is associative if, for all \(A,B,C\in S\),

\[
(A\oplus B)\oplus C=A\oplus(B\oplus C).
\]

This means that the grouping of operations does not change the result. It does not mean that the order of the operands can be changed.

### 1.4 Identity element

An element \(E\in S\) is an identity if, for every \(X\in S\),

\[
X\oplus E=E\oplus X=X.
\]

### 1.5 Monoid

A monoid is a set with a closed, associative binary operation and an identity element. A monoid does not have to be commutative, and it does not have to provide an inverse for every element.

### 1.6 Inverse

An inverse of an element \(X\), with respect to identity \(E\), is an element \(Y\) satisfying

\[
X\oplus Y=Y\oplus X=E.
\]

An inverse would allow an aggregate to be undone or removed. The Segment Tree range-query procedure proved below does not need this property.

### 1.7 Segment Tree aggregate

A Segment Tree stores the aggregate of the interval represented by each node. A leaf stores the value or summary for one array position. An internal node stores the merge of the aggregate of its left child and the aggregate of its right child, in left-to-right order.

---

## 2. Proof that (maximum, count of maximum) is a monoid

### 2.1 Define the merge operation

Let

\[
X=(m_1,c_1),\qquad Y=(m_2,c_2).
\]

First define the maximum of the two summaries:

\[
M=\max(m_1,m_2).
\]

The merged summary keeps the count only from the input summaries whose maximum equals \(M\):

\[
X\oplus Y=
\left(M,
[m_1=M]c_1+[m_2=M]c_2\right).
\]

Here, \([P]\) is 1 if the statement \(P\) is true and 0 otherwise. Thus, the count is retained from the side with the larger maximum. If the two maximums are equal, both counts are included.

The operation is closed. Its first component is the larger of the two maximums. Its second component is the sum of the counts belonging to that maximum. If both inputs are empty, the result is the empty summary. Otherwise, the result is a valid non-empty summary. Therefore, merging two elements of \(S\) always produces an element of \(S\).

### 2.2 Associativity proof

Take any three summaries:

\[
A=(m_1,c_1),\quad B=(m_2,c_2),\quad C=(m_3,c_3).
\]

Let the maximum across all three summaries be

\[
M=\max(m_1,m_2,m_3).
\]

A count contributes to the final summary exactly when the maximum of its original summary is equal to \(M\). Therefore, the total count that must appear in the final summary is

\[
T=[m_1=M]c_1+[m_2=M]c_2+[m_3=M]c_3.
\]

Now consider the two possible groupings.

**First grouping: \((A\oplus B)\oplus C\).**

When \(A\) and \(B\) are merged, the operation keeps the larger of \(m_1\) and \(m_2\). If those maximums are equal, it adds their counts. When this result is merged with \(C\), the operation keeps only the contributions whose maximum equals the overall maximum \(M\). The final pair is therefore \((M,T)\).

**Second grouping: \(A\oplus(B\oplus C)\).**

When \(B\) and \(C\) are merged first, the same rule keeps the larger of \(m_2\) and \(m_3\), adding their counts if they are equal. Merging this result with \(A\) again keeps exactly the contributions whose maximum equals \(M\). The final pair is also \((M,T)\).

Thus,

\[
(A\oplus B)\oplus C=(M,T)=A\oplus(B\oplus C).
\]

Since the two groupings produce the same maximum and the same total count, the merge operation is associative.

### 2.3 Identity proof

Let the proposed identity be

\[
E=(-\infty,0).
\]

Take any non-empty summary \(X=(m,c)\). Since \(m\) is a valid finite value, it is greater than \(-\infty\). Hence the larger maximum in \(X\oplus E\) is \(m\), and only the count `c` contributes:

\[
X\oplus E=(m,c)\oplus(-\infty,0)=(m,c)=X.
\]

If the operands are reversed, the maximum is still \(m\) and the count is still `c`:

\[
E\oplus X=(-\infty,0)\oplus(m,c)=(m,c)=X.
\]

Combining two empty summaries also returns the empty summary:

\[
E\oplus E=E.
\]

Therefore, \(E\) leaves every summary unchanged from either side and is an identity element.

The operation is closed, associative, and has an identity element. Hence,

\[
\boxed{(S,\oplus)\text{ is a monoid with identity }(-\infty,0).}
\]

---

## 3. Proof of Segment Tree range-query correctness for any monoid

Let \((S,\oplus,e)\) be any monoid. Consider an array whose elements belong to \(S\). The aggregate of an interval is the ordered combination of all its elements using \(\oplus\). The proof uses closure, associativity and the identity element. It does not assume commutativity or the existence of inverses.

### 3.1 Node invariant

**Invariant:** Every Segment Tree node stores the aggregate of exactly the array interval represented by that node, with the interval's elements combined in their original left-to-right order.

**Base case — leaves.** A leaf represents one array position. It stores that position's element, which is the correct aggregate of a one-element interval. Therefore, the invariant holds for every leaf.

**Inductive step — internal nodes.** Assume that the invariant holds for the left and right children of an internal node. The interval represented by the parent is the left child's interval followed by the right child's interval. The parent stores

\[
\text{left aggregate}\oplus\text{right aggregate}.
\]

By the induction assumption, both child aggregates are correct. By the definition of the interval aggregate, their ordered merge is the aggregate of the complete parent interval. Therefore, the invariant also holds for the parent.

Starting from the leaves and applying this argument to each parent proves that every node stores the correct aggregate of its represented interval.

### 3.2 Range-query proof

The implementation uses zero-based, half-open ranges \([l,r)\): the left endpoint is included and the right endpoint is excluded.

A Segment Tree range query decomposes \([l,r)\) into selected tree-node intervals with the following properties:

1. The selected intervals are disjoint.
2. Their union is exactly \([l,r)\), so every requested position is included exactly once.
3. Their aggregates are combined in left-to-right order.

By the node invariant, every selected node contains the correct aggregate of its own interval. Since the selected intervals are disjoint and cover the query range, combining their aggregates includes every requested element exactly once and in its original order.

Associativity allows the query to group these interval aggregates in the way produced by the tree decomposition without changing the result. Because their order is preserved, commutativity is not needed. The combined result is therefore the aggregate of the complete interval \([l,r)\).

Hence, the Segment Tree range query is correct for any monoid.

### 3.3 Why an inverse is not required

The query does not calculate a range aggregate by subtracting or removing one prefix aggregate from another. It directly combines the aggregates of the disjoint Segment Tree nodes that cover the requested interval.

The identity element is used to initialise the query accumulators and to represent an empty contribution. Associativity ensures that combining the selected node aggregates with different groupings gives the same result. No step requires an element that reverses a previous merge.

Therefore, a Segment Tree range query requires an associative operation and an identity element, but it does not require an inverse.

---

## 4. Where the result is used in the implementation

The D1 result is used by the maximum-with-count instance of Structure A.

### 4.1 `src/structA/segment_tree.hpp`

- **`MaxCountNode`** stores the pair `(maximum, count)`.
- **`MaxCountOps::merge()`** implements the merge operation defined in Section 2. It keeps the larger maximum and adds the counts when the two maximums are equal.
- **`MaxCountOps::identity()`** returns the empty summary, represented in the C++ implementation by `maximum = LLONG_MIN` and `count = 0`. The zero count distinguishes this empty summary from a valid interval whose maximum is `LLONG_MIN`.
- **`SegmentTree<Node, Ops>`** is the generic tree. Its `build()` combines child nodes to form parent nodes, and its **`query(l, r)`** selects the tree blocks covering the requested half-open range. The query uses `leftResult` and `rightResult` so that selected blocks remain in left-to-right order.

### 4.2 `src/structA/structure_a.hpp`

- **`StructureA::maxTree_`** is instantiated as `SegmentTree<MaxCountNode, MaxCountOps>`.
- **`StructureA::build()`** creates a maximum-count leaf summary for every input value, with count 1, and builds the maximum tree.
- **`StructureA::queryMax(l, r)`** delegates the requested half-open range to `maxTree_.query(l, r)`.
- **`StructureA::assign(index, value)`** replaces the corresponding leaf in the maximum tree and recalculates its ancestors through the generic Segment Tree update.

The same generic Segment Tree is also instantiated for GCD. The GCD operation's algebraic properties and its tighter complexity analysis are covered separately by D3; this D1 proof establishes the maximum-with-count monoid and the general range-query argument.

### 4.3 Tests that check the implementation

- **`tests/structA/test_query.cpp` — `main()`** checks all non-empty ranges on the test array against the plain-array Oracle and checks the empty-range identity result.
- **`tests/structA/test_update.cpp` — `checkAllRanges()` and `main()`** compare range results with the Oracle after point assignments, including decreases and increases.
- **`tests/structA/test_invariants.cpp` — `main()`** calls `debugCheckAgainst()` after updates to check that the stored leaves and internal nodes remain consistent.
- **`tests/structA/test_differential.cpp` — `main()`** compares generated assignment, maximum-query and GCD-query operations against the Oracle.
- **`tests/structA/test_counters.cpp` — `main()`** records selected query blocks and update visits to check the expected Segment Tree traversal bounds.

These tests provide implementation checks; the mathematical correctness of D1 follows from the proofs in Sections 2 and 3.

---

## References

1. CS 253 / MA 253, *Data Structures and Algorithms Laboratory: Lab Project Descriptions, Evaluation Criteria and Reading*, Project 5, pp. 10–11. The D1 statement and Structure A requirements are specified there.
2. `src/structA/segment_tree.hpp` — Structure A node, operation and generic Segment Tree implementation.
3. `src/structA/structure_a.hpp` — Structure A construction, point assignment and public range-query interface.
