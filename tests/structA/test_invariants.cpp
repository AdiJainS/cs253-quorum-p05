#include <iostream>
#include <random>
#include <vector>

#include "../../src/structA/structure_a.hpp"

int main() {
    const int n = 4097;
    const int updates = 20000;
    std::mt19937_64 rng(253);
    std::uniform_int_distribution<long long> valueDist(-1000000, 1000000);
    std::uniform_int_distribution<int> indexDist(0, n - 1);

    std::vector<long long> values(n);
    for (int i = 0; i < n; i++) {
        values[i] = valueDist(rng);
    }

    structA::StructureA tree(values);

    if (!tree.debugCheckAgainst(values)) {
        std::cout << "Initial invariant check failed.\n";
        return 1;
    }

    for (int step = 0; step < updates; step++) {
        int index = indexDist(rng);
        long long value = valueDist(rng);
        values[index] = value;
        tree.assign(index, value);

        if (!tree.debugCheckAgainst(values)) {
            std::cout << "Invariant check failed at update " << step << "\n";
            return 1;
        }
    }

    std::cout << "Structure A invariant tests passed: "
              << updates << " updates.\n";
    return 0;
}
