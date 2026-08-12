#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <thread>

#include "GlobalSolarDay.h"
#include "SolarEngineV2.h"

using namespace ftxui;

enum ThemeMode { GOLDEN, CYBERPUNK, MATRIX };
ThemeMode current_theme = GOLDEN;

// إرجاع لون العنوان حسب الثيم
Color get_primary_color() {
  if (current_theme == CYBERPUNK) return Color::Magenta;
  if (current_theme == MATRIX) return Color::Green;
  return Color::Yellow; // GOLDEN
}

Color get_accent_color() {
  if (current_theme == CYBERPUNK) return Color::Cyan;
  if (current_theme == MATRIX) return Color::GreenLight;
  return Color::Cyan;
}

// دالة الوقت الحالي
std::string get_current_time_string() {
  auto now = std::chrono::system_clock::now();
  auto in_time_t = std::chrono::system_clock::to_time_t(now);
  std::stringstream ss;
  ss << std::put_time(std::localtime(&in_time_t), "%H:%M:%S");
  return ss.str();
}

// دالة رسم الكرة الأرضية المتحركة (Animated Globe)
Element build_animated_globe(int frame) {
  static const std::vector<std::vector<std::string>> globes = {
      {"     .---.     ", "   /  .-.  \\   ", "  |  ( o )  |  ", "  |   `-'   |  ", "   \\       /   ", "     `---'     "},
      {"     .---.     ", "   /  .-.  \\   ", "  |  (  o)  |  ", "  |   `-'   |  ", "   \\       /   ", "     `---'     "},
      {"     .---.     ", "   /  .-.  \\   ", "  |  (   )  |  ", "  |  (o)    |  ", "   \\       /   ", "     `---'     "},
      {"     .---.     ", "   /  .-.  \\   ", "  |  (o  )  |  ", "  |   `-'   |  ", "   \\       /   ", "     `--- me'  "}
  };
  
  auto current_frame = globes[frame % globes.size()];
  Elements lines;
  for (const auto& l : current_frame) {
    lines.push_back(text(l) | color(get_accent_color()));
  }
  return vbox(std::move(lines)) | center;
}

// حساب الفصل الفلكي
std::string get_season_name(int solar_month) {
  if (solar_month >= 1 && solar_month <= 3) return "🌱 Spring (الربيع)";
  if (solar_month >= 4 && solar_month <= 6) return "☀️ Summer (الصيف)";
  if (solar_month >= 7 && solar_month <= 9) return "🍂 Autumn (الخريف)";
  return "❄️ Winter (الشتاء)";
}

// دالة بناء شبكة التقويم الشهري
Element build_monthly_grid(const Gabary::GlobalSolarDay& current_day, Gabary::SolarEngineV2& engine) {
  int64_t days_into_month = current_day.solarDay - 1;
  int64_t month_start_id = current_day.dayId - days_into_month;

  Elements header_row = {
      text(" Fr ") | bold | color(get_accent_color()),
      text(" Sa ") | bold | color(get_accent_color()),
      text(" Su ") | bold | color(get_accent_color()),
      text(" Mo ") | bold | color(get_accent_color()),
      text(" Tu ") | bold | color(get_accent_color()),
      text(" We ") | bold | color(get_accent_color()),
      text(" Th ") | bold | color(get_accent_color()),
  };

  std::vector<Elements> grid_rows;
  grid_rows.push_back(header_row);

  Elements current_row;
  int start_weekday_idx = static_cast<int>((month_start_id - 1) % 7);

  for (int i = 0; i < start_weekday_idx; ++i) {
    current_row.push_back(text("   "));
  }

  for (int day = 1; day <= 31; ++day) {
    int64_t target_id = month_start_id + (day - 1);
    if (target_id > 1825650) break;
    
    Gabary::GlobalSolarDay day_info = engine.buildDay(target_id);
    if (day_info.solarMonth != current_day.solarMonth) break;

    std::string day_str = (day < 10 ? " " : "") + std::to_string(day) + " ";
    
    if (day == current_day.solarDay) {
      current_row.push_back(text(day_str) | bold | bgcolor(Color::Green) | color(Color::Black));
    } else {
      current_row.push_back(text(day_str) | color(Color::White));
    }

    if (current_row.size() == 7) {
      grid_rows.push_back(current_row);
      current_row.clear();
    }
  }

  if (!current_row.empty()) {
    while (current_row.size() < 7) {
      current_row.push_back(text("   "));
    }
    grid_rows.push_back(current_row);
  }

  Elements final_table;
  for (auto& row : grid_rows) {
    final_table.push_back(hbox(std::move(row)));
  }

  std::string month_title = " MONTHLY GRID (" + std::to_string(current_day.solarYear) + "-" + std::to_string(current_day.solarMonth) + ") ";

  return window(
      text(month_title) | bold | color(get_primary_color()),
      vbox(std::move(final_table)) | center
  );
}

