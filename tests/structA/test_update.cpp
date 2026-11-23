#include <iostream>
#include <vector>

#include "../../src/structA/structure_a.hpp"
#include "../../oracle/oracle.hpp"

bool checkAllRanges(const structA::StructureA& tree, const Oracle& oracle, int n) {
    for (int l = 0; l < n; l++) {
        for (int r = l + 1; r <= n; r++) {
            MaxResult expectedMax = oracle.queryMax(l, r - 1);
            structA::MaxCountNode actualMax = tree.queryMax(l, r);
            if (actualMax.maximum != expectedMax.maximum ||
                actualMax.count != expectedMax.count) {
                return false;
            }

            long long expectedGcd = oracle.queryGCD(l, r - 1);
            if (tree.queryGCD(l, r) != (structA::GcdValue)expectedGcd) {
                return false;
            }
        }
    }
    return true;
}

int main() {
    std::vector<long long> values = {12, 8, 15, 15, 6, 9};
    int n = values.size();
    structA::StructureA tree(values);
    Oracle oracle(values);

    values[2] = 4;
    tree.assign(2, 4);
    oracle.assign(2, 4);
    structA::MaxCountNode result = tree.queryMax(0, 6);
    if (result.maximum != 15 || result.count != 1) {
        std::cout << "Decrease test failed.\n";
        return 1;
    }
    if (!checkAllRanges(tree, oracle, n)) {
        std::cout << "Decrease ranges failed.\n";
        return 1;
    }

    values[0] = 100;
    tree.assign(0, 100);
    oracle.assign(0, 100);
    result = tree.queryMax(0, 6);
    if (result.maximum != 100 || result.count != 1) {
        std::cout << "Increase test failed.\n";
        return 1;
    }
    if (!checkAllRanges(tree, oracle, n)) {
        std::cout << "Increase ranges failed.\n";
        return 1;
    }

    values[0] = -21;
    tree.assign(0, -21);
    oracle.assign(0, -21);
    if (!checkAllRanges(tree, oracle, n)) {
        std::cout << "Negative value ranges failed.\n";
        return 1;
    }

    std::cout << "Structure A update tests passed.\n";
    return 0;
}
