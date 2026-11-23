# D2: Structure B (Dima-Ceterchi range maximum)

Author: Vishruth Agrawal (Roll No. 250001078), Structure B owner

## What we prove

1. **Query is correct.** `queryMax(l, r)` returns the maximum of `a[l..r]`.
2. **Query is fast.** It reads at most `2*floor(log2(w)) + 1` blocks, where `w = r - l + 1`. So it is O(log n).
3. **Update is correct, decreases included.** After `assign(p, v)` every stored block value is again the true maximum of its block.
4. **Update is fast.** It does at most `4*(floor(log2(n)) + 1)` steps in the worst case. So it is O(log n). This is a worst-case bound, so no "amortised" argument is needed.

We work with maximum. The paper uses minimum, so every comparison is reversed. The "nothing here" value is minus infinity (`LLONG_MIN` in the code).

---

## 0. Setup

Positions are `1..n`. The array is `a[1..n]`.

`lowbit(i)` is the value of the lowest 1-bit of `i`. In code it is `i & -i`.
Examples: `lowbit(12) = 4`, `lowbit(6) = 2`, `lowbit(7) = 1`, `lowbit(8) = 8`.

For every position `i` there are two blocks:

- `B1(i) = [i - lowbit(i) + 1, i]` is the block that **ends** at `i`.
- `B2(i) = [i, min(n, i + lowbit(i) - 1)]` is the block that **starts** at `i`.

We store two arrays:

- `bit1[i]` = maximum of `a` over `B1(i)`
- `bit2[i]` = maximum of `a` over `B2(i)`

**Invariant I:** every stored value equals the true maximum of its block.

`build` makes I true, because it computes each block maximum directly from the definition.

Example with `a = [3, 2, 5, 1, 4, 6, 2, 7]`: `B1(6) = [5, 6]`, so `bit1[6] = max(4, 6) = 6`. `B2(4) = [4, 7]`, so `bit2[4] = max(1, 4, 6, 2) = 6`.

---

## 1. Three small facts about lowbit

**Fact 1 (lowbit at least doubles).**
Let `i` have lowbit `2^k`. Then:
- `i + lowbit(i)` has lowbit at least `2^(k+1)`.
- `i - lowbit(i)`, if it is not 0, has lowbit at least `2^(k+1)`.

*Why.* Write `i = c * 2^(k+1) + 2^k`. Then `i + 2^k = (c + 1) * 2^(k+1)` and `i - 2^k = c * 2^(k+1)`. Both are multiples of `2^(k+1)`.

So if we keep adding `lowbit` (or keep subtracting it), the lowbit at step `j` is at least `2^j`.

**Fact 2 (every interval has one "top" element).**
In any interval `[l, r]` with `l >= 1`, exactly one element has the biggest lowbit. Call it the **top** of the interval. All other elements have a strictly smaller lowbit.

*Why.* Suppose two different elements `u < v` both have the biggest lowbit `2^m`. Both are odd multiples of `2^m`, so `v >= u + 2^(m+1)`. Then `u + 2^m` is a multiple of `2^(m+1)`, and it lies between `u` and `v`, so it is inside the interval. Its lowbit is at least `2^(m+1)`, which is bigger. That contradicts the choice of `2^m`.

**Fact 3 (walks do not jump over the top).**
Let `k` be the top of `[l, r]`.
- (a) If `l <= x < k`, then `x + lowbit(x) <= k`.
- (b) If `k < y <= r`, then `y - lowbit(y) >= k`.

So starting at `l` and repeatedly adding lowbit, we land exactly on `k`. Starting at `r` and repeatedly subtracting lowbit, we also land exactly on `k`.

*Why (a).* `x` is not the top, so `lowbit(x) = 2^j` with `2^j` smaller than `lowbit(k)`. Then `x` is an odd multiple of `2^j`, so the next multiple of `2^(j+1)` after `x` is `x + 2^j`. And `k` is a multiple of `2^(j+1)` bigger than `x`. So `k >= x + 2^j`.
*Why (b).* The same idea: the previous multiple of `2^(j+1)` before `y` is `y - 2^j`, and `k` is one of those multiples below `y`.

