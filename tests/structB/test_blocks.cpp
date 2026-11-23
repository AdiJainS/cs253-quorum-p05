#include <cstdlib>
#include <iostream>
#include <vector>

#include "fenwick_max.hpp"

using namespace std;

int checks = 0;
int mismatches = 0;

void checkAfterAssign(const char* test, int size, int index, long long value,
                      const FenwickMax& fast, const FenwickMax& slow,
                      const vector<long long>& values) {
    FenwickMax fresh(values);

    checks++;
    if (!fast.sameBlocksAs(fresh) || !slow.sameBlocksAs(fresh)) {
        mismatches++;
        if (mismatches <= 5) {
            cout << "MISMATCH " << test
                 << " size=" << size
                 << " index=" << index
                 << " value=" << value
                 << " fastOk=" << fast.sameBlocksAs(fresh)
                 << " slowOk=" << slow.sameBlocksAs(fresh) << "\n";
        }
    }
}

void testSmallArrays() {
    for (int size = 1; size <= 70; size++) {
        for (int round = 0; round < 20; round++) {
            vector<long long> values;
            for (int i = 0; i < size; i++) {
                values.push_back(rand() % 12 - 4);
            }

            FenwickMax fast(values);
            FenwickMax slow(values);

            for (int step = 0; step < 200; step++) {
                int index = rand() % size;
                long long value = rand() % 12 - 4;

                values[index] = value;
                fast.assign(index, value);
                slow.assignSlow(index, value);

                checkAfterAssign("small", size, index, value, fast, slow, values);
            }
        }
    }
}

void testOnlyDecreases() {
    int sizes[] = {64, 100, 255, 256, 1000};

    for (int s = 0; s < 5; s++) {
        int size = sizes[s];

        vector<long long> values;
        for (int i = 0; i < size; i++) {
            values.push_back(1000000 + rand() % 1000);
        }

        FenwickMax fast(values);
        FenwickMax slow(values);

        for (int step = 0; step < 500; step++) {
            int index = rand() % size;
            values[index] = values[index] - 1 - rand() % 10;

            fast.assign(index, values[index]);
            slow.assignSlow(index, values[index]);

            checkAfterAssign("decreases", size, index, values[index],
                             fast, slow, values);
        }
    }
}

void testEveryPosition() {
    for (int size = 1; size <= 40; size++) {
        vector<long long> values;
        for (int i = 0; i < size; i++) {
            values.push_back(i);
        }

        for (int index = 0; index < size; index++) {
            FenwickMax fast(values);
            FenwickMax slow(values);

            vector<long long> changed = values;
            changed[index] = -1000;

            fast.assign(index, -1000);
            slow.assignSlow(index, -1000);

            checkAfterAssign("every position", size, index, -1000,
                             fast, slow, changed);
        }
    }
}

int main() {
    srand(12345);

    testSmallArrays();
    testOnlyDecreases();
    testEveryPosition();

    cout << checks << " checks, " << mismatches << " mismatches\n";

    if (mismatches == 0) {
        cout << "PASS\n";
        return 0;
    }

    cout << "FAIL\n";
    return 1;
}
