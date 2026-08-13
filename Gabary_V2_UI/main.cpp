#include <ftxui/dom/elements.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include "SolarEngineV2.h"
#include "GlobalSolarDay.h"
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>

using namespace ftxui;

Element CustomProgressBar(float ratio, int width = 14) {
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;
    
    int filled = static_cast<int>(ratio * width);
    std::string bar = "[";
    for (int i = 0; i < width; ++i) {
        if (i < filled) bar += "█";
        else bar += "░";
    }
    bar += "]";
    return text(bar) | color(Color::Cyan) | bold;
}

Element GenerateAcademicGrid(int start_offset, int total_days, int current_day_num, int prev_month_days) {
    Elements rows;
    Elements current_row;

    for (int i = start_offset - 1; i >= 0; --i) {
        int day_val = prev_month_days - i;
        std::string day_str = (day_val < 10 ? "  " : " ") + std::to_string(day_val) + "  ";
        current_row.push_back(text(day_str) | dim | color(Color::GrayDark));
    }

    for (int day = 1; day <= total_days; ++day) {
        std::string day_str = (day < 10 ? "  " : " ") + std::to_string(day) + "  ";
        
        if (day == current_day_num) {
            current_row.push_back(text(day_str) | bgcolor(Color::Green) | color(Color::Black) | bold);
        } else {
            current_row.push_back(text(day_str) | color(Color::CyanLight));
        }

        if (current_row.size() == 7) {
            rows.push_back(hbox(current_row));
            current_row.clear();
        }
    }

    int next_month_day = 1;
    while (rows.size() < 6) {
        while (current_row.size() < 7) {
            std::string day_str = (next_month_day < 10 ? "  " : " ") + std::to_string(next_month_day++) + "  ";
            current_row.push_back(text(day_str) | dim | color(Color::GrayDark));
        }
        rows.push_back(hbox(current_row));
        current_row.clear();
    }

    return vbox(rows);
}

