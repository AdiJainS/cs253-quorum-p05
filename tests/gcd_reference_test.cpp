
#include <iostream>
#include <cassert>

#include "gcd_reference.hpp"

int main() {
    GcdReference oracle({12, 18, 0, 30, 42});

    assert(oracle.query(0, 5) == 6);
    assert(oracle.query(1, 4) == 6);

    oracle.assign(1, 5);

    assert(oracle.query(0, 5) == 1);

    oracle.assign(1, 0);

    assert(oracle.query(0, 3) == 12);
    assert(oracle.query(2, 2) == 0);

    assert(oracle.query(0, 1) == 12);
    assert(oracle.query(0, 0) == 0);

    std::cout << "GCD reference tests passed!\n";
}