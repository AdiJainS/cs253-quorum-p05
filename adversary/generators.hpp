#pragma once

#include <stdexcept>
#include <vector>

#include "operations.hpp"

// A workload contains starting values and operations.
struct Workload {
    std::vector<long long> initial;
    std::vector<Operation> operations;
};

// Examples: 4, 8, 16 and 32 are powers of two.
inline bool isPowerOfTwo(int n) {
    if (n < 1) {
        return false;
    }

    while (n % 2 == 0) {
        n = n / 2;
    }

    return n == 1;
}

// Workload 1: remove the current maximum repeatedly.
inline Workload makeRebuildWorkload(int n, int k) {
    if (n < 1 || k < 1 || k > n) {
        throw std::invalid_argument(
            "Require n >= 1 and 1 <= k <= n"
        );
    }

    Workload result;

    // Example for n=4: [4, 3, 2, 1].
    for (int i = 0; i < n; i++) {
        result.initial.push_back(n - i);
    }

    for (int i = 0; i < k; i++) {
        result.operations.push_back({
            OpType::ASSIGN, i, 0, 0
        });

        result.operations.push_back({
            OpType::QUERY_MAX, 0, n - 1, 0
        });
    }

    return result;
}

// Workload 2: repeat a maximum-step query.
inline Workload makeQueryWorkload(int n, int queryCount) {
    if (n < 4 || !isPowerOfTwo(n) || queryCount < 1) {
        throw std::invalid_argument(
            "Require power-of-two n >= 4 and queryCount >= 1"
        );
    }

    Workload result;

    // Example for n=4: [1, 2, 3, 4].
    for (int i = 0; i < n; i++) {
        result.initial.push_back(i + 1);
    }

    for (int i = 0; i < queryCount; i++) {
        result.operations.push_back({
            OpType::QUERY_MAX, 0, n - 2, 0
        });
    }

    return result;
}

// Workload 3: repeatedly decrease the unique maximum at index 0.
inline Workload makeDecreaseWorkload(int n, int decreases) {
    if (n < 4 || !isPowerOfTwo(n) || decreases < 1) {
        throw std::invalid_argument(
            "Require power-of-two n >= 4 and decreases >= 1"
        );
    }

    Workload result;

    for (int i = 0; i < n; i++) {
        result.initial.push_back(0);
    }

    // 1LL makes the addition use long long arithmetic.
    result.initial[0] = decreases + 1LL;

    for (int i = 0; i < decreases; i++) {
        long long value = decreases - i;

        result.operations.push_back({
            OpType::ASSIGN, 0, 0, value
        });

        result.operations.push_back({
            OpType::QUERY_MAX, 0, n - 1, 0
        });
    }

    return result;
}