#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "SolarEngineV2.h"
#include "LunarEngineV2.h"

using namespace ftxui;

int main() {
    auto screen = ScreenInteractive::TerminalOutput();
    int64_t currentDayId = 1;

    auto renderer = Renderer([&] {
        GlobalSolarDay solarData = SolarEngineV2::calculateSolarDay(currentDayId);
        GlobalLunarDay lunarData = LunarEngineV2::calculateLunarDay(currentDayId);

        // 1. البطاقة القمرية في المربع الفارغ تحت INPUT DAY ID
        auto lunarCard = window(
            text(" 🌙 LUNAR CALENDAR ") | bold | color(Color::Cyan),
            vbox({
                hbox({
                    text("Lunar Date : ") | bold,
                    text(std::to_string(lunarData.lunarYear) + "-" + 
                         std::to_string(lunarData.lunarMonth) + "-" + 
                         std::to_string(lunarData.lunarDay)) | color(Color::GreenLight) | bold
                }),
                hbox({
                    text("Phase      : ") | bold,
                    text(lunarData.phaseIcon + " " + lunarData.phaseName) | color(Color::MagentaLight)
                }),
                separator(),
                hbox({
                    text("Progress   : ") | dim,
                    gauge(lunarData.phaseProgress) | color(Color::Cyan)
                })
            })
        );

        // 2. شبكة الجدول القمري (LUNAR MONTHLY MATRIX)
        Elements lunarGridRows;
        lunarGridRows.push_back(text("  1   2   3   4   5   6   7 ") | color(Color::Cyan) | bold);
        
        int currentLunarDay = lunarData.lunarDay;
        for (int week = 0; week < 4; ++week) {
            Elements row;
            for (int dayOfWeek = 1; dayOfWeek <= 7; ++dayOfWeek) {
                int dayNum = week * 7 + dayOfWeek;
                if (dayNum > 30) break;

                std::string dayStr = (dayNum < 10 ? "  " : " ") + std::to_string(dayNum) + " ";
                if (dayNum == currentLunarDay) {
                    row.push_back(text(dayStr) | bgcolor(Color::Cyan) | color(Color::Black) | bold);
                } else {
                    row.push_back(text(dayStr));
                }
            }
            lunarGridRows.push_back(hbox(row));
        }

        auto lunarMatrixPanel = window(
            text(" 🌙 LUNAR MATRIX (" + std::to_string(lunarData.lunarMonth) + "-" + std::to_string(lunarData.lunarDay) + ") "),
            vbox(lunarGridRows)
        ) | flex;

        // 3. شبكة الجدول الشمسي
        auto solarMatrixPanel = window(
            text(" 📅 SOLAR MATRIX (1-1) "),
            vbox({
                text(" Fr  Sa  Su  Mo  Tu  We  Th ") | color(Color::Yellow) | bold,
                hbox({text("  1 ") | bgcolor(Color::Green) | color(Color::Black) | bold, text("  2   3   4   5   6   7")}),
                text("  8   9  10  11  12  13  14"),
                text(" 15  16  17  18  19  20  21"),
                text(" 22  23  24  25  26  27  28"),
                text(" 29  30  31   1   2   3   4")
            })
        ) | flex;

        return vbox({
            text(" GABARY V2: TEMPORAL ENGINE DASHBOARD ") 
                | bold | center | color(Color::Yellow) | bgcolor(Color::Black),
            separator(),

            hbox({
                vbox({
                    window(text(" 🧩 NAV "), vbox({
                        text("[<< -10KY]"),
                        text("[(🎯) MID 25K]"),
                        text("[>> +10KY]")
                    })),
                    window(text(" 🪐 ASTRO "), vbox({
                        text("Season : Winter") | color(Color::Green),
                        text("Century: #1") | color(Color::Magenta),
                        text("400Y   : #1") | color(Color::Magenta)
                    }))
                }) | flex,

                vbox({
                    window(text(" 🖥️ CONSOLE "), vbox({
                        text("🟢 Friday, 1-1-1") | color(Color::Green) | bold,
                        text("⚡ COMMON YEAR (365d)") | color(Color::Cyan) | bold
                    })),
                    window(text(" 📊 METRICS "), vbox({
                        text("Year Progress: 1/365") | dim,
                        gauge(1.0 / 365.0) | color(Color::Cyan),
                        separator(),
                        hbox({text("ABSOLUTE ID : ") | color(Color::Yellow), text("#" + std::to_string(currentDayId)) | bold}),
                        hbox({text("ENGINEER    : ") | color(Color::Green), text("Nemimeche Benaissa")}),
                        hbox({text("PROJECT SYS : ") | color(Color::Green), text("Gabary V2")})
                    }))
                }) | flex,

                vbox({
                    window(text(" 📥 INPUT DAY ID "), text(std::to_string(currentDayId)) | bold | center),
                    lunarCard
                }) | flex
            }),

            separator(),

            window(text(" 🌌 50,000Y HORIZON TRACKER "), vbox({
                hbox({
                    text("0Y [") | dim,
                    gauge(static_cast<double>(currentDayId) / 18262125.0) | color(Color::Cyan),
                    text("] 50KY (0.0000%)") | dim
                }),
                text("Absolute ID Index: #" + std::to_string(currentDayId)) | dim
            })),

            separator(),

            // إظهار الجدولين الشمسي والقمري أفقياً
            hbox({
                solarMatrixPanel,
                separator(),
                lunarMatrixPanel
            }),

            window(text(" ⚙️ SYSTEM BENCHMARK "), vbox({
                text("Complexity : O(1) Constant Time") | color(Color::Green),
                text("Memory     : < 1.0 MB Stack") | color(Color::Green),
                text("Leap Rule  : 365.2425 Mean Days") | color(Color::Yellow)
            }))
        }) | border;
    });

    screen.Loop(renderer);
    return 0;
}
