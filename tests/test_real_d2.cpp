
#include <iostream>
#include <vector>
#include "oracle.hpp"
#include "../src/structB/fenwick_max.hpp"
#include "../adversary/generators.hpp"

using namespace std;

bool runTest(const Workload& workload, const string& name) {
    vector<long long> values = workload.initial;
    Oracle oracle(values);
    FenwickMax tree(values);

    int queries = 0;
    int updates = 0;

    for (int i = 0; i < (int)workload.operations.size(); i++) {
        Operation op = workload.operations[i];

        if (op.type == OpType::ASSIGN) {
            values[op.l] = op.value;
            oracle.assign(op.l, op.value);
            tree.assign(op.l, op.value);
            updates++;

            FenwickMax fresh(values);

            if (!tree.sameBlocksAs(fresh)) {
                cout << name << ": invariant failed at operation "
                     << i << "\n";
                return false;
            }
        }
        else if (op.type == OpType::QUERY_MAX) {
            long long expected =
                oracle.queryMax(op.l, op.r).maximum;

            long long actual =
                tree.queryMax(op.l, op.r);

            queries++;

            if (expected != actual) {
                cout << name << ": query failed at operation "
                     << i << "\n";
                cout << "Expected: " << expected
                     << ", Actual: " << actual << "\n";
                return false;
            }
        }
    }

    cout << name << ": PASS, " << updates
         << " updates, " << queries << " queries\n";

    return true;
}

int main() {
    bool ok = true;

    if (!runTest(makeRebuildWorkload(64, 32), "Rebuild"))
        ok = false;

    if (!runTest(makeQueryWorkload(64, 100), "Queries"))
        ok = false;

    if (!runTest(makeDecreaseWorkload(64, 32), "Decreases"))
        ok = false;

    return ok ? 0 : 1;
}
