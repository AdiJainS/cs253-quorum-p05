#include <iostream>
#include <vector>

#include "../../src/structA/structure_a.hpp"
#include "../../oracle/oracle.hpp"

int main() {
    std::vector<long long> values = {12, 8, 15, 15, 6, 9};
    int n = values.size();
    structA::StructureA tree(values);
    Oracle oracle(values);

    for (int l = 0; l < n; l++) {
        for (int r = l + 1; r <= n; r++) {
            MaxResult expectedMax = oracle.queryMax(l, r - 1);
            structA::MaxCountNode actualMax = tree.queryMax(l, r);
            if (actualMax.maximum != expectedMax.maximum ||
                actualMax.count != expectedMax.count) {
                std::cout << "Maximum mismatch at [" << l << "," << r << ")\n";
                return 1;
            }

            long long expectedGcd = oracle.queryGCD(l, r - 1);
            structA::GcdValue actualGcd = tree.queryGCD(l, r);
            if (actualGcd != (structA::GcdValue)expectedGcd) {
                std::cout << "GCD mismatch at [" << l << "," << r << ")\n";
                return 1;
            }
        }
    }

    if (tree.queryMax(2, 2).count != 0) {
        std::cout << "Empty maximum range failed.\n";
        return 1;
    }
    if (tree.queryGCD(2, 2) != 0ULL) {
        std::cout << "Empty gcd range failed.\n";
        return 1;
    }

    std::cout << "Structure A query tests passed.\n";
    return 0;
}
