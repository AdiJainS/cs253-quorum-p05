# D2 speaker notes (Vishruth Agrawal, Structure B)

Your slot is 3 to 4 minutes. In the deck it is slides 4, 5 and 6. Aim for about 4:00 in total: slide 4 about 1:15, slide 5 about 1:35, slide 6 about 1:10.

---

## Part 1. What you have to prove

The PS (D2) says: "The Dima-Ceterchi range-maximum query is correct and runs in O(log n), and point updates including decreases are correct. Prove it."

That is three claims. They are the three theorems on your slides.

1. **Theorem 1: the query is correct.** queryMax(l, r) returns the true maximum of a[l..r].
2. **Theorem 2: the query is fast.** It reads at most 2*floor(log2 w) + 1 stored values, where w = r - l + 1. For n = 10^6 that is 39.
3. **Theorem 3: the update is correct, even when a value goes down.** After assign(p, v), every stored group value is again the true maximum of its group.

Your own segment is marked on three things: your module and the design decision behind it (2 marks), your derivation presented correctly (2 marks), and your answers to questions (2 marks).

---

## Part 2. The proof, taught from zero

### 2.1 The setting

We want the maximum of a range, with updates that may lower a value. A normal Fenwick tree gets a range sum as prefix(r) minus prefix(l-1). Maximum cannot be subtracted, so we need another idea.

The idea: store the maximum of many **groups** of positions, and answer a query by gluing a few groups together. Neither tree ever subtracts.

### 2.2 lowbit

lowbit(i) is the value of the lowest 1-bit of i. In code it is `i & -i`.

- lowbit(6) = 2, lowbit(7) = 1, lowbit(8) = 8, lowbit(12) = 4.
- Another way to say it: if lowbit(i) = 2^k, then i = (an odd number) times 2^k.

### 2.3 The two trees

Positions are 1..n. We store two arrays.

- `tree[i]` is the max of the group that **ends** at i and has length lowbit(i): positions i-lowbit(i)+1 .. i.
- `mirror[i]` is the max of the group that **starts** at i and has length lowbit(i): positions i .. i+lowbit(i)-1 (cut off at n).

Worked example, a = [3, 2, 5, 1, 4, 6, 2, 7] (positions 1 to 8):

| i | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
|---|---|---|---|---|---|---|---|---|
| a[i] | 3 | 2 | 5 | 1 | 4 | 6 | 2 | 7 |
| tree[i] | 3 | 3 | 5 | 5 | 4 | 6 | 2 | 7 |
| mirror[i] | 3 | 5 | 5 | 6 | 4 | 6 | 2 | 7 |

For example tree[6] covers positions 5..6, so it is max(4, 6) = 6. mirror[4] covers positions 4..7, so it is max(1, 4, 6, 2) = 6.

The **invariant** (the rule that must always stay true) is: every stored value equals the true maximum of its group.

### 2.4 The query, in words

To get the max of [l, r]:

- Walk **right from l**: take mirror[x], then jump to x + lowbit(x). Keep going while x + lowbit(x) is still at most r.
- Walk **left from r**: take tree[y], then jump to y - lowbit(y). Keep going while y - lowbit(y) is still at least l.
- Both walks end at the same position. Take a[that position]. The answer is the max of everything taken.

Example, max of positions 3..7 on the array above. Left walk: x = 3, take mirror[3] = 5, jump to 4, and 4 + 4 = 8 is more than 7, so stop. Right walk: y = 7, take tree[7] = 2, jump to 6, take tree[6] = 6, jump to 4, and 4 - 4 = 0 is less than 3, so stop. Then take a[4] = 1. The max of 5, 2, 6, 1 is 6. Correct.

### 2.5 Three facts that make the proof work

**Fact 1: lowbit at least doubles.**
Let lowbit(x) = 2^k, so x = odd times 2^k. Then x + 2^k = (odd + 1) times 2^k, and odd + 1 is even, so the result is a multiple of 2^(k+1). The same works for x - 2^k. So every jump lands on a number with at least twice the lowbit. After j jumps the lowbit is at least 2^j.