---

## 2. The query is correct

`queryMax(l, r)` works on positions `l..r` (1-based) like this.

- **Left walk.** Start at `x = l`. While `x + lowbit(x) <= r`: take `bit2[x]`, then set `x = x + lowbit(x)`.
- **Right walk.** Start at `y = r`. While `y - lowbit(y) >= l`: take `bit1[y]`, then set `y = y - lowbit(y)`.
- Finally take `a[x]`, the single value where the left walk stopped.

**Claim A: both walks stop at the same position, and it is `k`, the top of `[l, r]`.**

*Left walk.* For every `x < k` in the walk, Fact 3(a) says `x + lowbit(x) <= k <= r`. So the walk does not stop and does not jump over `k`. It keeps moving up, so it reaches `k`. At `x = k` the walk must stop: if `k + lowbit(k) <= r`, that number would be inside `[l, r]` and have a bigger lowbit than `k` (Fact 1), but `k` is the top. So the left walk stops exactly at `k`.

*Right walk.* For every `y > k`, Fact 3(b) says `y - lowbit(y) >= k >= l`. So the walk keeps going down and reaches `k`. At `y = k` it must stop: if `k - lowbit(k) >= l`, that number is at least 1, lies inside `[l, r]`, and has a bigger lowbit than `k`, which is again impossible.

**Claim B: the blocks we take cover `[l, r]` exactly once.**

- A left step at `x` takes `bit2[x]`, the maximum over `B2(x) = [x, x + lowbit(x) - 1]`. The next position is `x + lowbit(x)`, so this block is `[x, next - 1]`. (It is not cut by `n`, because `x + lowbit(x) <= r <= n`.) Consecutive left blocks join with no gap and no overlap, and together they cover `[l, k - 1]`.
- A right step at `y` takes `bit1[y]`, the maximum over `B1(y) = [y - lowbit(y) + 1, y] = [next + 1, y]`. Together the right blocks cover `[k + 1, r]`.
- The last value `a[k]` covers `{k}`.

So the three parts are `[l, k-1]`, `{k}` and `[k+1, r]`. Together they are exactly `[l, r]`, and no position is counted twice. By invariant I, each stored value is the true maximum of its block, so the maximum of everything we took is the maximum of `a[l..r]`. **The query is correct.**

If `l = r`, neither walk moves and we return `a[l]`.

Example: `l = 3, r = 7` on the array above. The top of `[3, 7]` is `4` (lowbit 4). Left walk: `x = 3`, `3 + 1 = 4 <= 7`, take `bit2[3] = 5`, move to 4; then `4 + 4 = 8 > 7`, stop. Right walk: `y = 7`, take `bit1[7] = 2`, move to 6; take `bit1[6] = 6`, move to 4; then `4 - 4 = 0 < 3`, stop. Add `a[4] = 1`. The maximum of `5, 2, 6, 1` is `6`, which is the true answer.

---

## 3. The query is O(log n)

In the left walk, the lowbit at step `j` is at least `2^j` (Fact 1). After `s` steps we have moved at least `1 + 2 + ... + 2^(s-1) = 2^s - 1` positions. We never move more than `r - l = w - 1` positions. So `2^s - 1 <= w - 1`, which gives `s <= floor(log2 w)`.

The right walk is the same. So the query reads at most `floor(log2 w) + floor(log2 w) + 1 = 2*floor(log2 w) + 1` blocks. Each block read takes constant time. So the query is `O(log w) <= O(log n)`.

(Our counter tests agree: for `n = 10^6` the most blocks any query read was 31, and the limit is 39.)

---

## 4. The update is correct, decreases included

`assign(p, v)` sets `a[p] = v`. A block whose range does not contain `p` is not changed, so invariant I still holds for it. We only need to fix the blocks that **contain** `p`.

### 4.1 Which blocks contain p

**Lemma 5.**
- (i) `B1(x)` contains `p` exactly when `x` is on the chain `p, p + lowbit(p), p + lowbit(p) + lowbit(...), ...` (while `x <= n`).
- (ii) `B2(y)` contains `p` exactly when `y` is on the chain `p, p - lowbit(p), ...` (while `y >= 1`).

