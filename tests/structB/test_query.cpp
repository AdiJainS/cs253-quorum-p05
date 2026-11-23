#include <cstdlib>
#include <iostream>
#include <vector>

#include "fenwick_max.hpp"
#include "oracle.hpp"

using namespace std;

int main() {
    srand(12345);

    int checks = 0;
    int mismatches = 0;

    for (int size = 1; size <= 70; size++) {
        for (int round = 0; round < 5; round++) {
            vector<long long> values;
            for (int i = 0; i < size; i++) {
                values.push_back(rand() % 20 - 5);
            }

            FenwickMax b(values);
            Oracle oracle(values);

            for (int l = 0; l < size; l++) {
                for (int r = l; r < size; r++) {
                    long long expected = oracle.queryMax(l, r).maximum;
                    long long got = b.queryMax(l, r);
                    checks++;

                    if (expected != got) {
                        mismatches++;
                        if (mismatches <= 5) {
                            cout << "MISMATCH size=" << size
                                 << " l=" << l << " r=" << r
                                 << " expected=" << expected
                                 << " got=" << got << "\n";
                        }
                    }
                }
            }
        }
    }

    int bigSize = 100000;
    vector<long long> big;
    for (int i = 0; i < bigSize; i++) {
        big.push_back(rand() % 1000000);
    }

    FenwickMax bigB(big);
    Oracle bigOracle(big);

    for (int q = 0; q < 2000; q++) {
        int l = rand() % bigSize;
        int r = rand() % bigSize;
        if (l > r) {
            int temp = l;
            l = r;
            r = temp;
        }

        long long expected = bigOracle.queryMax(l, r).maximum;
        long long got = bigB.queryMax(l, r);
        checks++;

        if (expected != got) {
            mismatches++;
            if (mismatches <= 5) {
                cout << "MISMATCH big l=" << l << " r=" << r
                     << " expected=" << expected
                     << " got=" << got << "\n";
            }
        }
    }

    cout << checks << " checks, " << mismatches << " mismatches\n";

    if (mismatches == 0) {
        cout << "PASS\n";
        return 0;
    }

    cout << "FAIL\n";
    return 1;
}
