#pragma once
#include <memory>
#include "structure_adapters.hpp"

namespace aggregates {

enum class Backend { A, BHybrid };

struct Sensor {
    Reading reading;
    GcdValue calibration;
};

struct Answer {
    Reading maximum;
    int count;
    GcdValue gcd;
};

class SensorReplay {
    static void checkSensor(Sensor sensor) {
        if (sensor.reading < 0 || sensor.reading > 1000000000 ||
            sensor.calibration > 1000000000)
            throw std::out_of_range("Sensor values must be in [0, 1000000000]");
    }

    static std::vector<Reading> readingValues(const std::vector<Sensor>& sensors) {
        if (sensors.size() > static_cast<unsigned>(maximumSize))
            throw std::length_error("At most 1000000 sensors are supported");
        std::vector<Reading> values;
        values.reserve(sensors.size());
        for (Sensor sensor : sensors) {
            checkSensor(sensor);
            values.push_back(sensor.reading);
        }
        return values;
    }

    static std::vector<GcdValue> calibrationValues(const std::vector<Sensor>& sensors) {
        std::vector<GcdValue> values;
        values.reserve(sensors.size());
        for (Sensor sensor : sensors) values.push_back(sensor.calibration);
        return values;
    }

    std::vector<Sensor> current;
    StructureAAdapter readings;
    structA::GcdSegmentTree calibrations;
    std::unique_ptr<StructureBAdapter> fenwick;

public:
    explicit SensorReplay(const std::vector<Sensor>& sensors, Backend backend = Backend::A)
        : current(sensors), readings(readingValues(sensors)),
          calibrations(calibrationValues(sensors)) {
        if (backend == Backend::BHybrid)
            fenwick = std::make_unique<StructureBAdapter>(readingValues(sensors));
    }

    void assign(int index, Sensor sensor) {
        checkIndex(index, static_cast<int>(current.size()));
        checkSensor(sensor);
        readings.assign(index, sensor.reading);
        calibrations.assign(index, sensor.calibration);
        if (fenwick) fenwick->assign(index, sensor.reading);
        current[index] = sensor;
    }

    Answer query(int left, int right) const {
        checkRange(left, right, static_cast<int>(current.size()));
        if (left == right) return {-1, 0, 0};
        auto result = readings.queryMaxCount(left, right);
        Reading maximum = fenwick ? fenwick->queryMax(left, right) : result.maximum;
        return {maximum, result.count, calibrations.query(left, right)};
    }

    bool valid() const {
        auto values = readingValues(current);
        return readings.validAgainst(values) &&
               calibrations.debugCheckAgainst(calibrationValues(current)) &&
               (!fenwick || fenwick->validAgainst(values));
    }
};

}
