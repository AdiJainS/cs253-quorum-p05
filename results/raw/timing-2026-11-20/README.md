# 20 November timing runs

CPU: Intel Core i9-13900HX. Windows 11, same machine as the earlier experiments. Compiler: Zig 0.13.0, Clang 18.1.6, C++17, -O2.

D1 headers match `01da510`; D2 is `e925e6f` plus the D6 compile-time counter switch. D4 oracle headers are unchanged from `14fc835`. D5 uses the existing generator snapshot associated with `5085c20`, with comments removed. The team must still agree on its release revision.

Timing build: `zig c++ -std=c++17 -O2 -DSTRUCTB_DISABLE_COUNTERS bench/max_benchmark.cpp -o .bench-deps/max_timing_d6.exe`.
Counter build: `zig c++ -std=c++17 -O2 -DSTRUCTA_ENABLE_COUNTERS bench/max_benchmark.cpp -o .bench-deps/max_counts_d6.exe`.

`max-*.csv` contains nine random configurations; `decreases/` contains the 27-configuration decrease study plus small prefix cases. `d5-*-time.csv` contains the query/decrease scale cases. Each has five repetitions. Summaries give mean and sample standard deviation; build and replay are separate. `d5-*-check.txt` records the expected-answer checks; `checks.txt` records checksum and repetition validation. Instrumented D5 counters are separate from timing.

Commands:

```powershell
python bench/run_matrix.py .bench-deps/max_timing_d6.exe NEW_OUTPUT
python bench/run_decrease_study.py .bench-deps/max_timing_d6.exe NEW_DECREASE_OUTPUT
.\.bench-deps\max_timing_d6.exe 1048576 1000000 1 0 0 5 query check-scale
.\.bench-deps\max_timing_d6.exe 1048576 1000000 1 0 0 5 query time
.\.bench-deps\max_timing_d6.exe 1048576 500000 1 0 0 5 decrease check-scale
.\.bench-deps\max_timing_d6.exe 1048576 500000 1 0 0 5 decrease time
```

This was an interactive Windows session. Background load and thermal state were not controlled. Some configurations have large spread; retain the spread when comparing. No fresh WPR recording was collected for this build. The older traces must not be labeled as counters-off measurements.

Source SHA-256 values:

- `bench/max_benchmark.cpp`: `37c97f01502a14863543cfd17fecdfbedfcf5a7a849d83a1838f89fa9df12f9c`
- `src/structA/segment_tree.hpp`: `c26c788d5b37ee9c0a6cc78ef14f1c7758da2550e866cb65043e18eb976e55d8`
- `src/structB/fenwick_max.hpp`: `44ace408f5f4bc6e3a1feeeba98bd79f722c921622abbc18feb61640ab585774`
- `adversary/generators.hpp`: `78daa52d90e3be7a8d602f4d2a7004149aa36f27c9c9f4c761ca9290b6817e8a`
- `oracle/oracle.hpp`: `73d2337a2991e2081063e4651ba5d341bccdfc0ab58d03865cfb3fa2f18ffb12`
