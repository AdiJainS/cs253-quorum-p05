# Cache profiling

## 17 November recordings, checked on 20 November

The newer recordings are `a-current-2026-11-17.etl` and `b-current-2026-11-17.etl`. Their benchmark output is in the corresponding `.csv` files. Each contains 20 replays with n=1,000,000, 1,000,000 operations, width 1024, 50% updates, seed 253 and 50% requested decreases. A uses D1 `01da510`; B uses D2 `e925e6f`. B's counters were enabled in these recordings. They predate the counters-off timing build added on 20 November.

| Recording | Samples matching benchmark thread IDs | Samples within recorded thread lifetimes |
| --- | ---: | ---: |
| A | 1,568 | 1,568 |
| B | 4,892 | 4,891 |

B has one sample at timestamp `134393871830725691`, after its recorded process exit (`134393871830724030`) and main-thread exit (`134393871830721679`). Counting by thread ID alone includes it. The strict lifetime filter excludes it. State the filter when quoting the count: the presentation's 4,892 is the thread-ID count, while 4,891 is the lifetime-filtered count. Neither number is an exact cache-miss total or rate.

The benchmark process lifetimes were about 3.79 seconds for A and 4.75 seconds for B. Both traces include workload preparation and 20 constructions and replays. Trace summaries report zero lost ETW events. These observations do not establish a cache-performance winner or the reason for any timing difference.

`a-current-sample-counts.csv` and `b-current-sample-counts.csv` record both filters, trace SHA-256 hashes and process durations. `a-current-trace-summary.txt` and `b-current-trace-summary.txt` contain the tracerpt summaries. To reproduce the counts, export each trace and run the script:

```powershell
tracerpt results\cache\b-current-2026-11-17.etl -o .bench-deps\b-current-events.csv -of CSV -y
python bench/cache_samples.py .bench-deps/b-current-events.csv results/raw/cache/b-current-2026-11-17.etl
```

The script selects `max_benchmark_cache_current.exe` by default. Use `--process` for an executable with another name.

## Earlier recordings

WPR reported `CacheMisses`, `LLCReference` and `LLCMisses` as available sources on this machine; see `available-counters.txt`. The first attempt failed because the session lacked a required privilege; see `attempt.txt`. Running WPR as administrator produced traces for A and B.

For the workload shown below, `tracerpt` exported the traces to CSV. Filtering `PmcCounterProf` events for source 10 (`CacheMisses`) by the benchmark process's thread IDs and their lifetimes gave 1,587 samples for A and 6,412 for B. Both benchmark output files contain 20 runs and the same workload checksum. The trace summaries reported no lost ETW events.

These are samples from one recording per structure, not exact cache-miss counts or rates. They include workload generation and tree construction as well as benchmark operations. A and B also differ in their instrumentation, and A uses a provisional implementation. Repeat the recordings on final team code before drawing conclusions.

The recordings used the earlier D1 snapshot, not the 17 November rerun with D1 `01da510`.

To try the recording, open PowerShell as administrator in the project folder. First build the maximum benchmark as described in `bench/README.md`. The examples record A and B in separate processes with the same workload. Check each WPR start succeeds before running the benchmark. Use a new output file name for each trace.

```powershell
wpr -status -instancename D6Cache
wpr -start "$PWD\bench\cache_profile.wprp!Cache" -filemode -instancename D6Cache
.\.bench-deps\max_benchmark.exe 1000000 1000000 1024 50 253 20 random time-a 50 | Set-Content -Encoding utf8 results\cache\a-profile-run.csv
wpr -stop "$PWD\results\cache\a-cache.etl" -instancename D6Cache

wpr -start "$PWD\bench\cache_profile.wprp!Cache" -filemode -instancename D6Cache
.\.bench-deps\max_benchmark.exe 1000000 1000000 1024 50 253 20 random time-b 50 | Set-Content -Encoding utf8 results\cache\b-profile-run.csv
wpr -stop "$PWD\results\cache\b-cache.etl" -instancename D6Cache
```

The WPR profile samples the Windows `CacheMisses` source once per 65,536 events. Open the ETL files in Windows Performance Analyzer, filter to `max_benchmark.exe`, and report the sampled event type and sample counts along with the settings. Samples are not exact cache-miss totals or a cache-miss rate. The trace also includes workload generation and tree construction, plus activity from other processes, so filter carefully and state those limits. Do not use timings gathered during profiling as the normal benchmark timings.

WPR traces can include system-wide process information. ETL files in this folder are ignored by Git; review them before sharing. The A recording containing the benchmark is `a-cache-run2.etl`; the earlier `a-cache.etl` was stopped before the benchmark ran.

Reference: [Microsoft: Recording hardware performance events](https://learn.microsoft.com/en-us/windows-hardware/test/wpt/recording-pmu-events).
