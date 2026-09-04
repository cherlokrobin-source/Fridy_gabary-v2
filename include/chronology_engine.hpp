#ifndef CHRONOLOGY_ENGINE_HPP
#define CHRONOLOGY_ENGINE_HPP

#include <cmath>
#include <algorithm>

namespace Gabary::Core {

struct SolarLunarCoords {
    double julian_day;
    double solar_longitude; // زاوية الطول الشمسي الحقيقية (0 - 360)
    double lunar_phase;     // طور القمر النجمي (0.0 = محاق، 0.5 = بدر)
    double lunar_longitude; // زاوية الطول القمري (0 - 360)
    double sync_coefficient;// معامل التزامن المركب بين الدورتين
};

class ChronologyEngine {
public:
    static constexpr double J2000_EPOCH = 2451545.0; // التاريخ الجولياني لأساس J2000

    static SolarLunarCoords computeVector(long long solar_abs_day) {
        SolarLunarCoords coords;

        // 1. حساب اليوم الجولياني المطلق بدقة متناهية
        coords.julian_day = J2000_EPOCH + static_cast<double>(solar_abs_day) - 1.0;

        // 2. حساب عدد القرون الفلكية من حقبة J2000.0 (Julian Centuries)
        double T = (coords.julian_day - J2000_EPOCH) / 36525.0;

        // 3. حساب الطول المتوسط للشمس (Mean Solar Longitude)
        double L0 = 280.46646 + 36000.76983 * T + 0.0003032 * T * T;
        L0 = std::fmod(L0, 360.0);
        if (L0 < 0) L0 += 360.0;

        // 4. حساب الشذوذ المتوسط للشمس (Mean Anomaly)
        double M = 357.52911 + 35999.05029 * T - 0.0001537 * T * T;
        double M_rad = M * M_PI / 180.0;

        // 5. معادلة المركز لحساب الطول الحقيقي للشمس (Equation of Center)
        double C = (1.914602 - 0.004817 * T) * std::sin(M_rad)
                 + (0.019993 - 0.000101 * T) * std::sin(2 * M_rad)
                 + 0.000289 * std::sin(3 * M_rad);

        coords.solar_longitude = std::fmod(L0 + C, 360.0);
        if (coords.solar_longitude < 0) coords.solar_longitude += 360.0;

        // 6. حساب طور القمر والدورة القمرية الاقترانية (29.53058867 يوم)
        double mean_lunar_day = std::fmod(static_cast<double>(solar_abs_day), 29.53058867);
        coords.lunar_phase = mean_lunar_day / 29.53058867;

        coords.lunar_longitude = std::fmod(coords.solar_longitude + (coords.lunar_phase * 360.0), 360.0);

        // 7. حساب معامل التزامن المركب (Sync Coefficient)
        double wave_s = std::sin(coords.solar_longitude * M_PI / 180.0);
        double wave_l = std::sin(coords.lunar_longitude * M_PI / 180.0);
        coords.sync_coefficient = std::abs(wave_s - wave_l);

        return coords;
    }
};

} // namespace Gabary::Core

#endif