*Proof of (i), "only if".* If `p` is in `B1(x)`, then `x - lowbit(x) < p <= x`. Every element of `[p, x]` other than `x` lies strictly between `x - lowbit(x)` and `x`, so its lowbit is smaller than `lowbit(x)`. So `x` is the top of `[p, x]`, and by Fact 3 the chain starting at `p` lands on `x`.
*"If".* Going one step along the chain, from `x_t` to `x_(t+1) = x_t + lowbit(x_t)`, the block grows: its end moves right, and its start `x_(t+1) - lowbit(x_(t+1)) + 1` is at most `x_t + lowbit(x_t) - 2*lowbit(x_t) + 1 = x_t - lowbit(x_t) + 1` (Fact 1). So each block contains the previous one. The first block `B1(p)` contains `p`, so they all do.

*Proof of (ii), "only if".* If `p` is in `B2(y)`, then `y <= p < y + lowbit(y)`. Every element of `[y, p]` other than `y` lies strictly between `y` and `y + lowbit(y)`, so its lowbit is smaller. So `y` is the top of `[y, p]`, and Fact 3(b) says the downward chain from `p` lands on `y`.
*"If".* Going from `y_t` to `y_(t+1) = y_t - lowbit(y_t)`, the start moves left, and the end `y_(t+1) + lowbit(y_(t+1))` is at least `y_t - lowbit(y_t) + 2*lowbit(y_t) = y_t + lowbit(y_t)` (Fact 1). So each block contains the previous one, and `B2(p)` contains `p`.

### 4.2 What the new value of each such block must be

Split a block that contains `p` into three parts: the part left of `p`, the element `p` itself, and the part right of `p`. Its new maximum is

`max( v, max of left part, max of right part )`.

The left and right parts do not contain `p`, so their old values are still valid. **We never use the old value of `a[p]` or the old stored maximum of the block.** That is why decreases are no harder than increases.

Why a naive update fails: if we only did `stored = max(stored, v)`, a block whose old maximum was `a[p]` would keep that old, now too big, value. Example: `a = [3,2,5,1,4,6,2,7]`, set `a[6] = 1`. The block `B1(6) = [5, 6]` had maximum 6. The naive rule keeps 6. The true maximum is `max(4, 1) = 4`.

### 4.3 How the code finds the left and right parts quickly

The code first does two climbs. They only read blocks that do not contain `p`, and they finish before any write.

- **Right climb.** Start at `q = p + 1`. While `q <= n`: remember `rightBefore[q] = run`, then `run = max(run, bit2[q])`, then `q = q + lowbit(q)`. Each `bit2[q]` block lies right of `p`. The blocks `B2(q)` join one after another. So `rightBefore[q]` is the maximum of `a[p+1 .. q-1]`. At the end, `rightAll` is the maximum of `a[p+1 .. n]`.
- **Left climb.** Start at `u = p - 1`. While `u >= 1`: remember `leftBefore[u] = run`, then `run = max(run, bit1[u])`, then `u = u - lowbit(u)`. Each `bit1[u]` block lies left of `p`. So `leftBefore[u]` is the maximum of `a[u+1 .. p-1]`. At the end, `leftAll` is the maximum of `a[1 .. p-1]`.

**Lemma 6 (every value we look up was recorded).**

1. *Right side.* We look up `rightBefore[x]` for each `x > p` on the first chain (4.1 i), and `rightBefore[z]` for `z = y + lowbit(y)` when `y` is on the second chain and `z <= n`. Both are on the right climb.
   - For `x`: `x - lowbit(x) < p`, so every element of `[p+1, x]` other than `x` has a lowbit smaller than `lowbit(x)`. So `x` is the top of `[p+1, x]`. By Fact 3(a), the climb from `p+1` lands on `x`.
   - For `z`: `y <= p < z`, and every element of `[p+1, z-1]` is strictly between `y` and `z`, so its lowbit is smaller than `lowbit(y)`. Also `lowbit(z) >= 2*lowbit(y)` (Fact 1). So `z` is the top of `[p+1, z]` and the climb lands on `z`.