// دالة رسم اللوحة الهندسية
Element build_geometric_charts(const Gabary::GlobalSolarDay& active_day) {
  int approx_day_in_year = static_cast<int>((active_day.solarMonth - 1) * 30 + active_day.solarDay);
  double days_in_year = active_day.leapYear ? 366.0 : 365.0;
  double year_progress = (double)approx_day_in_year / days_in_year;
  if (year_progress > 1.0) year_progress = 1.0;

  double month_progress = (double)active_day.solarDay / 30.0;
  if (month_progress > 1.0) month_progress = 1.0;

  double global_horizon = (double)active_day.dayId / 1825650.0;
  int millennium = (active_day.solarYear - 1) / 1000 + 1;

  return window(
      text(" GEOMETRIC & ASTRO METRICS ") | bold | color(Color::GreenLight),
      vbox({
          hbox({
              text(" Year Progress  : ") | color(Color::White),
              gauge(year_progress) | color(get_accent_color()),
              text(" " + std::to_string((int)(year_progress * 100)) + "%") | bold | color(get_accent_color())
          }),
          hbox({
              text(" Month Progress : ") | color(Color::White),
              gauge(month_progress) | color(Color::Green),
              text(" " + std::to_string((int)(month_progress * 100)) + "%") | bold | color(Color::Green)
          }),
          hbox({
              text(" 50K Horizon    : ") | color(Color::White),
              gauge(global_horizon) | color(get_primary_color()),
              text(" " + std::to_string((int)(global_horizon * 100)) + "%") | bold | color(get_primary_color())
          }),
          separator(),
          vbox({
              hbox({
                  text(" Season Phase: ") | color(Color::GrayDark),
                  text(get_season_name(active_day.solarMonth)) | bold | color(Color::Magenta),
                  filler(),
                  text("Millennium  : ") | color(Color::GrayDark),
                  text("M" + std::to_string(millennium) + " / 50") | bold | color(get_primary_color())
              }),
              hbox({
                  text(" Day Status  : ") | color(Color::GrayDark),
                  text("Day " + std::to_string(approx_day_in_year) + " of " + std::to_string((int)days_in_year)) | color(Color::BlueLight),
                  filler(),
                  text("Leap Type   : ") | color(Color::GrayDark),
                  text(active_day.leapYear ? "366 Days (Leap)" : "365 Days (Norm)") | bold | color(active_day.leapYear ? Color::Green : Color::White)
              })
          })
      })
  );
}

