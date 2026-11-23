
#include <iostream>
#include <vector>
#include <numeric>
#include <fstream>

#include "gcd_reference.hpp"
#include "generator.hpp"

using namespace std;

class BuggyTree {
private:
    vector<Value> arr;

public:
    BuggyTree(vector<Value> a) {
        arr = a;
    }

    void assign(int index, Value value) {
        (void)index;
        (void)value;
    }

    Value query(int l, int r) {
        Value answer = 0;

        for (int i = l; i < r; i++) {
            answer = gcd(answer, arr[i]);
        }

        return answer;
    }
};

void saveFailure(
    const vector<Value>& arr,
    const vector<Operation>& operations,
    int failedAt,
    unsigned long long seed
) {
    ofstream file("failure.txt");

    if (!file) {
        cerr << "Could not create failure.txt\n";
        return;
    }

    file << "Seed: " << seed << "\n";
    file << "Array size: " << arr.size() << "\n";

    file << "Initial array: ";
    for (Value value : arr) {
        file << value << " ";
    }
    file << "\n";

    file << "Operations:\n";

    for (int i = 0; i <= failedAt; i++) {
        Operation op = operations[i];

        if (op.type == OpType::ASSIGN) {
            file << "ASSIGN " << op.l
                 << " " << op.value << "\n";
        }
        else if (op.type == OpType::QUERY_GCD) {
            file << "QUERY_GCD " << op.l
                 << " " << op.r << "\n";
        }
        else {
            file << "QUERY_MAX " << op.l
                 << " " << op.r << "\n";
        }
    }
}

int main() {
    int n = 100;
    int totalOps = 10000;
    unsigned long long seed = 42;

    vector<Value> arr(n, 10);

    GcdReference oracle(arr);
    BuggyTree tree(arr);

    vector<Operation> operations =
        generateOperations(n, totalOps, seed);

    int queriesChecked = 0;
    int failedAt = -1;

    for (int i = 0; i < totalOps; i++) {
        Operation op = operations[i];

        if (op.type == OpType::ASSIGN) {
            oracle.assign(op.l, op.value);
            tree.assign(op.l, op.value);
        }
        else if (op.type == OpType::QUERY_GCD) {
            Value expected = oracle.query(op.l, op.r + 1);
            Value actual = tree.query(op.l, op.r + 1);

            queriesChecked++;

            if (expected != actual) {
                failedAt = i;

                cout << "Mismatch at operation: "
                     << i << "\n";
                cout << "Expected: " << expected << "\n";
                cout << "Actual: " << actual << "\n";

                saveFailure(arr, operations, i, seed);
                break;
            }
        }
    }

    cout << "Seed: " << seed << "\n";
    cout << "GCD queries checked: " << queriesChecked << "\n";

    if (failedAt != -1) {
        cout << "Failure saved to failure.txt\n";
        cout << "SUCCESS: Bug detected!\n";
    }
    else {
        cout << "No mismatches found.\n";
    }

    return failedAt != -1 ? 0 : 1;
}