**Fact 2: every interval has exactly one "top".**
In [l, r], exactly one position has the largest lowbit. Call it k.
Why: suppose two different positions u < v both have the largest lowbit 2^m. Both are odd multiples of 2^m, so between them there is a multiple of 2^(m+1), namely u + 2^m. It lies inside [l, r] and has a larger lowbit. That contradicts "largest".

**Fact 3: the walks cannot jump over the top.**
If x is left of the top k and lowbit(x) = 2^j, then x sits strictly between two multiples of 2^(j+1), and k is a multiple of 2^(j+1) (because k's lowbit is bigger). So the next multiple after x, which is x + 2^j, is at most k. So the jump never goes past k. Same story from the right for y.

### 2.6 Theorem 1 (correct), in plain words

1. By Fact 3 both walks keep moving until they reach k, and they cannot go past it. At k they must stop: if the left walk could jump again from k, the landing spot would be inside [l, r] with a bigger lowbit than k, which Fact 2 forbids. (Same for the right walk.)
2. The mirror groups taken on the left walk are [x, x+lowbit(x)-1], and the next one begins at x+lowbit(x). So they sit side by side with no gap and no overlap, and together they cover l..k-1. The tree groups on the right walk cover k+1..r the same way. The single value a[k] covers k.
3. So the pieces tile [l, r] exactly, with nothing missed and nothing counted twice. Each stored value is the true max of its piece (the invariant). So the max of the pieces is the max of the range.

### 2.7 Theorem 2 (O(log n)), in plain words

- Each jump moves at least lowbit, and lowbit at least doubles, so after s jumps we have moved at least 1 + 2 + 4 + ... = 2^s - 1 positions.
- We never move further than r - l = w - 1 positions. So 2^s - 1 is at most w - 1, which gives s at most floor(log2 w).
- Two walks plus the final a[k] gives at most 2*floor(log2 w) + 1 reads. For n = 10^6: floor(log2 10^6) = 19, so 2*19 + 1 = **39**. Our measured maximum was 31.

### 2.8 Theorem 3 (update correct), in plain words

After a[p] = v, only groups that contain p can have a wrong value. All other groups do not contain p, so they did not change.

**Which groups contain p?**
- For `tree`: the groups on the up-chain p, p + lowbit(p), and so on, while at most n. Each next group has at least double the length and reaches further left, so it still holds p. Any index not on the chain has a group that starts after the chain index just below it, which is at least p, so it misses p.
- For `mirror`: the groups on the down-chain p, p - lowbit(p), and so on, while at least 1. This is the mirror image of the same argument.

**What do we do with them?**
We recompute each of those groups from the current array. We never look at the old stored value. That is why a decrease is no harder than an increase.

**Why the lazy rule fails.**
A lazy rule would say tree[i] = max(old tree[i], v). That is right when the value goes up. When it goes down it keeps the old, too-big maximum.
Example: a[6] goes from 6 to 1. tree[6] covers positions 5..6 and was 6. The lazy rule gives max(6, 1) = 6. The true maximum is max(4, 1) = 4. Wrong.

**Our worked case: assign(6, 1) on the array above.**
- Up-chain for tree: 6, then 8. Recompute tree[6] = max(a5, a6) = max(4, 1) = **4**. Recompute tree[8] = max of everything = **7**.
- Down-chain for mirror: 6, then 4, then stop (4 - 4 = 0). Recompute mirror[6] = max(a6, a7) = max(1, 2) = **2**. Recompute mirror[4] = max(a4..a7) = max(1, 4, 1, 2) = **4**.
- Every other group is untouched and still correct. The deck shows 4, 7, 2, 4.

---

## Part 3. What to say (the script)

Say it in your own voice. Short sentences are fine. The text in square brackets is what to do on the slide, not words to speak.

### Opening line (before slide 4, as the handoff from the previous presenter, 10 seconds)

"Structure A answered ranges with a segment tree. I will show the second design, which uses Fenwick trees, and prove that it is correct."

### Slide 4: Structure B (about 1:15)

[Slide builds. The assign(6, 1) tag is shown.]

"Structure B is the Dima and Ceterchi range-maximum structure. The problem is that a normal Fenwick tree answers a range as one prefix minus another, and maximum cannot be subtracted. So I keep two arrays. `tree[i]` is the maximum of the group that **ends** at i, and `mirror[i]` is the maximum of the group that **starts** at i. A group at i has length lowbit of i, the lowest set bit."

[Code panel highlights lowbit, then updateTree, updateMirror.]

"Here is my code. `lowbit` is `i & -i`. `updateTree` walks up from the changed position and recomputes each group on the way. `updateMirror` walks down and does the same for the mirror."

[Query demo runs: queryMax(3, 7).]

"For a query, I walk in from both ends. From the left I take mirror groups, from the right I take tree groups, and the two walks meet at one middle element. Here, for positions 3 to 7, I read 5, then 2 and 6, then the middle value 1, and the answer is 6."

"The design decision: mirror alone is correct but a query can cost log squared n. Using both trees brings it down to log n, at most 39 reads for a million elements. And an update recomputes the groups instead of trusting the old value, so it stays correct when a value is lowered. The baseline, a prefix-only tree, has to rebuild everything, which is order n, on a decrease."

### Slide 5: D2 part 1, the query (about 1:35)

[Slide builds the trace for 3..7.]

"I will now prove the query is correct and fast. Everything rests on `lowbit`: if lowbit of i is 2 to the k, then i is an odd number times 2 to the k."

[Fact 1 appears.]

"Fact one: a jump at least doubles the lowbit. If x is odd times 2 to the k, then x plus 2 to the k is an even number times 2 to the k, so it is a multiple of 2 to the k plus one."

[Fact 2 appears.]

"Fact two: every interval has exactly one position with the largest lowbit. I call it the top. If there were two, there would be a multiple of the next power of two between them, which has a bigger lowbit."

[Fact 3 appears.]

"Fact three: the walks cannot jump over the top. A position with a smaller lowbit sits strictly between two multiples of the next power of two, and the top is one of them. So the next jump lands at or before the top."

[Theorem 1 appears.]

"Theorem one, correctness. Both walks end exactly at the top. The mirror groups tile everything left of the top, the tree groups tile everything right of it, and a of k covers the top itself. So the pieces cover the range exactly once, each piece stores its true maximum, and so the maximum of the pieces is the maximum of the range."

[Theorem 2 appears.]

"Theorem two, cost. Each jump at least doubles the step size and the walk never travels further than the width w. So each walk takes at most log base 2 of w steps, and with the middle element that is at most 2 floor of log w plus 1 reads. For a million elements, that is 39."

### Slide 6: D2 part 2, updates and decreases (about 1:10)

[Slide shows a[6] going from 6 to 1.]

"Now updates. After a value at position p changes, only groups containing p can be wrong. Lemma three says those are exactly the tree groups on the up-chain from p, and Lemma four says the mirror groups on the down-chain. The chain argument is that each next group is at least twice as long and still reaches back to p, and every index off the chain starts after p."

[Lazy-rule animation.]

"The tempting shortcut is `max(old, v)`. It works for increases. For a decrease it keeps the old maximum. Here a[6] drops from 6 to 1. The group for tree[6] should become 4, but the lazy rule leaves it at 6."

[Chain recompute shows 4, 7, 2, 4.]

"So my update recomputes every group on the two chains from the current array. Here that gives 4, 7, 2 and 4, which are the true maxima."

[Theorem 3 appears.]

"Theorem three: after the update every group on the chains is correct because we recompute it, and every other group did not contain p so did not change. The invariant holds again, and Theorem one then says every query is still correct. This was tested against the plain-array oracle with more than a million checks and zero mismatches. Thank you. I will hand over to Anushri."

(Check the handoff against the slide order: slide 7 is Anushri's D3. If the order changes, change the name.)

### If you have 20 extra seconds

"In the final code I use a faster update that gets log n per assign instead of rescanning the groups. It is checked against this simple version."

---

## Part 4. Questions you will probably be asked, with answers

**Q1. What is the cost of one update in your simple code?**
Honest answer: the simple code touches at most 2 times (floor of log2 n + 1) groups, but it rescans each group from the array, and a group at i has size lowbit(i). On the up-chain the sizes at least double, so the total rescan can reach order n (for example assign at position 1 recomputes tree[2^19], which covers 2^19 elements). So the **simple version is correct but not O(log n) per update**. Our repository's final version avoids the rescan: it first climbs once to the right and once to the left, recording the maximum of the unchanged pieces, and then each group is recomputed in constant time from those records. That gives at most 4 times (floor of log2 n + 1) steps, so **O(log n) worst case**. Measured maximum for n = 10^6 was 40 against a limit of 80. The PS only asks us to prove that updates are *correct*, and the query is the part we prove is O(log n).

**Q2. Why can't you use a normal Fenwick tree?**
A Fenwick tree answers a range as prefix(r) minus prefix(l-1). Maximum has no inverse: you cannot undo "max with 10". Two different arrays can have the same prefix maxima and different range maxima (D4 shows this).

**Q3. Why two trees, why not just mirror?**
Mirror alone can only take groups starting at positions walking right, and it ends up needing log n groups to get to r, then each of those may require more. It is correct but a query can cost log squared n. Walking in from both ends meets at the top after at most log w steps each.

**Q4. What is the "top" and why is it unique?**
The position in [l, r] with the largest lowbit. If two positions had the same largest lowbit 2^m, both are odd multiples of 2^m, so there is a multiple of 2^(m+1) between them, which also lies in the range and has a larger lowbit. Contradiction.

**Q5. Why does the query take at most 2 floor(log2 w) + 1?**
Each walk's jump sizes at least double (1, 2, 4, ...), so s jumps cover at least 2^s - 1 positions. They cannot cover more than w - 1. So s is at most floor(log2 w). Two walks plus the top: 2*floor(log2 w) + 1. For w = 10^6, that is 39.

**Q6. Why is it 31 measured but 39 the bound?**
39 is the worst case for a width of 10^6 and an unlucky range. Typical queries read fewer. The bound is an upper limit, and the measured number is below it, which is what we want.

**Q7. How do you know the update touches every group that contains p?**
Lemma 3 and 4. For `tree`, the groups containing p are exactly the up-chain from p. For `mirror`, exactly the down-chain from p. Each next group on the chain at least doubles in length and reaches back past p, so it keeps containing p. Any index not on the chain begins after the chain index just below it, which is at least p, so its group misses p.

**Q8. Why does a decrease break the lazy rule, and how does yours fix it?**
The lazy rule replaces tree[i] with max(old, v). If the old tree[i] was equal to the old a[p], the true maximum may now be lower, but the rule keeps the old value. Ours throws away the old value and recomputes the group from the current array.

**Q9. What happens if there are equal values?**
No problem. We only need the maximum value, not which position holds it. The recompute does not care about ties. (The count of the maximum is Structure A's job, via the (maximum, count) monoid.)

**Q10. What if l = r?**
Neither walk moves, since x + lowbit(x) is more than r. We return a[l].

**Q11. What is the baseline and what does it do on a decrease?**
A prefix-only Fenwick tree stores the max of prefixes only. It uses the increase-only rule max(old, v). On a decrease that rule is wrong (D5 shows [5, 3] becoming [1, 3] gives 5, truth is 3), so we rebuild the whole array in O(n).

**Q12. What did you test?**
Four test files against the plain-array oracle: queries (300,200 checks), updates including decreases (786,533 checks), a block check that after every assign each group equals a freshly built group (283,320 checks), and counters that check the step limits (235,721 checks). Zero mismatches.

**Q13. Is it a min or a max in the paper?**
The paper (Dima and Ceterchi, 2015) does range minimum. We reverse the comparison and use minus infinity as the "nothing" value. The structure is the same.

**Q14. What does the paper say about update cost, and what did you do differently?**
The paper claims an amortized O(log n) update with only a short sketch. We prove a direct worst-case bound instead, so no amortization argument is needed.

---

## Part 5. Things to remember on the day

- Say "group" for a stored block; it is the word on the slides.
- Say the four numbers once: **39** (bound), **31** (measured max reads), **4, 7, 2, 4** (the recomputed groups in the example).
- If you are asked something about D1 or D3 or D4 or D5, answer in one sentence and say whose segment it is. The viva also has one question on a teammate's module.
- If you are running out of time, skip the Fact 2 proof and say "there is exactly one top, and I can show why in the questions."
