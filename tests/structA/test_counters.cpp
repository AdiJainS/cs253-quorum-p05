#ifndef STRUCTA_ENABLE_COUNTERS
#error "Compile this test with -DSTRUCTA_ENABLE_COUNTERS"
#endif

#include <iostream>
#include <random>
#include <vector>

#include "../../src/structA/structure_a.hpp"

int ceilLog2(int x) {
    int result = 0;
    int p = 1;
    while (p < x) {
        p = p * 2;
        result++;
    }
    return result;
}

int main() {
    const int n = 1000000;
    const int operations = 20000;
    std::mt19937_64 rng(12345);
    std::uniform_int_distribution<long long> valueDist(0, 1000000);
    std::uniform_int_distribution<int> indexDist(0, n - 1);

    std::vector<long long> values(n);
    for (int i = 0; i < n; i++) {
        values[i] = valueDist(rng);
    }

    structA::StructureA tree(values);
    tree.resetCounters();

    int maxAllowedBlocks = 2 * ceilLog2(tree.size()) + 1;
    int maxAllowedUpdateVisits = ceilLog2(tree.size()) + 1;

    unsigned long long worstQueryBlocks = 0;
    unsigned long long worstUpdateVisits = 0;

    for (int step = 0; step < operations; step++) {
        if (step % 2 == 0) {
            int l = indexDist(rng);
            int r = indexDist(rng);
            if (l > r) {
                int temp = l;
                l = r;
                r = temp;
            }
            r++;

            tree.resetCounters();
            tree.queryMax(l, r);
            structA::StructureACounters c = tree.counters();
            if (c.maximum.queryBlocks > worstQueryBlocks) {
                worstQueryBlocks = c.maximum.queryBlocks;
            }
            if ((int)c.maximum.queryBlocks > maxAllowedBlocks) {
                std::cout << "Query block bound exceeded.\n";
                return 1;
            }
        }
        else {
            int index = indexDist(rng);
            long long value = valueDist(rng);

            tree.resetCounters();
            tree.assign(index, value);
            structA::StructureACounters c = tree.counters();
            if (c.maximum.updateVisits > worstUpdateVisits) {
                worstUpdateVisits = c.maximum.updateVisits;
            }
            if ((int)c.maximum.updateVisits > maxAllowedUpdateVisits) {
                std::cout << "Update visit bound exceeded.\n";
                return 1;
            }
        }
    }

    std::cout << "n=" << n
              << " operations=" << operations
              << " worst_query_blocks=" << worstQueryBlocks
              << " query_limit=" << maxAllowedBlocks
              << " worst_update_visits=" << worstUpdateVisits
              << " update_limit=" << maxAllowedUpdateVisits
              << "\n";
    std::cout << "Counter tests passed.\n";
    return 0;
}
