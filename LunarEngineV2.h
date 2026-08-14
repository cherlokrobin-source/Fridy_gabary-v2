#ifndef LUNAR_ENGINE_V2_H
#define LUNAR_ENGINE_V2_H

#include "GlobalLunarDay.h"
#include <cmath>
#include <algorithm>

class LunarEngineV2 {
private:
    // الثوابت الفلكية
    static constexpr double SYNODIC_MONTH = 29.53058867; // متوسط طول الشهر القمري بالأيام
    static constexpr int64_t LUNAR_EPOCH = 1;             // نقطة مرجع البداية (Absolute ID = 1)

public:
    LunarEngineV2() = default;

    /**
     * @brief حساب البيانات القمرية لليوم المطلق بتعقيد O(1)
     */
    static GlobalLunarDay calculateLunarDay(int64_t absoluteDayId) {
        GlobalLunarDay dayData;
        dayData.absoluteDayId = absoluteDayId;

        // 1. حساب الإزاحة المطلقة عن نقطة المرجع
        double deltaDays = static_cast<double>(absoluteDayId - LUNAR_EPOCH);

        // 2. حساب إجمالي الشهور القمرية المنقضية
        double totalMonths = deltaDays / SYNODIC_MONTH;
        int64_t totalMonthsInt = static_cast<int64_t>(std::floor(totalMonths));

        // 3. حساب السنة القمرية الحالية والشهر داخل السنة (1 - 12)
        dayData.lunarYear = (totalMonthsInt / 12) + 1;
        dayData.lunarMonth = static_cast<int>(totalMonthsInt % 12) + 1;

        // 4. حساب التقدم داخل الشهر القمري الحالي (Phase Progress 0.0 - 1.0)
        double progress = totalMonths - std::floor(totalMonths);
        if (progress < 0) progress += 1.0;
        dayData.phaseProgress = progress;

        // 5. حساب اليوم القمري (1 - 30)
        int dayInMonth = static_cast<int>(std::floor(progress * SYNODIC_MONTH)) + 1;
        dayData.lunarDay = std::min(dayInMonth, 30); // ضمان عدم تجاوز 30 يوماً

        // 6. تعيين اسم الطور والرمز التعبيري للـ TUI
        assignPhaseDetails(dayData);

        return dayData;
    }

private:
    static void assignPhaseDetails(GlobalLunarDay& day) {
        double p = day.phaseProgress;

        if (p < 0.0625 || p >= 0.9375) {
            day.phaseName = "New Moon (محاق)";
            day.phaseIcon = "🌑";
        } else if (p < 0.1875) {
            day.phaseName = "Waxing Crescent (هلال متزايد)";
            day.phaseIcon = "🌒";
        } else if (p < 0.3125) {
            day.phaseName = "First Quarter (تربيع أول)";
            day.phaseIcon = "🌓";
        } else if (p < 0.4375) {
            day.phaseName = "Waxing Gibbous (أحدب متزايد)";
            day.phaseIcon = "🌔";
        } else if (p < 0.5625) {
            day.phaseName = "Full Moon (بدر)";
            day.phaseIcon = "🌕";
        } else if (p < 0.6875) {
            day.phaseName = "Waning Gibbous (أحدب متناقص)";
            day.phaseIcon = "🌖";
        } else if (p < 0.8125) {
            day.phaseName = "Last Quarter (تربيع أخير)";
            day.phaseIcon = "🌗";
        } else {
            day.phaseName = "Waning Crescent (هلال متناقص)";
            day.phaseIcon = "🌘";
        }
    }
};

#endif // LUNAR_ENGINE_V2_H
