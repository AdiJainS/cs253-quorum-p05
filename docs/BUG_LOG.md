# Bug log

This D4 shrinker run deliberately injected an omitted Structure B assignment. It demonstrates the differential shrinker and is not a naturally occurring defect in the production Fenwick implementation.

| Date found | Module | Symptom | Found by | Fix / disposition |
| --- | --- | --- | --- | --- |
| 2026-11-16 | D2 / Structure B (injected fault) | Starting from `[5, 10, 7, 3, 8]`, omission of `ASSIGN 4 6` leaves index 4 at 8. The subsequent inclusive query `[3,4]` returns stale 8 instead of oracle value 6. Shrinker reduced the five operations to `ASSIGN 4 6` and `QUERY_MAX 3 4`. | D4 differential shrinker with omitted assignment deliberately injected | No production fix needed; the fault was injected in the harness. The actual Fenwick update path passed the original sequence. Demonstration added in commit `f7ded3e`. |
