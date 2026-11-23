#!/bin/sh
set -eu
cd "$(dirname "$0")"
build_dir=$(mktemp -d "${TMPDIR:-/tmp}/sensor-checks.XXXXXX")
trap 'rm -rf "$build_dir"' EXIT HUP INT TERM
c++ -std=c++17 -O2 -Wall -Wextra -pedantic gcd_segment_tree.cpp -o "$build_dir/replay"
c++ -std=c++17 -O2 -Wall -Wextra -pedantic test_shared_interface.cpp -o "$build_dir/test-interface"
"$build_dir/test-interface"
for backend in a b-hybrid; do
    "$build_dir/replay" "--backend=$backend" < sample.in > "$build_dir/actual.out"
    diff -u sample.out "$build_dir/actual.out"
    "$build_dir/replay" "--backend=$backend" --test
    "$build_dir/replay" "--backend=$backend" --scale
done
