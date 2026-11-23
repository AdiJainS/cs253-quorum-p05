#include <iostream>

#include "generators.hpp"
#include "../src/structB/fenwick_max.hpp"

using namespace std;

int main() {
    int n = 1048576;
    int totalOperations = 1000000;

    // 1 = difficult queries
    // 2 = repeated decreases
    int test = 2;

    if (test != 1 && test != 2) {
        cout << "Choose test 1 or 2\n";
        return 1;
    }

    Workload workload;

    if (test == 1) {
        cout << "Workload: difficult queries\n";
        workload = makeQueryWorkload(n, totalOperations);
    }
    else {
        cout << "Workload: repeated decreases\n";
        workload = makeDecreaseWorkload(n, totalOperations / 2);
    }

    FenwickMax tree(workload.initial);
    tree.resetCounters();

    int assignments = 0;
    int queries = 0;

    long long expected = n - 1;

    for (Operation op : workload.operations) {
        if (op.type == OpType::ASSIGN) {
            tree.assign(op.l, op.value);

            // Index 0 remains the unique maximum.
            expected = op.value;
            assignments++;
        }
        else if (op.type == OpType::QUERY_MAX) {
            long long answer = tree.queryMax(op.l, op.r);

            if (answer != expected) {
                cout << "FAIL: wrong answer\n";
                cout << "Expected: " << expected << '\n';
                cout << "Received: " << answer << '\n';
                return 1;
            }

            queries++;
        }
        else {
            cout << "FAIL: unexpected operation\n";
            return 1;
        }
    }

    cout << "Elements: " << n << '\n';
    cout << "Operations: " << assignments + queries << '\n';
    cout << "Assignments: " << assignments << '\n';
    cout << "Queries checked: " << queries << '\n';
    cout << "Query steps: " << tree.queryBlocks << '\n';
    cout << "Assignment steps: " << tree.assignBlocks << '\n';
    cout << "Lowered blocks: " << tree.loweredBlocks << '\n';

    // Expected counts for the fixed sizes used above.
    bool countsCorrect;

    if (test == 1) {
        countsCorrect =
            assignments == 0 &&
            queries == 1000000 &&
            tree.queryBlocks == 39000000 &&
            tree.assignBlocks == 0 &&
            tree.loweredBlocks == 0;
    }
    else {
        countsCorrect =
            assignments == 500000 &&
            queries == 500000 &&
            tree.queryBlocks == 10500000 &&
            tree.assignBlocks == 21000000 &&
            tree.loweredBlocks == 11000000;
    }

    if (!countsCorrect) {
        cout << "FAIL: unexpected counts\n";
        return 1;
    }

    cout << "PASS: all answers and counter totals are correct\n";
    return 0;
}