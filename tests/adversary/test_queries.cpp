#include <iostream>

#include "generators.hpp"
#include "../../src/structB/fenwick_max.hpp"

using namespace std;

int main() {
    for (int n = 4; n <= 256; n = n * 2) {
        Workload workload = makeQueryWorkload(n, 1);
        FenwickMax tree(workload.initial);

        // Check our chosen range.
        tree.resetCounters();

        long long answer = tree.queryMax(0, n - 2);
        long long chosenSteps = tree.queryBlocks;

        if (answer != n - 1) {
            cout << "FAIL: wrong answer for chosen range\n";
            return 1;
        }

        long long maximumSteps = 0;
        int hardestLeft = 0;
        int hardestRight = 0;

        // Try every possible range.
        for (int left = 0; left < n; left++) {
            for (int right = left; right < n; right++) {
                tree.resetCounters();

                answer = tree.queryMax(left, right);

                // Array values are [1, 2, ..., n].
                if (answer != right + 1) {
                    cout << "FAIL: wrong range answer\n";
                    return 1;
                }

                if (tree.queryBlocks > maximumSteps) {
                    maximumSteps = tree.queryBlocks;
                    hardestLeft = left;
                    hardestRight = right;
                }
            }
        }

        cout << "n=" << n
             << ", chosen steps=" << chosenSteps
             << ", maximum steps=" << maximumSteps
             << ", hardest range=[" << hardestLeft
             << ", " << hardestRight << "]\n";

        if (chosenSteps != maximumSteps) {
            cout << "FAIL: chosen range is not a hardest range\n";
            return 1;
        }
    }

    cout << "PASS: chosen queries reach the tested maximum\n";
    return 0;
}