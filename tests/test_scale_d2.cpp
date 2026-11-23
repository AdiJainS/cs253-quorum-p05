
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

#include "oracle.hpp"
#include "../src/structB/fenwick_max.hpp"

using namespace std;

int main() {
    const int n = 1000000;
    const int totalOps = 1000000;
    const unsigned long long seed = 42;

    mt19937_64 rng(seed);

    vector<long long> values(n);

    for (int i = 0; i < n; i++) {
        values[i] = rng() % 1000000;
    }

    Oracle oracle(values);
    FenwickMax tree(values);

    int updates = 0;
    int queries = 0;
    int decreases = 0;

    for (int i = 0; i < totalOps; i++) {
        int index = rng() % n;

        if (i % 2 == 0) {
            long long value = rng() % 1000000;

            if (value < values[index]) {
                decreases++;
            }

            values[index] = value;

            oracle.assign(index, value);
            tree.assign(index, value);

            updates++;
        }
        else {
            int left = index;
            int right = min(n - 1, left + 63);

            if (i % 100001 == 1) {
                left = 0;
                right = n - 1;
            }

            long long expected =
                oracle.queryMax(left, right).maximum;

            long long actual =
                tree.queryMax(left, right);

            queries++;

            if (expected != actual) {
                cout << "FAIL at operation " << i << "\n";
                cout << "Seed: " << seed << "\n";
                cout << "Range: " << left << " " << right << "\n";
                cout << "Expected: " << expected << "\n";
                cout << "Actual: " << actual << "\n";

                return 1;
            }
        }
    }

    cout << "Seed: " << seed << "\n";
    cout << "Array size: " << n << "\n";
    cout << "Total operations: " << totalOps << "\n";
    cout << "Updates: " << updates << "\n";
    cout << "Decreases: " << decreases << "\n";
    cout << "Queries: " << queries << "\n";
    cout << "Mismatches: 0\n";

    FenwickMax fresh(values);

    if (!tree.sameBlocksAs(fresh)) {
        cout << "Internal block check: FAIL\n";
        return 1;
    }

    cout << "Internal block check: PASS\n";
    cout << "Full-scale D2 testing: PASS\n";

    return 0;
}