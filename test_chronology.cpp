#include <iostream>
#include <cassert>
#include <cmath>
#include <iomanip>

constexpr long long MAX_SOLAR_DAYS = 1825250;
constexpr long long MAX_LUNAR_DAYS = 1725050;

struct SolarLunarCoords {
    double solar_longitude;
    double lunar_longitude;
    double lunar_phase;
    double sync_coefficient;
    double julian_day;
};

class ChronologyEngine {
public:
    static SolarLunarCoords computeVectorAccurate(long long solar_abs_day) {
        SolarLunarCoords coords;

        constexpr double LUNAR_CYCLE_DAYS = 11697.85; 
        double cycle_progress = std::fmod(static_cast<double>(solar_abs_day), LUNAR_CYCLE_DAYS) / LUNAR_CYCLE_DAYS;
        if (cycle_progress < 0) cycle_progress += 1.0;

        constexpr double MEAN_LUNAR_MONTH = 29.53058886;
        double total_lunar_months = static_cast<double>(solar_abs_day) / MEAN_LUNAR_MONTH;
        
        coords.lunar_phase = std::fmod(total_lunar_months, 1.0);
        if (coords.lunar_phase < 0) coords.lunar_phase += 1.0;
        coords.lunar_longitude = coords.lunar_phase * 360.0;

        constexpr double TROPICAL_YEAR = 365.2422;
        double total_solar_years = static_cast<double>(solar_abs_day) / TROPICAL_YEAR;
        coords.solar_longitude = std::fmod(total_solar_years * 360.0, 360.0);
        if (coords.solar_longitude < 0) coords.solar_longitude += 360.0;

        double sync_diff = std::abs(coords.solar_longitude - coords.lunar_longitude);
        if (sync_diff > 180.0) sync_diff = 360.0 - sync_diff;
        coords.sync_coefficient = (180.0 - sync_diff) / 180.0;

        coords.julian_day = 2451545.0 + solar_abs_day;
        return coords;
    }
};

bool isClose(double a, double b, double tolerance = 1e-4) {
    return std::abs(a - b) < tolerance;
}

int main() {
    std::cout << "==========================================" << std::endl;
    std::cout << "🧪 GABARY V2 :: CHRONOLOGY ENGINE UNIT TESTS" << std::endl;
    std::cout << "==========================================" << std::endl << std::endl;

    std::cout << "[TEST 1] Testing Epoch Initial State (Day 1)... ";
    auto c1 = ChronologyEngine::computeVectorAccurate(1);
    assert(c1.solar_longitude >= 0.0 && c1.solar_longitude <= 360.0);
    assert(c1.lunar_phase >= 0.0 && c1.lunar_phase <= 1.0);
    assert(c1.sync_coefficient >= 0.0 && c1.sync_coefficient <= 1.0);
    std::cout << "PASSED ✅" << std::endl;

    std::cout << "[TEST 2] Testing Mid-Range Horizon (36,524 Days)... ";
    auto c2 = ChronologyEngine::computeVectorAccurate(36524);
    assert(c2.solar_longitude >= 0.0 && c2.solar_longitude <= 360.0);
    assert(c2.lunar_phase >= 0.0 && c2.lunar_phase <= 1.0);
    std::cout << "PASSED ✅" << std::endl;

    std::cout << "[TEST 3] Testing Maximum Horizon Boundary (1,825,250 Days)... ";
    auto c3 = ChronologyEngine::computeVectorAccurate(1825250);
    assert(!std::isnan(c3.solar_longitude));
    assert(!std::isnan(c3.sync_coefficient));
    assert(c3.lunar_phase >= 0.0 && c3.lunar_phase <= 1.0);
    std::cout << "PASSED ✅" << std::endl;

    std::cout << "[TEST 4] Testing Lunar Phase Periodicity... ";
    auto m1 = ChronologyEngine::computeVectorAccurate(1);
    auto m2 = ChronologyEngine::computeVectorAccurate(1 + static_cast<long long>(29.53058886));
    assert(isClose(m1.lunar_phase, m2.lunar_phase, 0.05));
    std::cout << "PASSED ✅" << std::endl;

    std::cout << std::endl << "✨ All Unit Tests PASSED Successfully!" << std::endl;
    std::cout << "==========================================" << std::endl;
    return 0;
}
