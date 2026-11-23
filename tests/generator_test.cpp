
#include <cassert>
#include <iostream>

#include "generator.hpp"

bool sameOperation(const Operation& a, const Operation& b) {
    return a.type == b.type &&
           a.l == b.l &&
           a.r == b.r &&
           a.value == b.value;
}

int main() {
    const int n = 100;
    const int totalOps = 10000;
    const unsigned long long seed = 42;

    auto operations = generateOperations(n, totalOps, seed);
    auto repeated = generateOperations(n, totalOps, seed);

    assert(operations.size() == totalOps);
    assert(repeated.size() == totalOps);

    for (int i = 0; i < totalOps; i++) {
        assert(sameOperation(operations[i], repeated[i]));

        const auto& op = operations[i];

        if (op.type == OpType::ASSIGN) {
            assert(op.l >= 0 && op.l < n);
        }
        else {
            assert(op.l >= 0);
            assert(op.l <= op.r);
            assert(op.r < n);
        }
    }

    std::cout << "Generator tests passed!\n";
}