// دالة التوقيع وتفاصيل المحرك
Element build_signature_footer(const Gabary::GlobalSolarDay& active_day) {
  int day_of_week_num = static_cast<int>((active_day.dayId - 1) % 7) + 1;

  return window(
      text(" ADVANCED DAY COORDINATES & ARCHITECTURE ") | bold | color(Color::MagentaLight),
      vbox({
          hbox({
              text(" ABSOLUTE ID  : ") | bold | color(Color::White),
              text("#" + std::to_string(active_day.dayId) + " / 1825650") | bold | color(get_accent_color()),
              filler(),
              text("WEEKDAY INDEX : ") | bold | color(Color::White),
              text("Day " + std::to_string(day_of_week_num) + " (" + active_day.weekName + ")") | color(Color::Green)
          }),
          hbox({
              text(" ENGINE Core  : ") | bold | color(Color::White),
              text("Gabary SolarEngineV2 [Precision: 100%]") | color(get_primary_color()),
              filler(),
              text("LATENCY       : ") | bold | color(Color::White),
              text("< 0.001 ms") | color(get_accent_color())
          }),
          separator(),
          hbox({
              text(" ENGINEER     : ") | bold | color(Color::Green),
              text("Nemimeche Benaissa") | bold | color(Color::White),
              filler(),
              text("PROJECT       : ") | bold | color(Color::Magenta),
              text("Gabary V2 TUI System") | color(get_primary_color())
          })
      })
  );
}

// تصدير الشامل بعدة صيغ (TXT / JSON / CSV)
std::string export_all_formats(const Gabary::GlobalSolarDay& active_day) {
  // TXT
  std::ofstream txt("export_report.txt");
  txt << "=== GABARY V2 REPORT ===\nID: " << active_day.dayId 
      << "\nDate: " << active_day.solarYear << "-" << active_day.solarMonth << "-" << active_day.solarDay 
      << "\nDay: " << active_day.weekName << "\nEngineer: Nemimeche Benaissa\n";
  txt.close();

  // JSON
  std::ofstream json("export_report.json");
  json << "{\n  \"dayId\": " << active_day.dayId 
       << ",\n  \"solarYear\": " << active_day.solarYear 
       << ",\n  \"solarMonth\": " << active_day.solarMonth 
       << ",\n  \"solarDay\": " << active_day.solarDay 
       << ",\n  \"weekName\": \"" << active_day.weekName << "\""
       << ",\n  \"engineer\": \"Nemimeche Benaissa\"\n}\n";
  json.close();

  // CSV
  std::ofstream csv("export_report.csv");
  csv << "DayID,SolarYear,SolarMonth,SolarDay,WeekName,Engineer\n";
  csv << active_day.dayId << "," << active_day.solarYear << "," << active_day.solarMonth << "," 
      << active_day.solarDay << "," << active_day.weekName << ",Nemimeche Benaissa\n";
  csv.close();

  return "💾 EXPORT COMPLETE: Saved (.txt, .json, .csv)";
}

std::string process_user_request(const std::string& input, Gabary::SolarEngineV2& engine, Gabary::GlobalSolarDay& out_day) {
  if (input.empty()) return "Waiting for input...";
  std::stringstream ss(input); std::string tag; ss >> tag;

  int64_t target_id = 1;

  if (tag == "date" || (isdigit(tag[0]) && input.find(' ') != std::string::npos)) {
      int64_t y = 0, m = 0, d = 0;
      if (tag == "date") ss >> y >> m >> d; else { std::stringstream full_ss(input); full_ss >> y >> m >> d; }
      target_id = engine.toDayId(y, m, d);
  } else {
      try {
        target_id = std::stoll(input);
      } catch (...) { return "⚠️ ERROR: Invalid ID."; }
  }

  if (target_id < 1 || target_id > 1825650) return "⚠️ ERROR: Out of bounds [1 - 1,825,650]";

  out_day = engine.buildDay(target_id);
  return "🟢 NAVIGATION SUCCESS:\n• ID: " + std::to_string(out_day.dayId) + 
         "\n• Date: " + std::to_string(out_day.solarYear) + "-" + std::to_string(out_day.solarMonth) + "-" + std::to_string(out_day.solarDay) + 
         "\n• Day: " + out_day.weekName;
}

