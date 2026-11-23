
#include <iostream>
#include <cassert>
#include "oracle.hpp"

int main() {
    std::vector<long long> a = {12, 8, 15, 15, 6, 9};

    Oracle oracle(a);

    auto ans = oracle.queryMax(0, 5);

    assert(ans.maximum == 15);
    assert(ans.count == 2);

    assert(oracle.queryGCD(0, 2) == 1);

    oracle.assign(2, 4);

    ans = oracle.queryMax(0, 5);

    assert(ans.maximum == 15);
    assert(ans.count == 1);

    assert(oracle.queryGCD(0, 2) == 4);

    ans = oracle.queryMax(2, 2);

    assert(ans.maximum == 4);
    assert(ans.count == 1);

    std::cout << "All Oracle tests passed!\n";

    return 0;
}