2. *Left side.* We look up `leftBefore[w]` for `w = x - lowbit(x)` (when `w >= 1`) and `leftBefore[y]` for `y < p`. Both are on the left climb.
   - For `w`: every element of `(w, p-1]` is strictly between `w` and `x`, so its lowbit is smaller than `lowbit(x)`. And `lowbit(w) >= 2*lowbit(x)`. So `w` is the top of `[w, p-1]`, and Fact 3(b) says the climb from `p-1` lands on `w`.
   - For `y < p`: `p < y + lowbit(y)`, so every element of `(y, p-1]` has a smaller lowbit than `y`. So `y` is the top of `[y, p-1]`, and the climb lands on `y`.

### 4.4 The two update loops

**Loop for `bit1`.** For each `x` on the first chain (`x = p, p + lowbit(p), ...`, while `x <= n`):

- The left part of `B1(x)` is `[x - lowbit(x) + 1, p - 1]`. Let `w = x - lowbit(x)`. Its maximum is `leftAll` if `w = 0`, and `leftBefore[w]` otherwise (Lemma 6).
- The right part is `[p + 1, x]`. It is empty when `x = p`. Otherwise its maximum is `max(rightBefore[x], a[x])`.
- Set `bit1[x] = max(v, left, right)`.

**Loop for `bit2`.** For each `y` on the second chain (`y = p, p - lowbit(p), ...`, while `y >= 1`):

- The left part of `B2(y)` is `[y, p - 1]`. It is empty when `y = p`. Otherwise its maximum is `max(leftBefore[y], a[y])`.
- Let `z = y + lowbit(y)`. The right part is `[p + 1, min(n, z - 1)]`. Its maximum is `rightBefore[z]` if `z <= n`, and `rightAll` if `z > n`.
- Set `bit2[y] = max(v, left, right)`.

By Lemma 5 these are exactly the blocks that contain `p`, and by 4.2 each new value is the true maximum of its block. All other blocks are unchanged and still correct. So invariant I holds after the update. **The update is correct for increases, decreases and equal values.**

---

## 5. The update is O(log n)

`assign` runs four loops: the right climb, the left climb, the `bit1` loop and the `bit2` loop. In each of them the index moves by `lowbit` every step, and by Fact 1 the lowbit at step `j` is at least `2^j`. The index always stays between 1 and `n`, so `2^j <= n`, which gives `j <= floor(log2 n)`. So each loop runs at most `floor(log2 n) + 1` times.

In total `assign` makes at most `4*(floor(log2 n) + 1)` steps, and each step takes constant time. So the update is `O(log n)` in the worst case.

(Our counter tests agree: for `n = 10^6` the most steps one `assign` took was 40, and the limit is 80.)

---

## 6. What we compare with

- The paper says the update is "amortized O(log N)" after a short sketch with no proof. We proved a direct worst-case bound instead.
- The slow version (`assignSlow`) recomputes each block with two range queries. That costs O(log n) per block and O(log^2 n) per update. We keep it as a cross-check.
- The prefix-only baseline (`PrefixFenwickMax`) answers only "maximum of positions 1..r". Its increase-only rule is wrong when a value goes down, so on a decrease it rebuilds the whole array in O(n).

## 7. How the code was tested (evidence, not part of the proof)

| Test | What it checks | Result |
|---|---|---|
| `test_query.cpp` | All ranges on small arrays and random ranges on a large one, against the oracle | 300,200 checks, 0 mismatches |
| `test_update.cpp` | Generator operations, many equal values, decreases only, against the oracle | 786,533 checks, 0 mismatches |
| `test_blocks.cpp` | After every `assign`, every stored block equals a freshly built one (fast and slow versions) | 283,320 checks, 0 mismatches |
| `test_counters.cpp` | Step limits from sections 3 and 5, and the changed/lowered block counts | 235,721 checks, 0 failures |

## References

1. M. Dima and R. Ceterchi, "Efficient range minimum queries using binary indexed trees", Olympiads in Informatics 9:39-44, 2015 (sections 2.2 to 2.4).
2. P. M. Fenwick, "A new data structure for cumulative frequency tables", Software: Practice and Experience 24(3):327-336, 1994.