int main() {
  Gabary::SolarEngineV2 engine;
  auto screen = ScreenInteractive::Fullscreen();
  std::string command_input = "";
  int anim_frame = 0;
  
  Gabary::GlobalSolarDay active_day = engine.buildDay(21582);
  std::string output_display = process_user_request("21582", engine, active_day);
  
  auto update_out = [&](std::string val) { 
    output_display = process_user_request(val, engine, active_day); 
  };

  auto navigate_by_offset = [&](int64_t offset) {
    int64_t next_id = active_day.dayId + offset;
    if (next_id < 1) next_id = 1;
    if (next_id > 1825650) next_id = 1825650;
    update_out(std::to_string(next_id));
  };
  
  auto nav_c = Container::Vertical({
      Button(" [⏮ DAY 1   ] ", [&] { update_out("1"); }),
      Button(" [⏪ -10,000Y] ", [&] { navigate_by_offset(-3652425); }),
      Button(" [🎯 MID 25K ] ", [&] { update_out("912825"); }),
      Button(" [⏩ +10,000Y] ", [&] { navigate_by_offset(3652425); }),
      Button(" [⏭ DAY MAX ] ", [&] { update_out("1825650"); }),
      Button(" [💾 EXPORT ] ", [&] { output_display = export_all_formats(active_day); })
  });
  
  auto input_f = Input(&command_input, "Enter Day ID or YYYY MM DD...");
  auto main_c = Container::Vertical({nav_c, input_f});

  // خيط الانيميشن والتحديث
  std::thread refresh_thread([&] {
    while (true) {
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      anim_frame++;
      screen.PostEvent(Event::Custom);
    }
  });
  refresh_thread.detach();

  auto renderer = Renderer(main_c, [&] {
    auto grid_view = build_monthly_grid(active_day, engine);
    auto charts_view = build_geometric_charts(active_day);
    auto signature_view = build_signature_footer(active_day);
    auto earth_view = build_animated_globe(anim_frame);
    std::string time_str = get_current_time_string();

    return vbox({
        text(">>> GABARY V2 TEMPORAL DASHBOARD <<<") | bold | color(get_primary_color()) | center,
        hbox({
            vbox({
                window(text(" TIMELINE ") | color(get_primary_color()), nav_c->Render() | size(WIDTH, EQUAL, 18)),
                window(text(" GLOBE ") | color(Color::Green), earth_view | size(WIDTH, EQUAL, 18)),
                window(text(" LIVE TIME ") | color(get_accent_color()), text(time_str) | bold | color(get_primary_color()) | center | size(WIDTH, EQUAL, 18))
            }),
            vbox({
                window(text(" CONSOLE ") | color(Color::Magenta), paragraph(output_display)),
                grid_view,
                charts_view,
                signature_view,
                window(text(" INPUT ") | color(Color::Green), input_f->Render()),
                text(" ⬅️/➡️: Day  |  ⬆️/⬇️: Month  |  F2: Switch Theme ") | dim | center
            }) | flex
        })
    });
  });

  auto event_handler = CatchEvent(renderer, [&](Event event) {
    if (event == Event::F2) {
      if (current_theme == GOLDEN) current_theme = CYBERPUNK;
      else if (current_theme == CYBERPUNK) current_theme = MATRIX;
      else current_theme = GOLDEN;
      return true;
    }
    if (event == Event::ArrowLeft) { navigate_by_offset(-1); return true; }
    if (event == Event::ArrowRight) { navigate_by_offset(1); return true; }
    if (event == Event::ArrowUp) { navigate_by_offset(-30); return true; }
    if (event == Event::ArrowDown) { navigate_by_offset(30); return true; }
    if (event == Event::PageUp) { navigate_by_offset(-365); return true; }
    if (event == Event::PageDown) { navigate_by_offset(365); return true; }
    if (event == Event::Home) { update_out("1"); return true; }
    if (event == Event::End) { update_out("1825650"); return true; }
    return false;
  });

  screen.Loop(event_handler);
  return 0;
}
