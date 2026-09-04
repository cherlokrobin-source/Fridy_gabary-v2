#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <iostream>
#include <string>

#include "chronology_engine.hpp"

using namespace ftxui;

struct CalendarState {
    int day = 1;
    int month = 9;
    long long year = 2026;
};

// دالة مساعدة لتفادي التعارض مع نطاقات FTXUI
template <typename T>
std::string to_str(T val) {
    return std::to_string(val);
}

// حساب اليوم الجولياني O(1)
long long calculate_jdn(long long y, int m, int d) {
    if (m <= 2) {
        y -= 1;
        m += 12;
    }
    long long a = y / 100;
    long long b = 2 - a + (a / 4);
    return static_cast<long long>(365.25 * (y + 4716)) + static_cast<int>(30.6001 * (m + 1)) + d + b - 1524;
}

int main() {
    auto screen = ScreenInteractive::TerminalOutput();
    CalendarState state;

    Gabary::Core::ChronologyEngine engine;

    auto renderer = Renderer([&]() -> Element {
        long long jdn = calculate_jdn(state.year, state.month, state.day);

        std::string year_str  = "السنة : " + to_str(state.year);
        std::string month_str = "الشهر : " + to_str(state.month);
        std::string day_str   = "اليوم  : " + to_str(state.day);
        std::string jdn_str   = "اليوم المطلق (JDN) : " + to_str(jdn);

        return vbox({
            text("─── محرك التقويم الذهبي (Gabary V2 Engine) ───") | bold | color(Color::Gold1) | center,
            separator(),
            hbox({
                vbox({
                    text("التاريخ المحدد:") | bold | color(Color::Cyan),
                    separator(),
                    text(year_str),
                    text(month_str),
                    text(day_str),
                }) | border | flex,

                vbox({
                    text("الحسابات الفلكية:") | bold | color(Color::Green),
                    separator(),
                    text(jdn_str) | color(Color::Yellow),
                    text("الحالة             : مزامنة قائمة") | color(Color::LightGreen),
                }) | border | flex,
            }),
            separator(),
            hbox({
                text("←/→: يوم | ↑/↓: شهر | PgUp/PgDn: سنة | Home: إعادة ضبط | Esc/Q: خروج") | dim | center
            }),
        }) | borderDouble;
    });

    auto main_component = CatchEvent(renderer, [&](Event event) {
        if (event == Event::ArrowRight) {
            state.day++;
            if (state.day > 30) { state.day = 1; state.month++; }
            return true;
        }
        if (event == Event::ArrowLeft) {
            state.day--;
            if (state.day < 1) { state.day = 30; state.month--; }
            return true;
        }
        if (event == Event::ArrowUp) {
            state.month++;
            if (state.month > 12) { state.month = 1; state.year++; }
            return true;
        }
        if (event == Event::ArrowDown) {
            state.month--;
            if (state.month < 1) { state.month = 12; state.year--; }
            return true;
        }
        if (event == Event::PageUp) {
            state.year++;
            return true;
        }
        if (event == Event::PageDown) {
            state.year--;
            return true;
        }
        if (event == Event::Home) {
            state.day = 1;
            state.month = 9;
            state.year = 2026;
            return true;
        }
        if (event == Event::Character('q') || event == Event::Character('Q') || event == Event::Escape) {
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });

    screen.Loop(main_component);
    return 0;
}
