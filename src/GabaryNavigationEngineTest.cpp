#include "GabaryNavigationEngine.h"

#include <cassert>
#include <cstdint>
#include <iostream>

int main()
{
    Gabary::GabaryNavigationEngine navigation;

    std::cout
        << "============================================\n"
        << " Gabary V2 Navigation Engine Test\n"
        << "============================================\n\n";

    // --------------------------------------------------------
    // 1. Global Day 1
    // --------------------------------------------------------

    auto day1 = navigation.getDate(1);

    assert(day1.dayId == 1);
    assert(day1.solar.solarYear == 1);
    assert(day1.solar.solarMonth == 1);
    assert(day1.solar.solarDay == 1);

    assert(day1.lunar.year == 1);
    assert(day1.lunar.month == 1);
    assert(day1.lunar.day == 1);

    std::cout
        << "[PASS] Navigation -> Global Day 1\n";

    // --------------------------------------------------------
    // 2. Next Day
    // --------------------------------------------------------

    auto day2 = navigation.nextDay(1);

    assert(day2.dayId == 2);

    assert(day2.solar.solarYear == 1);
    assert(day2.solar.solarMonth == 1);
    assert(day2.solar.solarDay == 2);

    assert(day2.lunar.dayId == 2);

    std::cout
        << "[PASS] Next day navigation\n";

    // --------------------------------------------------------
    // 3. Previous Day
    // --------------------------------------------------------

    auto backToDay1 =
        navigation.previousDay(2);

    assert(backToDay1.dayId == 1);

    assert(backToDay1.solar.solarYear == 1);
    assert(backToDay1.solar.solarMonth == 1);
    assert(backToDay1.solar.solarDay == 1);

    std::cout
        << "[PASS] Previous day navigation\n";

    // --------------------------------------------------------
    // 4. Epoch boundary protection
    // --------------------------------------------------------

    auto epoch =
        navigation.previousDay(1);

    assert(epoch.dayId == 1);
    assert(epoch.solar.solarYear == 1);
    assert(epoch.solar.solarMonth == 1);
    assert(epoch.solar.solarDay == 1);

    std::cout
        << "[PASS] Epoch boundary protection\n";

    // --------------------------------------------------------
    // 5. Solar date jump
    // --------------------------------------------------------

    const int64_t solarDay =
        navigation.jumpToSolarDate(
            2026,
            7,
            26
        );

    auto solarResult =
        navigation.getDate(solarDay);

    assert(solarResult.dayId == solarDay);

    assert(solarResult.solar.solarYear == 2026);
    assert(solarResult.solar.solarMonth == 7);
    assert(solarResult.solar.solarDay == 26);

    std::cout
        << "[PASS] Solar date navigation\n";

    // --------------------------------------------------------
    // 6. Navigation across lunar cycle boundary
    // --------------------------------------------------------

    auto cycleEnd =
        navigation.getDate(11693);

    auto cycleNext =
        navigation.nextDay(11693);

    assert(cycleEnd.lunar.year == 33);
    assert(cycleEnd.lunar.month == 12);
    assert(cycleEnd.lunar.day == 30);

    assert(cycleNext.dayId == 11694);
    assert(cycleNext.lunar.year == 34);
    assert(cycleNext.lunar.month == 1);
    assert(cycleNext.lunar.day == 1);

    std::cout
        << "[PASS] Lunar cycle boundary navigation\n";

    // --------------------------------------------------------
    // 7. Long-range navigation
    // --------------------------------------------------------

    const int64_t testDays[] =
    {
        100000,
        1000000,
        5000000,
        10000000,
        15000000
    };

    for (int64_t dayId : testDays)
    {
        auto current =
            navigation.getDate(dayId);

        auto next =
            navigation.nextDay(dayId);

        auto previous =
            navigation.previousDay(dayId);

        assert(current.dayId == dayId);
        assert(next.dayId == dayId + 1);
        assert(previous.dayId == dayId - 1);
    }

    std::cout
        << "[PASS] Long-range navigation\n";

// --------------------------------------------------------
// 8. Global timeline boundaries
// --------------------------------------------------------

auto firstDay =
    navigation.getDate(
        Gabary::GabaryNavigationEngine::MIN_GLOBAL_DAY
    );

auto lastDay =
    navigation.getDate(
        Gabary::GabaryNavigationEngine::MAX_GLOBAL_DAY
    );

assert(firstDay.dayId == 1);

assert(lastDay.dayId == 18262125);
assert(lastDay.solar.solarYear == 50000);
assert(lastDay.solar.solarMonth == 12);
assert(lastDay.solar.solarDay == 31);

std::cout
    << "[PASS] Global timeline boundaries\n";

// --------------------------------------------------------
// 9. Maximum-day protection
// --------------------------------------------------------

auto afterEnd =
    navigation.nextDay(
        Gabary::GabaryNavigationEngine::MAX_GLOBAL_DAY
    );

assert(afterEnd.dayId == 18262125);
assert(afterEnd.solar.solarYear == 50000);
assert(afterEnd.solar.solarMonth == 12);
assert(afterEnd.solar.solarDay == 31);

std::cout
    << "[PASS] Maximum-day boundary protection\n";

// --------------------------------------------------------
// 10. Out-of-range protection
// --------------------------------------------------------

auto belowStart =
    navigation.getDate(0);

auto aboveEnd =
    navigation.getDate(18262126);

assert(belowStart.dayId == 1);
assert(aboveEnd.dayId == 18262125);

std::cout
    << "[PASS] Out-of-range protection\n";
    // --------------------------------------------------------
    // Final result
    // --------------------------------------------------------

    std::cout
        << "\n============================================\n"
        << " Gabary V2 Navigation Engine: PASSED\n"
        << "============================================\n";

    return 0;
}
