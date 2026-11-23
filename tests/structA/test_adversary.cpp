#include <iostream>
#include <vector>

#include "../../src/structA/structure_a.hpp"
#include "../../oracle/oracle.hpp"
#include "../../adversary/generators.hpp"

bool replay(const Workload& workload) {
    structA::StructureA tree(workload.initial);
    Oracle oracle(workload.initial);
    std::vector<long long> current = workload.initial;

    for (int i = 0; i < (int)workload.operations.size(); i++) {
        Operation op = workload.operations[i];

        if (op.type == OpType::ASSIGN) {
            current[op.l] = op.value;
            tree.assign(op.l, op.value);
            oracle.assign(op.l, op.value);
        }
        else if (op.type == OpType::QUERY_MAX) {
            MaxResult expected = oracle.queryMax(op.l, op.r);
            structA::MaxCountNode actual = tree.queryMax(op.l, op.r + 1);
            if (actual.maximum != expected.maximum ||
                actual.count != expected.count) {
                return false;
            }
        }
        else if (op.type == OpType::QUERY_GCD) {
            long long expected = oracle.queryGCD(op.l, op.r);
            structA::GcdValue actual = tree.queryGCD(op.l, op.r + 1);
            if (actual != (structA::GcdValue)expected) {
                return false;
            }
        }
    }

    return tree.debugCheckAgainst(current);
}

int main() {
    if (!replay(makeRebuildWorkload(1024, 512))) {
        std::cout << "Rebuild workload failed.\n";
        return 1;
    }

    if (!replay(makeQueryWorkload(1024, 10000))) {
        std::cout << "Difficult-query workload failed.\n";
        return 1;
    }

    if (!replay(makeDecreaseWorkload(1024, 1000))) {
        std::cout << "Repeated-decrease workload failed.\n";
        return 1;
    }

    std::cout << "Structure A adversarial tests passed.\n";
    return 0;
}
