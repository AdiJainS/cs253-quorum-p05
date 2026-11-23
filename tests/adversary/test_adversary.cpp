#include <iostream>
#include <vector>

#include "generators.hpp"
#include "../../src/structB/fenwick_max.hpp"
#include "../../src/structB/prefix_fenwick_max.hpp"

using namespace std;

int main() {
    int n = 4;
    int k = 4;

    Workload workload = makeRebuildWorkload(n, k);

    vector<long long> values = workload.initial;

    PrefixFenwickMax baseline(values);
    PrefixFenwickMax increaseOnly(values);
    FenwickMax twoTree(values);

    int queries = 0;
    int wrongAnswers = 0;

    for (int i = 0; i < workload.operations.size(); i++) {
        Operation op = workload.operations[i];

        if (op.type == OpType::ASSIGN) {
            values[op.l] = op.value;

            baseline.assign(op.l, op.value);
            increaseOnly.assignIncreaseOnly(op.l, op.value);
            twoTree.assign(op.l, op.value);
        }
        else if (op.type == OpType::QUERY_MAX) {
            // Find the correct maximum by checking the array.
            long long expected = values[0];

            for (int j = 1; j < n; j++) {
                if (values[j] > expected) {
                    expected = values[j];
                }
            }

            long long answer1 = baseline.queryPrefix(op.r);
            long long answer2 = twoTree.queryMax(op.l, op.r);
            long long answer3 = increaseOnly.queryPrefix(op.r);

            cout << "Expected: " << expected
                 << ", baseline: " << answer1
                 << ", two-tree: " << answer2
                 << ", increase-only: " << answer3 << '\n';

            if (answer1 != expected || answer2 != expected) {
                cout << "FAIL: incorrect answer\n";
                return 1;
            }

            if (answer3 != expected) {
                wrongAnswers++;
            }

            queries++;
        }
    }

    cout << "Queries checked: " << queries << '\n';
    cout << "Baseline rebuilds: " << baseline.rebuildCount() << '\n';
    cout << "Increase-only wrong answers: " << wrongAnswers << '\n';

    cout << "Two-tree assignment steps: "
         << twoTree.assignBlocks << '\n';

    cout << "Two-tree lowered blocks: "
         << twoTree.loweredBlocks << '\n';

    cout << "Two-tree query steps: "
         << twoTree.queryBlocks << '\n';

    if (queries != k ||
        baseline.rebuildCount() != k ||
        wrongAnswers != k) {
        cout << "FAIL: unexpected counts\n";
        return 1;
    }

    cout << "PASS\n";
    return 0;
}