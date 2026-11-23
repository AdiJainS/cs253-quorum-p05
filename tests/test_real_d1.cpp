#include <iostream>
#include <vector>
#include <random>
#include "oracle.hpp"
#include "../src/structA/structure_a.hpp"
using namespace std;
int main() {
    const int n = 300;
    const int totalOps = 100000;
    const unsigned long long seed = 42;
    mt19937_64 rng(seed);
    vector<long long> values(n);
    for (int i = 0; i < n; i++) values[i] = (long long)(rng() % 100001) - 50000;
    Oracle oracle(values);
    structA::StructureA tree(values);
    int updates = 0, maxQueries = 0, gcdQueries = 0;
    for (int step = 0; step < totalOps; step++) {
        int type = (int)(rng() % 3);
        int l = (int)(rng() % n);
        int r = (int)(rng() % n);
        if (l > r) swap(l, r);
        if (type == 0) {
            long long value = (long long)(rng() % 100001) - 50000;
            oracle.assign(l, value);
            tree.assign(l, value);
            values[l] = value;
            updates++;
        } else if (type == 1) {
            auto expected = oracle.queryMax(l, r);
            auto actual = tree.queryMax(l, r + 1);
            maxQueries++;
            if (expected.maximum != actual.maximum || expected.count != actual.count) {
                cout << "Maximum mismatch at operation " << step << "\n";
                return 1;
            }
        } else {
            auto expected = oracle.queryGCD(l, r);
            auto actual = tree.queryGCD(l, r + 1);
            gcdQueries++;
            if ((unsigned long long)expected != actual) {
                cout << "GCD mismatch at operation " << step << "\n";
                return 1;
            }
        }
    }
    cout << "Seed: " << seed << "\n";
    cout << "Operations: " << totalOps << "\n";
    cout << "Assignments: " << updates << "\n";
    cout << "Max queries: " << maxQueries << "\n";
    cout << "GCD queries: " << gcdQueries << "\n";
    if (!tree.debugCheckAgainst(values)) {
        cout << "Invariant check: FAIL\n";
        return 1;
    }
    cout << "Invariant check: PASS\n";
    cout << "D1 differential test: PASS\n";
    return 0;
}
