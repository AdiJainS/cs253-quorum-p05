#pragma once
#include <climits>
#include <stdexcept>
#include <vector>

namespace aggregates {

using Reading = long long;
using GcdValue = unsigned long long;
inline constexpr Reading emptyMaximum = LLONG_MIN;
inline constexpr int maximumSize = 1000000;

inline const std::vector<Reading>& checkedValues(const std::vector<Reading>& values) {
    if (values.size() > static_cast<unsigned>(maximumSize))
        throw std::length_error("At most 1000000 values are supported");
    return values;
}

inline void checkIndex(int index, int size) {
    if (index < 0 || index >= size) throw std::out_of_range("Invalid index");
}

inline void checkRange(int left, int right, int size) {
    if (left < 0 || left > right || right > size)
        throw std::out_of_range("Invalid half-open range");
}

class RangeMax {
public:
    virtual ~RangeMax() = default;
    virtual int size() const = 0;
    virtual void assign(int index, Reading value) = 0;
    virtual Reading queryMax(int left, int right) const = 0;
};

}