std::string GetDayName(int64_t day_id) {
    static const std::string days[] = {"Friday", "Saturday", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday"};
    int idx = static_cast<int>((day_id - 1) % 7);
    if (idx < 0) idx += 7;
    return days[idx];
}

std::string GetSeason(int day_of_year) {
    if (day_of_year >= 80 && day_of_year < 172) return "Spring";
    if (day_of_year >= 172 && day_of_year < 264) return "Summer";
    if (day_of_year >= 264 && day_of_year < 355) return "Autumn";
    return "Winter";
}

int main() {
    auto screen = ScreenInteractive::Fullscreen();

    Gabary::SolarEngineV2 engine;
    int64_t current_day_id = 1;
    std::string input_text = "1";

    auto input_comp = Input(&input_text, "Enter Day ID...");

    auto btn_prev = Button(" [<< -10KY] ", [&] { 
        current_day_id = std::max((int64_t)1, current_day_id - 3652425); 
        input_text = std::to_string(current_day_id); 
    });
    auto btn_next = Button(" [>> +10KY] ", [&] { 
        current_day_id += 3652425; 
        input_text = std::to_string(current_day_id); 
    });
    auto btn_mid = Button(" [(🎯) MID 25K] ", [&] { 
        current_day_id = 9131062; 
        input_text = std::to_string(current_day_id); 
    });

    auto main_container = Container::Vertical({
        input_comp,
        btn_prev,
        btn_next,
        btn_mid
    });

    auto catch_event_container = CatchEvent(main_container, [&](Event event) {
        if (event == Event::ArrowRight) {
            current_day_id++;
            input_text = std::to_string(current_day_id);
            return true;
        }
        if (event == Event::ArrowLeft) {
            if (current_day_id > 1) current_day_id--;
            input_text = std::to_string(current_day_id);
            return true;
        }
        if (event == Event::ArrowUp) {
            current_day_id += 30;
            input_text = std::to_string(current_day_id);
            return true;
        }
        if (event == Event::ArrowDown) {
            if (current_day_id > 30) current_day_id -= 30;
            input_text = std::to_string(current_day_id);
            return true;
        }
        return false;
    });

    auto renderer = Renderer(catch_event_container, [&]() -> Element {
        try {
            int64_t parsed_id = std::stoll(input_text);
            if (parsed_id > 0) current_day_id = parsed_id;
        } catch (...) {}

        Gabary::GlobalSolarDay day = engine.buildDay(current_day_id);

        bool is_leap = (day.solarYear % 4 == 0 && day.solarYear % 100 != 0) || (day.solarYear % 400 == 0);
        int64_t cycle_400_id = (current_day_id - 1) / 146097 + 1;
        int century_num = (day.solarYear - 1) / 100 + 1;

        auto timeline_box = window(text(" 🧭 NAV ") | bold | color(Color::Cyan), vbox({
            btn_prev->Render(),
            btn_mid->Render(),
            btn_next->Render()
        }));

        auto globe_box = window(text(" 🪐 ASTRO ") | bold | color(Color::Yellow), vbox({
            text("Season : " + GetSeason(day.dayOfYear)) | color(Color::Green),
            text("Century: #" + std::to_string(century_num)),
            text("400Y   : #" + std::to_string(cycle_400_id)) | color(Color::Magenta)
        }));

        auto status_box = window(text(" CORE ") | bold | color(Color::Green), vbox({
            text("Status: ⚡ RUN") | color(Color::Green) | bold,
            text("UDL Sidi Bel Abbès") | dim
        }));

        auto left_column = vbox({
            timeline_box,
            globe_box,
            status_box
        }) | size(WIDTH, EQUAL, 20);

        std::string day_name = GetDayName(current_day_id);
        std::string full_date_str = day_name + ", " + std::to_string(day.solarYear) + "-" + std::to_string(day.solarMonth) + "-" + std::to_string(day.solarDay);

        auto console_box = window(text(" 🖥️ CONSOLE ") | bold | color(Color::Magenta), vbox({
            text("🟢 " + full_date_str) | color(Color::Yellow) | bold,
            text("⚡ " + std::string(is_leap ? "LEAP YEAR (366d)" : "COMMON YEAR (365d)")) | color(is_leap ? Color::Red : Color::Cyan)
        }));

        float year_ratio = static_cast<float>(day.dayOfYear) / (is_leap ? 366.0f : 365.0f);
        auto astro_box = window(text(" 📊 METRICS ") | bold | color(Color::Green), vbox({
            text("Year Progress: " + std::to_string(day.dayOfYear) + "/" + (is_leap ? "366" : "365")),
            CustomProgressBar(year_ratio, 16),
            separator(),
            text("ABSOLUTE ID : #" + std::to_string(current_day_id)) | color(Color::Yellow),
            text("ENGINEER    : Nemimeche Benaissa") | color(Color::Green),
            text("PROJECT SYS : Gabary V2") | color(Color::Cyan)
        }));

        auto input_box = window(text(" ⌨️ INPUT DAY ID ") | bold | color(Color::Cyan), input_comp->Render());

        auto right_column = vbox({
            console_box,
            astro_box,
            input_box
        }) | flex;

        double max_days = 50000.0 * 365.2425;
        float horizon_ratio = static_cast<float>(current_day_id / max_days);
        std::stringstream ss;
        ss << std::fixed << std::setprecision(4) << (current_day_id / max_days * 100.0) << "%";

        auto horizon_gauge = window(text(" 🌌 50,000Y HORIZON TRACKER ") | bold | color(Color::Cyan), vbox({
            hbox({
                text("0Y "),
                CustomProgressBar(horizon_ratio, 14),
                text(" 50KY (" + ss.str() + ")")
            }),
            text("Absolute ID Index: #" + std::to_string(current_day_id)) | dim
        }));

        int64_t first_day_of_month_id = current_day_id - day.solarDay + 1;
        int start_offset = static_cast<int>((first_day_of_month_id - 1) % 7);
        if (start_offset < 0) start_offset += 7;

        int total_month_days = (day.solarMonth == 2) ? (is_leap ? 29 : 28) : ((day.solarMonth % 2 == 1) ? 31 : 30);
        
        int prev_month = (day.solarMonth == 1) ? 12 : day.solarMonth - 1;
        int prev_year = (day.solarMonth == 1) ? day.solarYear - 1 : day.solarYear;
        bool prev_is_leap = (prev_year % 4 == 0 && prev_year % 100 != 0) || (prev_year % 400 == 0);
        int prev_month_days = (prev_month == 2) ? (prev_is_leap ? 29 : 28) : ((prev_month % 2 == 1) ? 31 : 30);

        auto days_header = hbox({
            text("  Fr ") | color(Color::Red) | bold,
            text("  Sa ") | color(Color::Yellow) | bold,
            text("  Su ") | color(Color::White) | bold,
            text("  Mo ") | color(Color::White) | bold,
            text("  Tu ") | color(Color::White) | bold,
            text("  We ") | color(Color::White) | bold,
            text("  Th ") | color(Color::White) | bold
        });

        auto calendar_grid_box = window(text(" 📅 MONTHLY MATRIX (" + std::to_string(day.solarYear) + "-" + std::to_string(day.solarMonth) + ") ") | bold | color(Color::Yellow), vbox({
            days_header,
            separator(),
            GenerateAcademicGrid(start_offset, total_month_days, day.solarDay, prev_month_days)
        }));

        auto perf_box = window(text(" ⚙️ SYSTEM BENCHMARK ") | bold | color(Color::Cyan), vbox({
            text("Complexity : O(1) Constant Time") | color(Color::Green),
            text("Memory     : < 1.0 MB Stack") | color(Color::Cyan),
            text("Leap Rule  : 365.2425 Mean Days") | color(Color::Yellow)
        }));

        auto live_ticker_bar = window(text(" 📡 LIVE ENGINE STATUS ") | bold | color(Color::Red), vbox({
            text("TARGET: " + full_date_str + " | ID: #" + std::to_string(current_day_id)) | color(Color::Yellow) | bold | hcenter
        }));

        auto hotkeys_bar = hbox({
            text(" [<- / ->] Day ") | bgcolor(Color::Blue) | color(Color::White),
            text(" "),
            text(" [^ / v] Month ") | bgcolor(Color::Blue) | color(Color::White),
            text(" "),
            text(" [TAB] Nav ") | bgcolor(Color::GrayDark) | color(Color::White)
        }) | hcenter;

        return vbox({
            text(" GABARY V2: TEMPORAL ENGINE DASHBOARD ") | bold | color(Color::Yellow) | hcenter,
            hbox({ left_column, right_column }),
            horizon_gauge,
            calendar_grid_box,
            perf_box,
            live_ticker_bar,
            hotkeys_bar
        });
    });

    screen.Loop(renderer);
    return 0;
}
