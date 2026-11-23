
#include <iostream>
#include <vector>
#include <fstream>
#include "oracle.hpp"
#include "operations.hpp"
#include "../src/structB/fenwick_max.hpp"

using namespace std;

bool hasFailure(const vector<long long>& initial,
                const vector<Operation>& ops,
                bool injectFault) {
    Oracle oracle(initial);
    FenwickMax tree(initial);

    for (int i = 0; i < (int)ops.size(); i++) {
        Operation op = ops[i];

        if (op.type == OpType::ASSIGN) {
            oracle.assign(op.l, op.value);

            if (!injectFault) {
                tree.assign(op.l, op.value);
            }
        }
        else if (op.type == OpType::QUERY_MAX) {
            long long expected =
                oracle.queryMax(op.l, op.r).maximum;

            long long actual =
                tree.queryMax(op.l, op.r);

            if (expected != actual) {
                return true;
            }
        }
    }

    return false;
}

int main() {
    vector<long long> initial = {5, 10, 7, 3, 8};

    vector<Operation> ops = {
        {OpType::QUERY_MAX, 0, 2, 0},
        {OpType::ASSIGN, 4, 0, 6},
        {OpType::ASSIGN, 1, 0, 2},
        {OpType::QUERY_MAX, 0, 2, 0},
        {OpType::QUERY_MAX, 3, 4, 0}
    };

    if (hasFailure(initial, ops, false)) {
        cout << "Actual Fenwick test: FAIL\n";
        return 1;
    }

    cout << "Actual Fenwick test: PASS\n";

    if (!hasFailure(initial, ops, true)) {
        cout << "Injected bug was not detected\n";
        return 1;
    }

    cout << "Injected bug detected\n";

    int before = (int)ops.size();
    int i = 0;

    while (i < (int)ops.size()) {
        vector<Operation> smaller = ops;
        smaller.erase(smaller.begin() + i);

        if (hasFailure(initial, smaller, true)) {
            ops = smaller;
            i = 0;
        }
        else {
            i++;
        }
    }

    ofstream file("minimized_d2_failure.txt");

    file << "Initial array:";

    for (long long value : initial) {
        file << " " << value;
    }

    file << "\n";

    for (Operation op : ops) {
        if (op.type == OpType::ASSIGN) {
            file << "ASSIGN " << op.l
                 << " " << op.value << "\n";
        }
        else if (op.type == OpType::QUERY_MAX) {
            file << "QUERY_MAX " << op.l
                 << " " << op.r << "\n";
        }
    }

    file.close();

    cout << "Original operations: " << before << "\n";
    cout << "Remaining operations: "
         << ops.size() << "\n";
    cout << "Saved to minimized_d2_failure.txt\n";

    return 0;
}
