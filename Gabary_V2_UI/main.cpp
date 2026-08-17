#include "GlobalLunarDay.h"
#include "GlobalSolarDay.h"
#include "SolarEngineV2.h"
#include "LunarEngineV2.h"
using namespace Gabary;
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/event.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>

#include "GlobalSolarDay.h"
#include "SolarEngineV2.h"
#include "LunarEngineV2.h"

using namespace ftxui;

// ثوابت الأفق الزمني المزدوج
const int64_t LUNAR_MAX_LIMIT = 17718350; // سقف 50,000 سنة قمرية
const int64_t SOLAR_MAX_LIMIT = 18262125; // سقف 50,000 سنة شمسية

std::string getCurrentSystemTime() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%H:%M:%S");
    return ss.str();
}

std::string getCurrentSystemDate() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d");
    return ss.str();
}

int main() {
    auto screen = ScreenInteractive::TerminalOutput();
    int64_t currentDayId = 77;
    size_t marqueeOffset = 0;

    Gabary::SolarEngineV2 solarEngine;

    auto input_day_option = InputOption();
    std::string day_str = std::to_string(currentDayId);
    Component input_day = Input(&day_str, "Day ID...", input_day_option);

    auto renderer = Renderer(input_day, [&] () -> Element {
        try {
            if (!day_str.empty()) {
                currentDayId = std::stoll(day_str);
                if (currentDayId < 1) currentDayId = 1;
            }
        } catch (...) {}

        Gabary::GlobalSolarDay solarData = solarEngine.buildDay(currentDayId);
       LunarEngineV2 lunarEngine;
Date lDate = lunarEngine.getDate(currentDayId);

GlobalLunarDay lunarData;
lunarData.lunarYear = lDate.year;
lunarData.lunarMonth = lDate.month;
lunarData.lunarDay = lDate.day;
lunarData.phaseName = "LUNAR PHASE";
lunarData.phaseProgress = static_cast<double>(lDate.day) / 30.0;
lunarData.phaseIcon = "🌙";


        // --- منطق تجميد التقويم القمري عند 50,000 سنة قمرية ---
        bool isLunarCompleted = (currentDayId >= LUNAR_MAX_LIMIT);
        if (isLunarCompleted) {
            lunarData.lunarYear = 50000;
            lunarData.lunarMonth = 12;
            lunarData.lunarDay = 30;
            lunarData.phaseName = "FULL CYCLE DONE";
            lunarData.phaseProgress = 1.0;
        }

        // نسبة التقدم في الأفق المزدوج
        double lunarHorizonProgress = std::min(1.0, static_cast<double>(currentDayId) / LUNAR_MAX_LIMIT);
        double solarHorizonProgress = std::min(1.0, static_cast<double>(currentDayId) / SOLAR_MAX_LIMIT);

        int solarTotalDays = solarData.leapYear ? 366 : 365;

        double solarProgress = static_cast<double>(solarData.dayOfYear) / static_cast<double>(solarTotalDays);
        double lunarProgress = isLunarCompleted ? 1.0 : (static_cast<double>(lunarData.lunarDay) / 30.0);

        // 1. LUNAR CARD
        auto lunarCard = window(
            text(" 🌙 LUNAR CALENDAR ") | bold | color(Color::Cyan),
            vbox({
                hbox({
                    text("Date : ") | dim,
                    text(std::to_string(lunarData.lunarYear) + "-" + 
                         std::to_string(lunarData.lunarMonth) + "-" + 
                         std::to_string(lunarData.lunarDay)) | color(isLunarCompleted ? Color::Red : Color::GreenLight) | bold
                }),
                hbox({
                    text("Phase: ") | dim,
                    text(lunarData.phaseIcon + " " + lunarData.phaseName) | color(Color::MagentaLight)
                }),
                separator(),
                vbox({
                    text("Gauge:") | dim,
                    gauge(lunarProgress) | color(Color::Cyan)
                })
            })
        );

        // 2. CELESTIAL & CHRONO PANEL
        double julianDay = 1721425.5 + currentDayId;
        auto celestialCard = window(
            text(" 🔭 CELESTIAL & CHRONO ") | bold | color(Color::YellowLight),
            vbox({
                hbox({text("Julian Day : ") | dim, text(std::to_string(julianDay).substr(0, 10)) | color(Color::Cyan)}),
                hbox({text("Synodic    : ") | dim, text(std::to_string(lunarData.phaseProgress * 100.0).substr(0, 5) + "%") | color(Color::Magenta)}),
                hbox({text("Lunar Status: ") | dim, text(isLunarCompleted ? "LOCKED (50KY)" : "ACTIVE") | color(isLunarCompleted ? Color::Red : Color::Green) | bold}),
                hbox({text("Engine     : ") | dim, text("Golden Dual") | color(Color::Yellow)})
            })
        );

        // 3. SOLAR MATRIX
        Elements solarGridRows;
        solarGridRows.push_back(text(" Fr Sa Su Mo Tu We Th ") | color(Color::Yellow) | bold);

        int currentSolarDay = solarData.solarDay;
        int maxDaysInSolarMonth = solarEngine.daysInMonth(solarData.solarYear, solarData.solarMonth);

        int64_t solarFirstDayId = currentDayId - (solarData.solarDay - 1);
        Gabary::GlobalSolarDay startSolarData = solarEngine.buildDay(solarFirstDayId);
        int solarOffset = (startSolarData.weekIndex) % 7;
        if (solarOffset < 0) solarOffset += 7;

        Elements currentSolarRow;
        for (int i = 0; i < solarOffset; ++i) {
            currentSolarRow.push_back(text("   "));
        }

        int solarRowsCount = 0;
        for (int dayNum = 1; dayNum <= maxDaysInSolarMonth; ++dayNum) {
            std::string dayDisplay = (dayNum < 10 ? " " : "") + std::to_string(dayNum) + " ";
            if (dayNum == currentSolarDay) {
                currentSolarRow.push_back(text(dayDisplay) | bgcolor(Color::Green) | color(Color::Black) | bold);
            } else {
                currentSolarRow.push_back(text(dayDisplay));
            }

            if (currentSolarRow.size() == 7) {
                solarGridRows.push_back(hbox(currentSolarRow));
                currentSolarRow.clear();
                solarRowsCount++;
            }
        }
        if (!currentSolarRow.empty()) {
            while (currentSolarRow.size() < 7) currentSolarRow.push_back(text("   "));
            solarGridRows.push_back(hbox(currentSolarRow));
            solarRowsCount++;
        }
        while (solarRowsCount < 6) {
            solarGridRows.push_back(text("                      "));
            solarRowsCount++;
        }

        auto solarMatrixPanel = window(
            text(" 📅 MONTHLY MATRIX (" + std::to_string(solarData.solarMonth) + "-" + std::to_string(solarData.solarDay) + ") ") | bold | color(Color::Yellow),
            vbox(solarGridRows)
        ) | flex;

        // 4. LUNAR MATRIX
        Elements lunarGridRows;
        lunarGridRows.push_back(text(" Fr Sa Su Mo Tu We Th ") | color(Color::Cyan) | bold);

        int currentLunarDay = lunarData.lunarDay;
        int64_t lunarFirstDayId = currentDayId - (lunarData.lunarDay - 1);
        Gabary::GlobalSolarDay startLunarData = solarEngine.buildDay(lunarFirstDayId);
        int lunarOffset = (startLunarData.weekIndex) % 7;
        if (lunarOffset < 0) lunarOffset += 7;

        Elements currentLunarRow;
        for (int i = 0; i < lunarOffset; ++i) {
            currentLunarRow.push_back(text("   "));
        }

        int lunarRowsCount = 0;
        for (int dayNum = 1; dayNum <= 30; ++dayNum) {
            std::string dayDisplay = (dayNum < 10 ? " " : "") + std::to_string(dayNum) + " ";
            if (dayNum == currentLunarDay) {
                currentLunarRow.push_back(text(dayDisplay) | bgcolor(Color::Cyan) | color(Color::Black) | bold);
            } else {
                currentLunarRow.push_back(text(dayDisplay));
            }

            if (currentLunarRow.size() == 7) {
                lunarGridRows.push_back(hbox(currentLunarRow));
                currentLunarRow.clear();
                lunarRowsCount++;
            }
        }
        if (!currentLunarRow.empty()) {
            while (currentLunarRow.size() < 7) currentLunarRow.push_back(text("   "));
            lunarGridRows.push_back(hbox(currentLunarRow));
            lunarRowsCount++;
        }
        while (lunarRowsCount < 6) {
            lunarGridRows.push_back(text("                      "));
            lunarRowsCount++;
        }

        auto lunarMatrixPanel = window(
            text(" 🌙 LUNAR MATRIX (" + std::to_string(lunarData.lunarMonth) + "-" + std::to_string(lunarData.lunarDay) + ") ") | bold | color(Color::Cyan),
            vbox(lunarGridRows)
        ) | flex;

        // 5. SIGNATURE & REAL-TIME MARQUEE
        std::string fullBanner = " ★ GABARY V2 TEMPORAL SYSTEM ★ ARCHITECT: NEMISH BENAISSA ★ UNIVERSITÉ DJILLALI LIABÈS ★ 50,000Y CHRONOLOGY ENGINE ★ ";
        size_t bannerLength = fullBanner.length();
        std::string marqueeView = "";
        size_t viewWindowSize = 22;

        for (size_t i = 0; i < viewWindowSize; ++i) {
            marqueeView += fullBanner[(marqueeOffset + i) % bannerLength];
        }

        std::string sysTime = getCurrentSystemTime();
        std::string sysDate = getCurrentSystemDate();

        auto bottomPanel = window(text(" ✍️ SYSTEM SIGNATURE & REAL-TIME "), hbox({
            hbox({
                text("✍️ ") | color(Color::Yellow),
                text(marqueeView) | bold | color(Color::YellowLight) | bgcolor(Color::Blue)
            }) | flex,
            
            separator(),

            hbox({
                text("⏰") | color(Color::Yellow),
                text(" " + sysTime + " ") | bold | color(Color::White) | bgcolor(Color::Red),
                text(" "),
                text("📅") | color(Color::Green),
                text(" " + sysDate) | bold | color(Color::GreenLight)
            })
        }));

        // 6. DUAL HORIZON TRACKER PANEL
        auto horizonPanel = window(text(" 🌌 DUAL 50,000Y HORIZON TRACKER "), vbox({
            hbox({
                text("🌙 Lunar Horizon (17.71M d): [") | color(Color::Cyan),
                gauge(lunarHorizonProgress) | color(Color::Cyan),
                text("] " + std::to_string(lunarHorizonProgress * 100.0).substr(0, 5) + "% ") | bold | color(Color::CyanLight),
                text(isLunarCompleted ? " 🔒 [MAX 50KY REACHED]" : "") | color(Color::Red) | bold
            }),
            hbox({
                text("☀️ Solar Horizon (18.26M d): [") | color(Color::Yellow),
                gauge(solarHorizonProgress) | color(Color::Yellow),
                text("] " + std::to_string(solarHorizonProgress * 100.0).substr(0, 5) + "% ") | bold | color(Color::YellowLight)
            }),
            text("Absolute Day ID Index: #" + std::to_string(currentDayId)) | dim
        }));

        return vbox({
            text(" GABARY V2: TEMPORAL ENGINE DASHBOARD ") 
                | bold | center | color(Color::Yellow) | bgcolor(Color::Black),
            text(" [ARROWS: UP (+1d), DOWN (-1d), RIGHT (+30d), LEFT (-30d) | Q: QUIT] ") 
                | center | dim | color(Color::GrayLight),
            separator(),

            hbox({
                vbox({
                    window(text(" 🧩 NAV "), vbox({
                        text("[<< -10KY]"),
                        text("[(🎯) MID 25K]"),
                        text("[>> +10KY]")
                    })),
                    window(text(" 🪐 ASTRO "), vbox({
                        text("Season : Autumn") | color(Color::Green),
                        text("Century: #1") | color(Color::Magenta),
                        text("400Y   : #1") | color(Color::Magenta)
                    })),
                    window(text(" ⚡ CORE "), vbox({
                        text("Status: ⚡ RUN") | color(Color::Green),
                        text("UDL Sidi Bel Abbès") | dim
                    }))
                }) | size(WIDTH, LESS_THAN, 22),

                vbox({
                    window(text(" 🖥️ CONSOLE "), vbox({
                        text("🟢 " + solarData.weekName) | color(Color::Green) | bold,
                        text("   " + std::to_string(solarData.solarDay) + "-" + std::to_string(solarData.solarMonth) + "-" + std::to_string(solarData.solarYear)) | color(Color::Green) | bold,
                        text(solarData.leapYear ? "⚡ LEAP (366d)" : "⚡ COMMON (365d)") | color(Color::Cyan) | bold
                    })),
                    
                    window(text(" 📊 METRICS "), vbox({
                        text("Solar: " + std::to_string(solarData.dayOfYear) + "/" + std::to_string(solarTotalDays) + "d") | color(Color::Cyan) | bold,
                        gauge(solarProgress) | color(Color::Cyan),
                        
                        text("Lunar: " + std::to_string(lunarData.lunarDay) + "/30d") | color(Color::Magenta) | bold,
                        gauge(lunarProgress) | color(Color::Magenta),

                        separator(),
                        hbox({text("ID      : ") | color(Color::Yellow), text("#" + std::to_string(currentDayId)) | bold}),
                        hbox({text("ENGINEER: ") | color(Color::Green), text("Nemish")}),
                        hbox({text("SYSTEM  : ") | color(Color::Green), text("Gabary V2")})
                    }))
                }) | flex,

                vbox({
                    window(text(" 📥 INPUT DAY ID "), input_day->Render()),
                    lunarCard,
                    celestialCard
                }) | flex
            }),

            separator(),

            horizonPanel,

            separator(),

            hbox({
                solarMatrixPanel,
                separator(),
                lunarMatrixPanel
            }),

            window(text(" ⚙️ SYSTEM BENCHMARK "), vbox({
                text("Complexity : O(1) Constant Time") | color(Color::Green),
                text("Memory     : < 1.0 MB Stack") | color(Color::Green),
                text("Leap Rule  : 365.2425 Mean Days") | color(Color::Yellow)
            })),

            bottomPanel
        }) | border;
    });

    auto main_component = CatchEvent(renderer, [&](Event event) {
        marqueeOffset = (marqueeOffset + 1) % 120;

        if (event == Event::Character('q') || event == Event::Character('Q') || event == Event::Escape) {
            screen.ExitLoopClosure()();
            return true;
        }

        if (event == Event::ArrowDown) {
            currentDayId = std::max<int64_t>(1, currentDayId - 1);
            day_str = std::to_string(currentDayId);
            return true;
        }
        if (event == Event::ArrowUp) {
            currentDayId++;
            day_str = std::to_string(currentDayId);
            return true;
        }
        if (event == Event::ArrowRight) {
            currentDayId += 30;
            day_str = std::to_string(currentDayId);
            return true;
        }
        if (event == Event::ArrowLeft) {
            currentDayId = std::max<int64_t>(1, currentDayId - 30);
            day_str = std::to_string(currentDayId);
            return true;
        }
        return false;
    });

    screen.Loop(main_component);
    return 0;
}
