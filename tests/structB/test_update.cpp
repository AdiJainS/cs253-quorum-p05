#include <cstdlib>
#include <iostream>
#include <vector>

#include "fenwick_max.hpp"
#include "generator.hpp"
#include "oracle.hpp"

using namespace std;

int checks = 0;
int mismatches = 0;

void compare(const char* test, int l, int r, long long expected, long long got) {
    checks++;

    if (expected != got) {
        mismatches++;
        if (mismatches <= 5) {
            cout << "MISMATCH " << test
                 << " l=" << l << " r=" << r
                 << " expected=" << expected
                 << " got=" << got << "\n";
        }
    }
}

void testWithGenerator() {
    int sizes[] = {1, 2, 3, 5, 8, 13, 50, 1000};

    for (int s = 0; s < 8; s++) {
        int size = sizes[s];

        for (int seed = 1; seed <= 5; seed++) {
            vector<long long> values;
            for (int i = 0; i < size; i++) {
                values.push_back(rand() % 1000001);
            }

            FenwickMax b(values);
            Oracle oracle(values);

            vector<Operation> operations = generateOperations(size, 20000, seed);

            for (int k = 0; k < (int)operations.size(); k++) {
                Operation op = operations[k];

                if (op.type == OpType::ASSIGN) {
                    b.assign(op.l, op.value);
                    oracle.assign(op.l, op.value);
                }
                else if (op.type == OpType::QUERY_MAX) {
                    long long expected = oracle.queryMax(op.l, op.r).maximum;
                    long long got = b.queryMax(op.l, op.r);
                    compare("generator", op.l, op.r, expected, got);
                }
            }
        }
    }
}

void testManyEqualValues() {
    for (int size = 1; size <= 40; size++) {
        for (int round = 0; round < 20; round++) {
            vector<long long> values;
            for (int i = 0; i < size; i++) {
                values.push_back(rand() % 5);
            }

            FenwickMax b(values);
            Oracle oracle(values);

            for (int step = 0; step < 300; step++) {
                int index = rand() % size;
                long long value = rand() % 5;
                b.assign(index, value);
                oracle.assign(index, value);

                int l = rand() % size;
                int r = rand() % size;
                if (l > r) {
                    int temp = l;
                    l = r;
                    r = temp;
                }

                compare("equal values", l, r,
                        oracle.queryMax(l, r).maximum, b.queryMax(l, r));
                compare("equal values full", 0, size - 1,
                        oracle.queryMax(0, size - 1).maximum,
                        b.queryMax(0, size - 1));
            }
        }
    }
}

void testOnlyDecreases() {
    int size = 1000;

    vector<long long> values;
    for (int i = 0; i < size; i++) {
        values.push_back(1000000 + rand() % 1000);
    }

    FenwickMax b(values);
    Oracle oracle(values);

    for (int step = 0; step < 20000; step++) {
        int index = rand() % size;
        values[index] = values[index] - 1 - rand() % 10;

        b.assign(index, values[index]);
        oracle.assign(index, values[index]);

        int l = rand() % size;
        int r = rand() % size;
        if (l > r) {
            int temp = l;
            l = r;
            r = temp;
        }

        compare("only decreases", l, r,
                oracle.queryMax(l, r).maximum, b.queryMax(l, r));
        compare("only decreases full", 0, size - 1,
                oracle.queryMax(0, size - 1).maximum,
                b.queryMax(0, size - 1));
    }
}

int main() {
    srand(12345);

    testWithGenerator();
    testManyEqualValues();
    testOnlyDecreases();

    cout << checks << " checks, " << mismatches << " mismatches\n";

    if (mismatches == 0) {
        cout << "PASS\n";
        return 0;
    }

    cout << "FAIL\n";
    return 1;
}
