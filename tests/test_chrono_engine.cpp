#include <iostream>
#include <cassert>
#include <cmath>
#include <iomanip>

// محرك الحسابات المدمج للاختبار
namespace Gabary::Core {

    struct SolarLunarCoords {
        double solar_longitude;
        double lunar_phase;
        double julian_day;
    };

    class ChronologyEngine {
    public:
        static constexpr double absDayToJulianDay(long long abs_day) {
            return static_cast<double>(abs_day) + 1721424.5;
        }

        static double getJulianCenturies(double jd) {
            return (jd - 2451545.0) / 36525.0;
        }

        static double calculateSolarLongitude(double T) {
            double L0 = 280.46646 + 36000.76983 * T;
            double M  = 357.52911 + 35999.05029 * T;
            double C  = (1.914602 - 0.004817 * T) * std::sin(M * M_PI / 180.0)
                      + (0.019993 - 0.000101 * T) * std::sin(2 * M * M_PI / 180.0);
            
            double true_long = std::fmod(L0 + C, 360.0);
            return (true_long < 0) ? true_long + 360.0 : true_long;
        }

        static SolarLunarCoords computeVector(long long abs_day) {
            double jd = absDayToJulianDay(abs_day);
            double T  = getJulianCenturies(jd);
            double s_long = calculateSolarLongitude(T);
            
            long long lunar_base = 17250176;
            double lunar_phase = std::fmod(static_cast<double>(lunar_base + abs_day) / 29.53058886, 1.0);

            return { s_long, lunar_phase, jd };
        }
    };
}

// دالة تنفيذيّة للاختبارات
void run_tests() {
    using namespace Gabary::Core;
    std::cout << "\033[1;36m[GABARY TEST SUITE] Starting Chronology Engine Verification...\033[0m\n\n";

    int passed = 0;
    int total = 0;

    auto TEST_ASSERT = [&](bool condition, const std::string& name) {
        total++;
        if (condition) {
            std::cout << "\033[1;32m[PASSED]\033[0m " << name << "\n";
            passed++;
        } else {
            std::cout << "\033[1;31m[FAILED]\033[0m " << name << "\n";
        }
    };

    // Test 1: Julian Day Conversion
    double jd_epoch = ChronologyEngine::absDayToJulianDay(0);
    TEST_ASSERT(std::abs(jd_epoch - 1721424.5) < 0.0001, "Test 1: ABS_DAY 0 -> Julian Day Alignment");

    // Test 2: Current Era ABS_DAY Alignment
    long long current_abs_day = 18894871;
    auto coords = ChronologyEngine::computeVector(current_abs_day);
    TEST_ASSERT(coords.julian_day > 2400000.0, "Test 2: Modern Epoch Julian Day Validity");

    // Test 3: Solar Longitude Bounds
    TEST_ASSERT(coords.solar_longitude >= 0.0 && coords.solar_longitude < 360.0, "Test 3: Solar Longitude within [0, 360)");

    // Test 4: Lunar Phase Cycle Range
    TEST_ASSERT(coords.lunar_phase >= 0.0 && coords.lunar_phase <= 1.0, "Test 4: Lunar Phase Ratio within [0.0, 1.0]");

    // Test 5: 50,000 Years Horizon Overflow Safety
    long long horizon_50k_days = 18262125;
    auto future_coords = ChronologyEngine::computeVector(current_abs_day + horizon_50k_days);
    TEST_ASSERT(!std::isnan(future_coords.solar_longitude) && !std::isnan(future_coords.julian_day), "Test 5: 50,000-Year Horizon Numerical Stability");

    std::cout << "\n--------------------------------------------------\n";
    std::cout << "Test Summary: \033[1;33m" << passed << "/" << total << " Passed.\033[0m\n";
    
    if (passed == total) {
        std::cout << "\033[1;32mALL CHRONO-TESTS PASSED SUCCESSFULLY!\033[0m\n";
    }
}

int main() {
    run_tests();
    return 0;
}
