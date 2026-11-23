#include <cstdlib>
#include <iostream>
#include <vector>

#include "fenwick_max.hpp"
#include "prefix_fenwick_max.hpp"

using namespace std;

int checks = 0;
int failures = 0;

void check(const char* name, long long expected, long long got) {
    checks++;

    if (expected != got) {
        failures++;
        if (failures <= 10) {
            cout << "FAIL " << name
                 << " expected=" << expected
                 << " got=" << got << "\n";
        }
    }
}

void checkAtMost(const char* name, long long limit, long long got) {
    checks++;

    if (got > limit) {
        failures++;
        if (failures <= 10) {
            cout << "FAIL " << name
                 << " limit=" << limit
                 << " got=" << got << "\n";
        }
    }
}

int floorLog2(int x) {
    int k = 0;
    while (x > 1) {
        x = x / 2;
        k++;
    }
    return k;
}

int countOneBits(int x) {
    int count = 0;
    while (x > 0) {
        if (x % 2 == 1) {
            count++;
        }
        x = x / 2;
    }
    return count;
}

vector<long long> exampleArray() {
    vector<long long> values;
    values.push_back(3);
    values.push_back(2);
    values.push_back(5);
    values.push_back(1);
    values.push_back(4);
    values.push_back(6);
    values.push_back(2);
    values.push_back(7);
    return values;
}

void testExampleQuery() {
    FenwickMax b(exampleArray());

    check("example query answer", 6, b.queryMax(2, 6));
    check("example query count", 1, b.queries);
    check("example query blocks", 4, b.queryBlocks);

    b.resetCounters();
    check("single element answer", 1, b.queryMax(3, 3));
    check("single element blocks", 1, b.queryBlocks);
}

void testExampleAssign() {
    FenwickMax b(exampleArray());

    b.assign(5, 1);
    check("example assign count", 1, b.assigns);
    check("example assign changed", 3, b.changedBlocks);
    check("example assign lowered", 3, b.loweredBlocks);
    check("example answer after", 5, b.queryMax(2, 6));

    FenwickMax c(exampleArray());
    c.assign(5, 9);
    check("raise lowers nothing", 0, c.loweredBlocks);
}

void testQueryBlockLimit() {
    for (int size = 1; size <= 100; size++) {
        vector<long long> values;
        for (int i = 0; i < size; i++) {
            values.push_back(rand() % 50);
        }

        FenwickMax b(values);

        for (int l = 0; l < size; l++) {
            for (int r = l; r < size; r++) {
                b.resetCounters();
                b.queryMax(l, r);

                int width = r - l + 1;
                checkAtMost("query blocks, small", 2 * floorLog2(width) + 1,
                            b.queryBlocks);
            }
        }
    }

    int size = 1000000;
    vector<long long> values;
    for (int i = 0; i < size; i++) {
        values.push_back(rand() % 1000000);
    }

    FenwickMax big(values);
    long long worst = 0;

    for (int q = 0; q < 20000; q++) {
        int l = rand() % size;
        int r = rand() % size;
        if (l > r) {
            int temp = l;
            l = r;
            r = temp;
        }

        big.resetCounters();
        big.queryMax(l, r);

        int width = r - l + 1;
        checkAtMost("query blocks, big", 2 * floorLog2(width) + 1,
                    big.queryBlocks);

        if (big.queryBlocks > worst) {
            worst = big.queryBlocks;
        }
    }

    cout << "n=1000000: most blocks in one query = " << worst
         << " (limit 2*log2(n)+1 = " << 2 * floorLog2(size) + 1 << ")\n";
}

void testAssignBlockLimit() {
    int size = 1000000;
    vector<long long> values;
    for (int i = 0; i < size; i++) {
        values.push_back(rand() % 1000000);
    }

    FenwickMax b(values);
    long long worst = 0;

    for (int step = 0; step < 20000; step++) {
        int index = rand() % size;

        b.resetCounters();
        b.assign(index, rand() % 1000000);

        checkAtMost("assign blocks", 4 * (floorLog2(size) + 1), b.assignBlocks);

        if (b.assignBlocks > worst) {
            worst = b.assignBlocks;
        }
    }

    cout << "n=1000000: most steps in one assign = " << worst
         << " (limit 4*(log2(n)+1) = " << 4 * (floorLog2(size) + 1) << ")\n";
}

void testFastAndSlowCountSameChanges() {
    for (int size = 1; size <= 60; size++) {
        vector<long long> values;
        for (int i = 0; i < size; i++) {
            values.push_back(rand() % 10);
        }

        FenwickMax fast(values);
        FenwickMax slow(values);

        for (int step = 0; step < 200; step++) {
            int index = rand() % size;
            long long value = rand() % 10;

            fast.assign(index, value);
            slow.assignSlow(index, value);

            check("changed blocks", slow.changedBlocks, fast.changedBlocks);
            check("lowered blocks", slow.loweredBlocks, fast.loweredBlocks);
        }
    }
}

void testPrefixCounters() {
    PrefixFenwickMax b(exampleArray());

    for (int r = 0; r < 8; r++) {
        b.resetCounters();
        b.queryPrefix(r);
        check("prefix query blocks", countOneBits(r + 1), b.queryBlocks);
    }

    b.resetCounters();
    b.assign(0, 100);
    check("prefix raise steps", 4, b.raiseSteps);
    check("raise needs no rebuild", 0, b.rebuildCount());

    b.assign(0, 1);
    check("decrease rebuilds once", 1, b.rebuildCount());
}

int main() {
    srand(12345);

    testExampleQuery();
    testExampleAssign();
    testQueryBlockLimit();
    testAssignBlockLimit();
    testFastAndSlowCountSameChanges();
    testPrefixCounters();

    cout << checks << " checks, " << failures << " failures\n";

    if (failures == 0) {
        cout << "PASS\n";
        return 0;
    }

    cout << "FAIL\n";
    return 1;
}
