#pragma once
#include "range_max.hpp"
#include "../src/structA/structure_a.hpp"
#include "../src/structB/fenwick_max.hpp"

namespace aggregates {

struct MaximumCount {
    Reading maximum;
    int count;
};

class StructureAAdapter final : public RangeMax {
    structA::StructureA tree;
public:
    explicit StructureAAdapter(const std::vector<Reading>& values)
        : tree(checkedValues(values)) {}

    int size() const override { return tree.size(); }

    void assign(int index, Reading value) override {
        checkIndex(index, size());
        tree.assign(index, value);
    }

    Reading queryMax(int left, int right) const override {
        return queryMaxCount(left, right).maximum;
    }

    MaximumCount queryMaxCount(int left, int right) const {
        checkRange(left, right, size());
        auto result = tree.queryMax(left, right);
        return {result.maximum, result.count};
    }

    GcdValue queryGCD(int left, int right) const {
        checkRange(left, right, size());
        return tree.queryGCD(left, right);
    }

    bool validAgainst(const std::vector<Reading>& values) const {
        return tree.debugCheckAgainst(values);
    }
};

class StructureBAdapter final : public RangeMax {
    FenwickMax tree;
    int length;
public:
    explicit StructureBAdapter(const std::vector<Reading>& values)
        : tree(checkedValues(values)), length(static_cast<int>(values.size())) {}

    int size() const override { return length; }

    void assign(int index, Reading value) override {
        checkIndex(index, length);
        tree.assign(index, value);
    }

    Reading queryMax(int left, int right) const override {
        checkRange(left, right, length);
        if (left == right) return emptyMaximum;
        return tree.queryMax(left, right - 1);
    }

    bool validAgainst(const std::vector<Reading>& values) const {
        if (values.size() != static_cast<unsigned>(length)) return false;
        FenwickMax rebuilt(values);
        return tree.sameBlocksAs(rebuilt);
    }
};

}
