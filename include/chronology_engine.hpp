#ifndef CHRONOLOGY_ENGINE_HPP
#define CHRONOLOGY_ENGINE_HPP

#include <cmath>
#include <cstdint>

namespace Gabary::Core {

    struct SolarLunarCoords {
        double solar_longitude; // خط طول الشمس بالدرجات
        double lunar_phase;     // نسبة الطور القمري (0.0 - 1.0)
        double julian_day;      // اليوم اليولياني
    };

    class ChronologyEngine {
    public:
        static constexpr double absDayToJulianDay(int64_t abs_day) {
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

        static SolarLunarCoords computeVector(int64_t abs_day) {
            double jd = absDayToJulianDay(abs_day);
            double T  = getJulianCenturies(jd);
            double s_long = calculateSolarLongitude(T);
            
            int64_t lunar_base = 17250176;
            double lunar_phase = std::fmod(static_cast<double>(lunar_base + abs_day) / 29.53058886, 1.0);
            if (lunar_phase < 0) lunar_phase += 1.0;

            return { s_long, lunar_phase, jd };
        }
    };
}

#endif
