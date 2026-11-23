#!/bin/sh
set -eu
cd "$(dirname "$0")/.."

make all
make test

./bin/app < app/sample.in

RAW=results/raw/timing-2026-11-20
FIG=results/figures/timing-2026-11-20
mkdir -p "$FIG/decreases"

if [ "${FULL:-0}" = "1" ]; then
    OUT=results/raw/rerun-$(date +%Y-%m-%d)
    mkdir -p "$OUT"
    python3 bench/run_matrix.py bin/max_benchmark "$OUT/matrix"
    python3 bench/run_decrease_study.py bin/max_benchmark "$OUT/decreases"
    ./bin/max_benchmark 1048576 1000000 1 0 0 5 query time > "$OUT/d5-query-time.csv"
    ./bin/max_benchmark 1048576 500000 1 0 0 5 decrease time > "$OUT/d5-decrease-time.csv"
    echo "Full rerun written to $OUT. Point RAW in this script at it to plot it."
fi

for u in 10 50 90; do
    python3 bench/plot_width.py "$RAW/max-summary.csv" "$FIG/max-width-u$u.png" \
        --n 1000000 --operations 1000000 --update-percent $u --seed 253 --workload random
done
python3 bench/plot_d5.py "$RAW/d5-summary.csv" "$FIG/d5-timings.png"
python3 bench/plot_decreases.py "$RAW/decreases/prefix-summary.csv" "$FIG/decreases/prefix-decreases.png" \
    --n 4096 --operations 10000 --width 4096 --update-percent 50 --seed 253 --log-y

python3 bench/plot_decreases.py "$RAW/decreases/summary.csv" "$FIG/decreases/width1024-updates50.png" \
    --n 1000000 --operations 1000000 --width 1024 --update-percent 50 --seed 253
python3 bench/plot_prediction.py "$RAW/decreases/prefix-summary.csv" "$FIG/decreases/prefix-prediction.png" \
    --n 4096 --operations 10000 --update-percent 50 --seed 253

echo "run_all finished"
