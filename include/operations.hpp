
#pragma once

#include <cstdint>

enum class OpType {
    ASSIGN,
    QUERY_MAX,
    QUERY_GCD
};

struct Operation {
    OpType type;
    int l;
    int r;
    long long value;
};
