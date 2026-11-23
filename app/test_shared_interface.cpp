#include <iostream>
#include <numeric>
#include <random>
#include <functional>
#include "../include/sensor_replay.hpp"

using namespace std;
using namespace aggregates;

void require(bool condition, const string& message) {
    if (!condition) throw runtime_error(message);
}

void rejects(const function<void()>& action) {
    bool caught = false;
    try { action(); } catch (const out_of_range&) { caught = true; }
    require(caught, "Invalid range or index was accepted");
}

void compare(const vector<Reading>& values, const StructureAAdapter& a,
             const RangeMax& b, int left, int right) {
    Reading expected = emptyMaximum;
    int count = 0;
    GcdValue expectedGcd = 0;
    for (int i = left; i < right; ++i) {
        if (values[i] > expected) {
            expected = values[i];
            count = 1;
        } else if (values[i] == expected) {
            ++count;
        }
        GcdValue magnitude = values[i] < 0 ?
            0ULL - static_cast<GcdValue>(values[i]) : static_cast<GcdValue>(values[i]);
        expectedGcd = gcd(expectedGcd, magnitude);
    }
    const RangeMax& commonA = a;
    require(commonA.queryMax(left, right) == expected, "A maximum mismatch");
    require(b.queryMax(left, right) == expected, "B maximum mismatch");
    auto result = a.queryMaxCount(left, right);
    require(result.maximum == expected && result.count == count, "A count mismatch");
    require(a.queryGCD(left, right) == expectedGcd, "A gcd mismatch");
}

int main() {
    try {
        mt19937_64 random(253);
        for (int n : {0, 1, 2, 3, 7, 16, 37, 64}) {
            vector<Reading> values(n, LLONG_MIN);
            StructureAAdapter a(values);
            StructureBAdapter b(values);
            RangeMax& commonA = a;
            RangeMax& commonB = b;
            require(commonA.size() == n && commonB.size() == n, "Size mismatch");
            for (int left = 0; left <= n; ++left)
                for (int right = left; right <= n; ++right)
                    compare(values, a, commonB, left, right);
            rejects([&] { commonA.assign(-1, 0); });
            rejects([&] { commonB.assign(n, 0); });
            rejects([&] { commonA.queryMax(-1, 0); });
            rejects([&] { commonB.queryMax(0, n+1); });
            rejects([&] { commonA.queryMax(1, 0); });
            rejects([&] { a.queryMaxCount(0, n+1); });
            rejects([&] { a.queryGCD(-1, 0); });
            rejects([&] { commonB.queryMax(1, 0); });
            for (int step = 0; step < 2000 && n > 0; ++step) {
                int index = static_cast<int>(random() % n);
                Reading value = static_cast<Reading>(random() % 2001) - 1000;
                if (step % 11 == 0) value = LLONG_MIN;
                if (step % 13 == 0) value = LLONG_MAX;
                values[index] = value;
                commonA.assign(index, value);
                commonB.assign(index, value);
                int left = static_cast<int>(random() % (n+1));
                int right = static_cast<int>(random() % (n+1));
                if (left > right) swap(left, right);
                compare(values, a, commonB, left, right);
                if (step % 200 == 0)
                    require(a.validAgainst(values) && b.validAgainst(values), "Invariant mismatch");
            }
        }
        StructureAAdapter minimum({LLONG_MIN});
        require(minimum.queryGCD(0, 1) == 0ULL - static_cast<GcdValue>(LLONG_MIN),
                "Minimum signed value magnitude failed");
        for (Backend backend : {Backend::A, Backend::BHybrid}) {
            SensorReplay app({{0, 0}, {0, 0}}, backend);
            auto result = app.query(0, 2);
            require(result.maximum == 0 && result.count == 2 && result.gcd == 0,
                    "Zero reading identity failed");
            rejects([&] { app.assign(0, {-1, 0}); });
            rejects([&] { app.assign(0, {0, 1000000001}); });
            require(app.valid(), "Rejected assignment changed the state");
            rejects([&] { SensorReplay invalid({{-1, 0}}, backend); });
            rejects([&] { SensorReplay invalid({{0, 1000000001}}, backend); });
        }
        cout << "PASS: shared A/B contract, signed limits, empty ranges and invalid inputs; seed=253\n";
    } catch (const exception& error) {
        cerr << error.what() << '\n';
        return 1;
    }
}
