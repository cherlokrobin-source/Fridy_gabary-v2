#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/dom/canvas.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <cmath>
#include <string>

using namespace ftxui;

// ============================================================================
// 1. CHRONO ENGINE: O(1) GEAR CALCULATOR
// ============================================================================
struct ChronoGears {
    static constexpr double DAYS_PER_YEAR  = 365.2425;
    static constexpr double DAYS_PER_EPOCH = 2083.0 * DAYS_PER_YEAR; // 2083Y
    static constexpr double DAYS_50K_HORIZON = 50000.0 * DAYS_PER_YEAR; // 50KY

    static double GetChronoRatio(long long day) {
        return std::fmod(static_cast<double>(day), 30.0) / 30.0;
    }

    static double GetYearRatio(long long day) {
        return std::fmod(static_cast<double>(day), DAYS_PER_YEAR) / DAYS_PER_YEAR;
    }

    static double GetEpochRatio(long long day) {
        return std::fmod(static_cast<double>(day), DAYS_PER_EPOCH) / DAYS_PER_EPOCH;
    }

    static double GetHorizonRatio(long long day) {
        double r = static_cast<double>(day) / DAYS_50K_HORIZON;
        return (r > 1.0) ? 1.0 : ((r < 0.0) ? 0.0 : r);
    }
};

// ============================================================================
// 2. CANVAS MECHANICS: RENDER 4-HAND ASTRONOMICAL DIAL
// ============================================================================
Element RenderMechanicalClock(long long absolute_day) {
    constexpr int width = 64;
    constexpr int height = 64;
    auto c = Canvas(width, height);

    const int cx = width / 2;
    const int cy = height / 2;
    const int max_r = 28;

    // --- A. Draw Gear Rim & Teeth ---
    for (double a = 0; a < 2 * M_PI; a += 0.08) {
        int x = cx + static_cast<int>(max_r * std::cos(a));
        int y = cy + static_cast<int>(max_r * std::sin(a));
        c.DrawPoint(x, y, true);
    }

    // 12 Gear Teeth Pivots
    for (int i = 0; i < 12; ++i) {
        double a = i * (2 * M_PI / 12.0) - (M_PI / 2.0);
        int x1 = cx + static_cast<int>((max_r - 2) * std::cos(a));
        int y1 = cy + static_cast<int>((max_r - 2) * std::sin(a));
        int x2 = cx + static_cast<int>(max_r * std::cos(a));
        int y2 = cy + static_cast<int>(max_r * std::sin(a));
        c.DrawPointLine(x1, y1, x2, y2);
    }

    // --- B. Calculate Hand Angles (Radians) ---
    const double a_chrono  = ChronoGears::GetChronoRatio(absolute_day) * 2 * M_PI - (M_PI / 2.0);
    const double a_year    = ChronoGears::GetYearRatio(absolute_day) * 2 * M_PI - (M_PI / 2.0);
    const double a_epoch   = ChronoGears::GetEpochRatio(absolute_day) * 2 * M_PI - (M_PI / 2.0);
    const double a_horizon = ChronoGears::GetHorizonRatio(absolute_day) * 2 * M_PI - (M_PI / 2.0);

    // --- C. Draw 4 Hands ---
    // 1. 50KY Horizon Hand (Longest - Outer Edge)
    int r_50k = max_r - 2;
    c.DrawPointLine(cx, cy, cx + static_cast<int>(r_50k * std::cos(a_horizon)), cy + static_cast<int>(r_50k * std::sin(a_horizon)));

    // 2. 2083Y Epoch Hand
    int r_epoch = max_r - 8;
    c.DrawPointLine(cx, cy, cx + static_cast<int>(r_epoch * std::cos(a_epoch)), cy + static_cast<int>(r_epoch * std::sin(a_epoch)));

    // 3. Solar Year Hand
    int r_year = max_r - 14;
    c.DrawPointLine(cx, cy, cx + static_cast<int>(r_year * std::cos(a_year)), cy + static_cast<int>(r_year * std::sin(a_year)));

    // 4. Daily Chrono Fast Hand
    int r_chrono = max_r - 20;
    c.DrawPointLine(cx, cy, cx + static_cast<int>(r_chrono * std::cos(a_chrono)), cy + static_cast<int>(r_chrono * std::sin(a_chrono)));

    // Center Escapement Pin
    c.DrawPoint(cx, cy, true);

    return canvas(std::move(c));
}

// ============================================================================
// 3. DASHBOARD LAYOUT & TELEMETRY
// ============================================================================
Element RenderDashboard(long long current_day) {
    double chrono_p  = ChronoGears::GetChronoRatio(current_day);
    double year_p    = ChronoGears::GetYearRatio(current_day);
    double epoch_p   = ChronoGears::GetEpochRatio(current_day);
    double horizon_p = ChronoGears::GetHorizonRatio(current_day);

    return hbox({
        // Mechanical Clock Panel
        vbox({
            text("⚙ GABARY V2: 4-HAND ASTRONOMICAL CHRONO ⚙") | bold | color(Color::Yellow) | center,
            separator(),
            RenderMechanicalClock(current_day) | center,
            separator(),
            text(" Controls: [UP] +1d | [DOWN] -1d | [RIGHT] +30d | [LEFT] -30d ") | dim | center,
        }) | borderHeavy | flex,

        // Telemetry Gauges Panel
        vbox({
            text(" [MECHANICAL GEAR TELEMETRY] ") | bold | color(Color::Cyan) | center,
            separator(),

            text("🔴 Daily Chrono Hand (30d Cycle):"),
            gauge(chrono_p) | color(Color::Red),
            text("   Progress: " + std::to_string(static_cast<int>(chrono_p * 100)) + "%") | dim,

            separator(),
            text("🟢 Solar Year Hand (365.2425d):"),
            gauge(year_p) | color(Color::Green),
            text("   Progress: " + std::to_string(static_cast<int>(year_p * 100)) + "%") | dim,

            separator(),
            text("🟡 2083Y Epoch Hand:"),
            gauge(epoch_p) | color(Color::Yellow),
            text("   Progress: " + std::to_string(static_cast<int>(epoch_p * 100)) + "%") | dim,

            separator(),
            text("🟣 50,000Y Horizon Hand:"),
            gauge(horizon_p) | color(Color::Magenta),
            text("   Progress: " + std::to_string(horizon_p * 100.0).substr(0, 5) + "%") | dim,

            separator(),
            vbox({
                text("SYSTEM STATE:") | bold,
                text("Absolute Day ID : #" + std::to_string(current_day)),
                text("Time Complexity : O(1) Constant"),
                text("Stack Usage     : < 1.0 MB"),
            }) | borderDashed
        }) | border | flex
    });
}

// ============================================================================
// 4. MAIN INTERACTIVE ENTRY POINT
// ============================================================================
int main() {
    auto screen = ScreenInteractive::TerminalOutput();

    long long current_day = 1; // Starting Day ID

    auto component = Renderer([&] {
        return RenderDashboard(current_day);
    });

    // Catch Keyboard Events
    component |= CatchEvent([&](Event event) {
        if (event == Event::ArrowUp) {
            current_day++;
            return true;
        }
        if (event == Event::ArrowDown && current_day > 1) {
            current_day--;
            return true;
        }
        if (event == Event::ArrowRight) {
            current_day += 30;
            return true;
        }
        if (event == Event::ArrowLeft && current_day > 31) {
            current_day -= 30;
            return true;
        }
        return false;
    });

    screen.Loop(component);
    return 0;
}

