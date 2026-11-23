#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/structA/structure_a.hpp"

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

bool sameCounters(const structA::SegmentTreeCounters& a,
                  const structA::SegmentTreeCounters& b) {
    return a.buildMerges == b.buildMerges && a.queryCalls == b.queryCalls &&
           a.queryBlocks == b.queryBlocks && a.queryMerges == b.queryMerges &&
           a.updateVisits == b.updateVisits && a.updateMerges == b.updateMerges;
}

struct StringOps {
    std::string identity() const { return ""; }
    std::string merge(const std::string& a, const std::string& b) const { return a + b; }
    void resetCounters() const {}
    unsigned long long euclidRemainderSteps() const { return 0; }
};

int main() {
    structA::SegmentTree<structA::GcdValue, structA::GcdOps> unbuilt;
    require(unbuilt.debugCheckAgainst({}), "Default empty tree must pass validation safely");
    unbuilt.build({});
    require(unbuilt.debugCheckAgainst({}), "Built empty tree must pass validation");

    structA::GcdSegmentTree emptyGcd;
    require(emptyGcd.debugCheckAgainst({}), "Default GCD wrapper must validate safely");
    structA::GcdSegmentTree gcd(std::vector<structA::GcdValue>{12, 18, 30, 42});
    gcd.resetCounters();
    gcd.query(0, 3);
    const auto gcdBefore = gcd.euclidRemainderSteps();
    require(gcd.debugCheckAgainst({12, 18, 30, 42}), "GCD wrapper rejected valid tree");
    require(gcd.euclidRemainderSteps() == gcdBefore, "GCD wrapper validation changed counters");

    std::vector<long long> values = {12, 18, 30};
    structA::StructureA tree(values);
    tree.queryGCD(0, 3);
    tree.queryMax(1, 3);
    tree.assign(1, 24);
    values[1] = 24;
    const auto before = tree.counters();
    require(tree.debugCheckAgainst(values), "Valid tree rejected");
    const auto after = tree.counters();
    require(sameCounters(before.maximum, after.maximum) &&
            sameCounters(before.gcd, after.gcd) &&
            before.gcdRemainderSteps == after.gcdRemainderSteps,
            "Validation changed operation counters");
    require(!tree.debugCheckAgainst({12, 18, 30}), "Incorrect leaves accepted");
    require(!tree.debugCheckAgainst({12}), "Incorrect size accepted");
    require(tree.counters().gcdRemainderSteps == before.gcdRemainderSteps,
            "Failed validation changed GCD counters");
    tree.resetCounters();
    require(tree.debugCheckAgainst(values), "Validation failed after counter reset");
    require(tree.counters().gcdRemainderSteps == 0,
            "Validation added GCD remainder steps after reset");

    // Concatenation checks ordering, which maximum and GCD cannot expose.
    std::vector<std::string> words = {"a", "b", "c", "d", "e", "f", "g"};
    structA::SegmentTree<std::string, StringOps> ordered(words);
    for (int l = 0; l <= (int)words.size(); ++l) {
        for (int r = l; r <= (int)words.size(); ++r) {
            std::string expected;
            for (int i = l; i < r; ++i) expected += words[i];
            require(ordered.query(l, r) == expected, "Noncommutative query reordered blocks");
        }
    }
    ordered.assign(3, "XYZ");
    require(ordered.query(1, 6) == "bcXYZef", "Noncommutative update/query failed");

    structA::StructureA extremes(std::vector<long long>{LLONG_MIN, LLONG_MIN, 0});
    const auto maximum = extremes.queryMax(0, 2);
    require(maximum.maximum == LLONG_MIN && maximum.count == 2,
            "LLONG_MIN values confused with empty identity");
    require(extremes.queryGCD(0, 2) == (1ULL << 63), "LLONG_MIN magnitude incorrect");
    require(extremes.queryMax(0, 0).count == 0, "Empty maximum identity incorrect");

    std::cout << "Structure A debug and edge-case tests passed.\n";
}
