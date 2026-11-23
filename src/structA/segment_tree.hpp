#pragma once

#include <climits>
#include <stdexcept>
#include <vector>

namespace structA {

typedef unsigned long long GcdValue;

struct MaxCountNode {
    long long maximum = LLONG_MIN;
    int count = 0;

    bool operator==(const MaxCountNode& other) const {
        return maximum == other.maximum && count == other.count;
    }

    bool operator!=(const MaxCountNode& other) const {
        return !(*this == other);
    }
};

struct SegmentTreeCounters {
    unsigned long long buildMerges = 0;
    unsigned long long queryCalls = 0;
    unsigned long long queryBlocks = 0;
    unsigned long long queryMerges = 0;
    unsigned long long updateVisits = 0;
    unsigned long long updateMerges = 0;
};

struct MaxCountOps {
    MaxCountNode identity() const {
        MaxCountNode result;
        result.maximum = LLONG_MIN;
        result.count = 0;
        return result;
    }

    MaxCountNode merge(const MaxCountNode& left, const MaxCountNode& right) const {
        if (left.maximum > right.maximum) return left;
        if (right.maximum > left.maximum) return right;
        MaxCountNode result; // else both max are equal
        result.maximum = left.maximum;
        result.count = left.count + right.count;
        return result;
    }

    void resetCounters() const {}
    unsigned long long euclidRemainderSteps() const {
        return 0;
    }
};

struct GcdOps {
#ifdef STRUCTA_ENABLE_COUNTERS
    mutable unsigned long long remainderSteps = 0;
#endif

    GcdValue identity() const {
        return 0ULL;
    }


    GcdValue merge(GcdValue a, GcdValue b) const {
        while (b != 0ULL) {
            GcdValue remainder = a % b;
#ifdef STRUCTA_ENABLE_COUNTERS
            remainderSteps++;
#endif
            a = b;
            b = remainder;
        }
        return a;
    }

    void resetCounters() const {
#ifdef STRUCTA_ENABLE_COUNTERS
        remainderSteps = 0;
#endif
    }

    unsigned long long euclidRemainderSteps() const {
#ifdef STRUCTA_ENABLE_COUNTERS
        return remainderSteps;
#else
        return 0;
#endif
    }
};

template <class Node, class Ops>
class SegmentTree {
private:
    int n_ = 0;
    int base_ = 1;
    std::vector<Node> tree_;
    Ops ops_;
    mutable SegmentTreeCounters counters_;

public:
    SegmentTree() { tree_.assign(2, ops_.identity()); }

    SegmentTree(const std::vector<Node>& values) {
        build(values);
    }

    void build(const std::vector<Node>& values) {
        n_ = (int)values.size();
        base_ = 1;
        while (base_ < n_) {
            base_ = base_ * 2;
        }

        tree_.assign(2 * base_, ops_.identity());
        for (int i = 0; i < n_; i++) {
            tree_[base_ + i] = values[i];
        }

        counters_ = SegmentTreeCounters();
        ops_.resetCounters();
        for (int p = base_ - 1; p >= 1; p--) {
            tree_[p] = ops_.merge(tree_[2 * p], tree_[2 * p + 1]);
#ifdef STRUCTA_ENABLE_COUNTERS
            counters_.buildMerges++;
#endif
        }
    }

    int size() const {
        return n_;
    }

    int base() const {
        return base_;
    }

    void assign(int index, const Node& value) {
        if (index < 0 || index >= n_) {
            throw std::out_of_range("SegmentTree::assign: invalid index");
        }

        int p = base_ + index;
        tree_[p] = value;
#ifdef STRUCTA_ENABLE_COUNTERS
        counters_.updateVisits++;
#endif

        while (p > 1) {
            p = p / 2;
            tree_[p] = ops_.merge(tree_[2 * p], tree_[2 * p + 1]);
#ifdef STRUCTA_ENABLE_COUNTERS
            counters_.updateVisits++;
            counters_.updateMerges++;
#endif
        }
    }

    Node query(int l, int r) const {
        if (l < 0 || l > r || r > n_) {
            throw std::out_of_range("SegmentTree::query: invalid range");
        }

#ifdef STRUCTA_ENABLE_COUNTERS
        counters_.queryCalls++;
#endif
        if (l == r) {
            return ops_.identity();
        }

        Node leftResult = ops_.identity();
        Node rightResult = ops_.identity();

        l = l + base_;
        r = r + base_;

        while (l < r) {
            if (l % 2 == 1) {
                leftResult = ops_.merge(leftResult, tree_[l]);
#ifdef STRUCTA_ENABLE_COUNTERS
                counters_.queryBlocks++;
                counters_.queryMerges++;
#endif
                l++;
            }

            if (r % 2 == 1) {
                r--;
                rightResult = ops_.merge(tree_[r], rightResult);
#ifdef STRUCTA_ENABLE_COUNTERS
                counters_.queryBlocks++;
                counters_.queryMerges++;
#endif
            }

            l = l / 2;
            r = r / 2;
        }

#ifdef STRUCTA_ENABLE_COUNTERS
        counters_.queryMerges++;
#endif
        return ops_.merge(leftResult, rightResult);
    }

    bool debugCheckAgainst(const std::vector<Node>& expected) const {
        if (tree_.empty() || (int)expected.size() != n_) {
            return false;
        }

        for (int i = 0; i < n_; i++) {
            if (!(tree_[base_ + i] == expected[i])) {
                return false;
            }
        }

        for (int i = n_; i < base_; i++) {
            if (!(tree_[base_ + i] == ops_.identity())) {
                return false;
            }
        }

        // Validate with a copy so checking does not change operation counters.
        Ops checkOps = ops_;
        for (int p = base_ - 1; p >= 1; p--) {
            Node expectedParent = checkOps.merge(tree_[2 * p], tree_[2 * p + 1]);
            if (!(tree_[p] == expectedParent)) {
                return false;
            }
        }

        return true;
    }

    SegmentTreeCounters counters() const {
        return counters_;
    }

    unsigned long long euclidRemainderSteps() const {
        return ops_.euclidRemainderSteps();
    }

    void resetCounters() const {
        counters_ = SegmentTreeCounters();
        ops_.resetCounters();
    }
};

inline GcdValue gcdMagnitude(long long value) {
    if (value >= 0) {
        return (GcdValue)value;
    }
    return (GcdValue)(-(value + 1)) + 1ULL;
}

}
