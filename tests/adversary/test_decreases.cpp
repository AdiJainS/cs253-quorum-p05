#include <iostream>
#include <vector>

#include "generators.hpp"
#include "../../src/structB/fenwick_max.hpp"

using namespace std;

int main() {
    for (int n = 4; n <= 256; n = n * 2) {
        Workload workload = makeDecreaseWorkload(n, 10);
        FenwickMax tree(workload.initial);

        long long chosenSteps = 0;
        long long chosenLowered = 0;
        long long expected = workload.initial[0];

        bool firstUpdate = true;

        // Check repeated decreases at index 0.
        for (int i = 0; i < workload.operations.size(); i++) {
            Operation op = workload.operations[i];

            if (op.type == OpType::ASSIGN) {
                tree.resetCounters();
                tree.assign(op.l, op.value);

                expected = op.value;

                if (firstUpdate) {
                    chosenSteps = tree.assignBlocks;
                    chosenLowered = tree.loweredBlocks;
                    firstUpdate = false;
                }

                if (tree.assignBlocks != chosenSteps ||
                    tree.loweredBlocks != chosenLowered) {
                    cout << "FAIL: repeated update counts changed\n";
                    return 1;
                }
            }
            else if (op.type == OpType::QUERY_MAX) {
                long long answer = tree.queryMax(op.l, op.r);

                if (answer != expected) {
                    cout << "FAIL: wrong answer after decrease\n";
                    return 1;
                }
            }
        }

        long long maximumSteps = 0;
        long long maximumLowered = 0;

        // Test every possible update position.
        for (int position = 0; position < n; position++) {
            vector<long long> values(n, 0);
            values[position] = 2;

            FenwickMax otherTree(values);
            otherTree.assign(position, 1);

            if (otherTree.assignBlocks > maximumSteps) {
                maximumSteps = otherTree.assignBlocks;
            }

            if (otherTree.loweredBlocks > maximumLowered) {
                maximumLowered = otherTree.loweredBlocks;
            }

            if (otherTree.queryMax(0, n - 1) != 1) {
                cout << "FAIL: wrong answer in position check\n";
                return 1;
            }
        }

        cout << "n=" << n
             << ", steps=" << chosenSteps
             << ", maximum steps=" << maximumSteps
             << ", lowered=" << chosenLowered
             << ", maximum lowered=" << maximumLowered << '\n';

        if (chosenSteps != maximumSteps ||
            chosenLowered != maximumLowered) {
            cout << "FAIL: index 0 did not reach both maxima\n";
            return 1;
        }
    }

    cout << "PASS: decreases reach both tested maxima\n";
    return 0;
}