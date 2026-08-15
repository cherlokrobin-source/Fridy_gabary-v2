#ifndef GLOBAL_LUNAR_DAY_H
#define GLOBAL_LUNAR_DAY_H

#include <string>
#include <cstdint>

/**
 * @brief تمثيل البيانات المرجعة ليوم قمري مستقل
 */
struct GlobalLunarDay {
    int64_t absoluteDayId;   // الرقم المعرف الموحد للأيام
    int64_t lunarYear;       // السنة القمرية (حتى 50,000 سنة)
    int lunarMonth;          // الشهر القمري (1 إلى 12)
    int lunarDay;            // اليوم في الشهر القمري (1 إلى 30)
    
    double phaseProgress;    // نسبة طور القمر (0.00 إلى 1.00)
    std::string phaseName;   // اسم الطور (محاق، هلال، بدر...)
    std::string phaseIcon;   // الرمز التعبيري للواجهة (🌑, 🌒, 🌕...)
};

#endif // GLOBAL_LUNAR_DAY_H
