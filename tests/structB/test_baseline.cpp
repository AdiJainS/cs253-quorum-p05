#include <cstdlib>
#include <iostream>
#include <vector>

#include "generator.hpp"
#include "oracle.hpp"
#include "prefix_fenwick_max.hpp"

using namespace std;

int checks = 0;
int mismatches = 0;

void compare(const char* test, int r, long long expected, long long got) {
    checks++;

    if (expected != got) {
        mismatches++;
        if (mismatches <= 5) {
            cout << "MISMATCH " << test
                 << " r=" << r
                 << " expected=" << expected
                 << " got=" << got << "\n";
        }
    }
}

void testWithGenerator() {
    int sizes[] = {1, 2, 3, 5, 8, 13, 50, 1000};
    int rebuildsTotal = 0;
    int assignsTotal = 0;

    for (int s = 0; s < 8; s++) {
        int size = sizes[s];

        for (int seed = 1; seed <= 5; seed++) {
            vector<long long> values;
            for (int i = 0; i < size; i++) {
                values.push_back(rand() % 1000001);
            }

            PrefixFenwickMax b(values);
            Oracle oracle(values);

            vector<Operation> operations = generateOperations(size, 20000, seed);

            for (int k = 0; k < (int)operations.size(); k++) {
                Operation op = operations[k];

                if (op.type == OpType::ASSIGN) {
                    b.assign(op.l, op.value);
                    oracle.assign(op.l, op.value);
                    assignsTotal++;
                }
                else if (op.type == OpType::QUERY_MAX) {
                    long long expected = oracle.queryMax(0, op.r).maximum;
                    long long got = b.queryPrefix(op.r);
                    compare("generator", op.r, expected, got);
                }
            }

            rebuildsTotal = rebuildsTotal + b.rebuildCount();
        }
    }

    cout << "generator runs: " << assignsTotal << " assigns, "
         << rebuildsTotal << " of them needed a rebuild\n";
}

void testManyEqualValues() {
    for (int size = 1; size <= 40; size++) {
        for (int round = 0; round < 20; round++) {
            vector<long long> values;
            for (int i = 0; i < size; i++) {
                values.push_back(rand() % 5);
            }

            PrefixFenwickMax b(values);
            Oracle oracle(values);

            for (int step = 0; step < 300; step++) {
                int index = rand() % size;
                long long value = rand() % 5;
                b.assign(index, value);
                oracle.assign(index, value);

                int r = rand() % size;
                compare("equal values", r,
                        oracle.queryMax(0, r).maximum, b.queryPrefix(r));
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

    PrefixFenwickMax b(values);
    Oracle oracle(values);

    for (int step = 0; step < 5000; step++) {
        int index = rand() % size;
        values[index] = values[index] - 1 - rand() % 10;

        b.assign(index, values[index]);
        oracle.assign(index, values[index]);

        int r = rand() % size;
        compare("only decreases", r,
                oracle.queryMax(0, r).maximum, b.queryPrefix(r));
    }

    cout << "only decreases: " << b.rebuildCount() << " rebuilds for 5000 assigns\n";
}

int wrongWithDecreases() {
    int size = 1000;
    int wrong = 0;

    vector<long long> values;
    for (int i = 0; i < size; i++) {
        values.push_back(1000000 + rand() % 1000);
    }

    PrefixFenwickMax b(values);
    Oracle oracle(values);

    for (int step = 0; step < 5000; step++) {
        int index = rand() % size;
        values[index] = values[index] - 1 - rand() % 10;

        b.assignIncreaseOnly(index, values[index]);
        oracle.assign(index, values[index]);

        int r = rand() % size;
        if (oracle.queryMax(0, r).maximum != b.queryPrefix(r)) {
            wrong++;
        }
    }

    return wrong;
}

int wrongWithIncreases() {
    int size = 1000;
    int wrong = 0;

    vector<long long> values;
    for (int i = 0; i < size; i++) {
        values.push_back(rand() % 1000);
    }

    PrefixFenwickMax b(values);
    Oracle oracle(values);

    for (int step = 0; step < 5000; step++) {
        int index = rand() % size;
        values[index] = values[index] + 1 + rand() % 10;

        b.assignIncreaseOnly(index, values[index]);
        oracle.assign(index, values[index]);

        int r = rand() % size;
        if (oracle.queryMax(0, r).maximum != b.queryPrefix(r)) {
            wrong++;
        }
    }

    return wrong;
}

int main() {
    srand(12345);

    testWithGenerator();
    testManyEqualValues();
    testOnlyDecreases();

    int wrongDec = wrongWithDecreases();
    int wrongInc = wrongWithIncreases();

    cout << "increase-only rule, workload with decreases: "
         << wrongDec << " wrong answers out of 5000\n";
    cout << "increase-only rule, workload with only increases: "
         << wrongInc << " wrong answers out of 5000\n";

    cout << checks << " checks, " << mismatches << " mismatches\n";

    if (mismatches == 0 && wrongDec > 0 && wrongInc == 0) {
        cout << "PASS\n";
        return 0;
    }

    cout << "FAIL\n";
    return 1;
}
