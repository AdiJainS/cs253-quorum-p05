# D6 derivation review

The prefix-baseline derivation and the 20 November measured check are in `docs/derivations/extra/D6_prefix_supplement.pdf`; regenerate it from `docs/derivations/sources/make_d6_pdf.py` run from the repository root. The earlier draft and pre-measurement planning notes are retained. Adi should review the PDF before submission. The team still needs to select release revisions and confirm the separate D3 GCD proof.

20 November update: `REPORT.md` now includes reruns with the current D1 header and both counters disabled. B's local counter switch and the dated result folder resolve the earlier instrumentation mismatch for these new timings. The PDF still needs the author's final review and the team's release decision. Earlier review notes below describe the earlier runs.

The current `D6_250001083.pdf` names Adi Jain and covers the segment tree, two-tree Fenwick, prefix baseline and array oracle. Its predictions can be compared with the observations in `REPORT.md`. The 20 November timing build disabled counters in both structures. The team still needs to confirm release revisions and D3's GCD proof before treating the comparison as final.

Before submitting:

- Explain why a segment-tree assignment visits one root path and why a query uses a bounded number of blocks per level.
- For D2's constructor, explain the block-scan cost. At `n = 2^h`, blocks of length `2^k` occur about `n / 2^(k+1)` times at each level, so scanning them contributes about `n/2` values per level. Across `h` levels this gives `O(n log n)` work; this is a model of the committed constructor, not the paper's linear construction.
- Explain why each decrease in the prefix baseline causes a rebuild and why the resulting workload term is proportional to `n` times the number of decreases.
- For GCD, state whether a count means tree blocks or Euclidean remainders. The tighter telescoping proof is D3's derivation; confirm its assumptions with the architect. The planning draft includes the updated build, query and update remainder-step terms.
- D2's `changedBlocks` and `loweredBlocks` count changes in stored values; they are not a count of every operation performed. Do not add those counts to other counters as if the work were disjoint.
- Review the prefix PDF and confirm the release revisions. Keep the original pre-measurement planning commit in the repository history. Do not describe a post-measurement adjustment as an original prediction.
- Review all code and recorded tool use before submission, and be ready to explain your own work.
