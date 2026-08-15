#include <iostream>
#include "LunarEngineV2.h"

int main() {
    int64_t testIds[] = {1, 15, 1000, 17718350}; // تجربة أيام متنوعة وصولاً إلى 50,000 سنة قمرية

    std::cout << "========= Lunar Engine V2 Test =========" << std::endl;
    for (int64_t id : testIds) {
        GlobalLunarDay day = LunarEngineV2::calculateLunarDay(id);
        std::cout << "Day ID: " << day.absoluteDayId
                  << " | Lunar Year: " << day.lunarYear
                  << " | Month: " << day.lunarMonth
                  << " | Day: " << day.lunarDay
                  << " | Phase: " << day.phaseIcon << " " << day.phaseName << std::endl;
    }
    std::cout << "========================================" << std::endl;

    return 0;
}
