#include <iostream>
#include <random>
#include <vector>

#include "../../src/structA/structure_a.hpp"
#include "../../oracle/oracle.hpp"
#include "../../oracle/generator.hpp"

int main() {
    const int n = 300;
    const int operationsCount = 1000000;
    const unsigned long long seed = 20261116ULL;

    std::mt19937_64 rng(seed);
    std::uniform_int_distribution<long long> initialDist(-100000, 100000);

    std::vector<long long> values(n);
    for (int i = 0; i < n; i++) {
        values[i] = initialDist(rng);
    }

    structA::StructureA tree(values);
    Oracle oracle(values);

    std::vector<Operation> operations = generateOperations(n, operationsCount, seed);

    unsigned long long maxQueries = 0;
    unsigned long long gcdQueries = 0;

    for (int step = 0; step < (int)operations.size(); step++) {
        Operation op = operations[step];

        if (op.type == OpType::ASSIGN) {
            values[op.l] = op.value;
            tree.assign(op.l, op.value);
            oracle.assign(op.l, op.value);
        }
        else if (op.type == OpType::QUERY_MAX) {
            MaxResult expected = oracle.queryMax(op.l, op.r);
            structA::MaxCountNode actual = tree.queryMax(op.l, op.r + 1);
            maxQueries++;

            if (actual.maximum != expected.maximum ||
                actual.count != expected.count) {
                std::cout << "Maximum mismatch at operation " << step
                          << ", range [" << op.l << "," << op.r << "]\n";
                return 1;
            }
        }
        else if (op.type == OpType::QUERY_GCD) {
            long long expected = oracle.queryGCD(op.l, op.r);
            structA::GcdValue actual = tree.queryGCD(op.l, op.r + 1);
            gcdQueries++;

            if (actual != (structA::GcdValue)expected) {
                std::cout << "GCD mismatch at operation " << step
                          << ", range [" << op.l << "," << op.r << "]\n";
                return 1;
            }
        }
    }

    if (!tree.debugCheckAgainst(values)) {
        std::cout << "Final invariant check failed.\n";
        return 1;
    }

    std::cout << "Differential test passed. seed=" << seed
              << " operations=" << operationsCount
              << " max_queries=" << maxQueries
              << " gcd_queries=" << gcdQueries << "\n";
    return 0;
}
