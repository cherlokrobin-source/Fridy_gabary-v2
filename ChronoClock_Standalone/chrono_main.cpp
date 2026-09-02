#include <notcurses/notcurses.h>
#include <cmath>
#include <ctime>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <algorithm>

#if __has_include("include/chronology_engine.hpp")
    #include "include/chronology_engine.hpp"
#elif __has_include("include/gabary.hpp")
    #include "include/gabary.hpp"
#endif

void draw_panel_box(struct ncplane* plane, int y, int x, int h, int w, uint32_t rgb, const std::string& title) {
    if (h < 3 || w < 6) return;
    
    ncplane_set_fg_rgb8(plane, (rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF);
    ncplane_putstr_yx(plane, y, x, "┌");
    ncplane_putstr_yx(plane, y, x + w - 1, "┐");
    ncplane_putstr_yx(plane, y + h - 1, x, "└");
    ncplane_putstr_yx(plane, y + h - 1, x + w - 1, "┘");

    for (int i = 1; i < w - 1; ++i) {
        ncplane_putstr_yx(plane, y, x + i, "─");
        ncplane_putstr_yx(plane, y + h - 1, x + i, "─");
    }
    for (int j = 1; j < h - 1; ++j) {
        ncplane_putstr_yx(plane, y + j, x, "│");
        ncplane_putstr_yx(plane, y + j, x + w - 1, "│");
    }

    if (!title.empty() && w > static_cast<int>(title.length() + 4)) {
        ncplane_set_fg_rgb8(plane, 255, 255, 255);
        std::string formatted_title = "┤ " + title + " ├";
        ncplane_putstr_yx(plane, y, x + 2, formatted_title.c_str());
    }
}

void render_horizon_tracker(struct ncplane* plane, int y, int x, int h, int w, long long abs_day, long long lunar_base, bool is_running) {
    std::string title = "🌌 DUAL 50,000Y HORIZON TRACKER " + std::string(is_running ? "[AUTO]" : "[MANUAL]");
    draw_panel_box(plane, y, x, h, w, is_running ? 0xBD93F9 : 0x50FA7B, title);
    if (h < 4) return;

    double solar_horizon_days = 18262125.0;
    double lunar_horizon_days = 17710472.0;

    double solar_pct = std::fmod((static_cast<double>(abs_day) / solar_horizon_days) * 100.0, 100.0);
    double lunar_pct = std::fmod((static_cast<double>(lunar_base + abs_day) / lunar_horizon_days) * 100.0, 100.0);

    auto draw_bar = [&](int row, const std::string& label, double pct, uint32_t color) {
        ncplane_set_fg_rgb8(plane, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
        ncplane_putstr_yx(plane, y + row, x + 2, label.c_str());
        
        int bar_x = x + 2 + label.length();
        int max_bar_w = w - label.length() - 14;
        if (max_bar_w > 4) {
            ncplane_putstr_yx(plane, y + row, bar_x, "[");
            int filled = static_cast<int>((pct / 100.0) * (max_bar_w - 2));
            filled = std::clamp(filled, 0, max_bar_w - 2);
            for (int b = 0; b < max_bar_w - 2; ++b) {
                ncplane_putstr_yx(plane, y + row, bar_x + 1 + b, (b < filled) ? "█" : "░");
            }
            ncplane_putstr_yx(plane, y + row, bar_x + max_bar_w - 1, "]");
            
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << pct << "%";
            ncplane_set_fg_rgb8(plane, 241, 250, 140);
            ncplane_putstr_yx(plane, y + row, bar_x + max_bar_w + 1, ss.str().c_str());
        }
    };

    draw_bar(1, "🌙 Lunar (17.71M d): ", lunar_pct, 0x8BE9FD);
    draw_bar(2, "☀️ Solar (18.26M d): ", solar_pct, 0xF1FA8C);
}

void render_calendar_matrix(struct ncplane* plane, int y, int x, int h, int w, uint32_t color, const std::string& title, int max_days, int current_day) {
    draw_panel_box(plane, y, x, h, w, color, title);
    if (h < 5 || w < 20) return;

    ncplane_set_fg_rgb8(plane, 139, 233, 253);
    ncplane_putstr_yx(plane, y + 1, x + 2, "Fr Sa Su Mo Tu We Th");

    int row = y + 2;
    int col = x + 2;
    int day_counter = 1;

    for (int i = 0; i < 35 && day_counter <= max_days; ++i) {
        int r = row + (i / 7);
        int c = col + (i % 7) * 3;
        if (r >= y + h - 1) break;

        if (day_counter == current_day) {
            ncplane_set_fg_rgb8(plane, 0, 0, 0);
            ncplane_set_bg_rgb8(plane, 80, 250, 123);
        } else {
            ncplane_set_fg_rgb8(plane, 248, 248, 242);
            ncplane_set_bg_default(plane);
        }

        std::stringstream ss;
        ss << std::setw(2) << day_counter;
        ncplane_putstr_yx(plane, r, c, ss.str().c_str());
        day_counter++;
    }
    ncplane_set_bg_default(plane);
}

void render_metrics_panel(struct ncplane* plane, int y, int x, int h, int w, double diurnal, double annual, double epoch, double horizon, double lunar_cycle, double grand_cyc) {
    draw_panel_box(plane, y, x, h, w, 0x50FA7B, "⚙️ REAL-TIME CHRONO VECTOR METRICS");
    if (h < 4) return;

    auto print_row = [&](int line, uint32_t color, const std::string& label, double ratio) {
        if (y + line >= y + h - 1) return;
        std::stringstream ss;
        ss << std::left << std::setw(12) << label
           << std::right << std::setw(5) << std::fixed << std::setprecision(1) << (ratio * 360.0) << "° "
           << std::right << std::setw(5) << std::setprecision(3) << ratio << " ";
        
        ncplane_set_fg_rgb8(plane, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
        ncplane_putstr_yx(plane, y + line, x + 2, ss.str().c_str());

        int rem_w = w - ss.str().length() - 4;
        if (rem_w > 4) {
            std::string bar = "[";
            int filled = static_cast<int>(ratio * (rem_w - 2));
            for (int b = 0; b < rem_w - 2; ++b) {
                bar += (b < filled) ? "█" : "░";
            }
            bar += "]";
            ncplane_putstr_yx(plane, y + line, x + 2 + ss.str().length(), bar.c_str());
        }
    };

    print_row(1, 0x50FA7B, "DIURNAL [A]", diurnal);
    print_row(2, 0xF1FA8C, "ANNUAL  [S]", annual);
    print_row(3, 0xFFB86C, "EPOCH   [M]", epoch);
    print_row(4, 0xBD93F9, "HORIZON [H]", horizon);
    print_row(5, 0x8BE9FD, "LUNAR-D [☾]", lunar_cycle);
    print_row(6, 0xFF79C6, "GRAND-C [G]", grand_cyc);
}

void render_wave_panel(struct ncplane* plane, int y, int x, int h, int w, double diurnal, double lunar_cycle) {
    draw_panel_box(plane, y, x, h, w, 0xFF79C6, "🌊 TEMPORAL RESONANCE WAVE");
    if (h < 3) return;

    int max_pts = w - 4;
    double amplitude = (h - 2) / 2.0;
    double mid_y = y + 1 + amplitude;

    ncplane_set_fg_rgb8(plane, 80, 250, 150);

    for (int i = 0; i < max_pts; ++i) {
        double norm_i = static_cast<double>(i) / max_pts;
        double wave_val = std::sin((norm_i + diurnal) * 2.0 * M_PI) * std::cos((norm_i + lunar_cycle) * M_PI);
        int wy = static_cast<int>(mid_y - (wave_val * amplitude));
        wy = std::clamp(wy, y + 1, y + h - 2);

        ncplane_putstr_yx(plane, wy, x + 2 + i, "■");
    }
}

int main() {
    struct notcurses_options opts = {};
    struct notcurses* nc = notcurses_init(&opts, NULL);
    if (!nc) return 1;
    struct ncplane* stdplane = notcurses_stdplane(nc);

    long long abs_day = 18894871; 
    long long lunar_base = 17250176;
    bool is_running = false; // افتراضياً متوقف (تحكم يدو بالكامل)

    const char* days_of_week[] = {"Friday", "Saturday", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday"};

    while (true) {
        ncplane_erase(stdplane);

        struct winsize w_size;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w_size);
        unsigned int dimy = w_size.ws_row > 0 ? w_size.ws_row : ncplane_dim_y(stdplane);
        unsigned int dimx = w_size.ws_col > 0 ? w_size.ws_col : ncplane_dim_x(stdplane);

        ncplane_resize_simple(stdplane, dimy, dimx);

        draw_panel_box(stdplane, 0, 0, dimy, dimx, 0x6272A4, "GABARY V2 :: MASTER CHRONO DASHBOARD");

        // 1. Horizon Tracker
        render_horizon_tracker(stdplane, 1, 1, 4, dimx - 2, abs_day, lunar_base, is_running);

        // 2. Dual Calendar Matrices
        int matrix_w = (dimx - 3) / 2;
        int matrix_h = 8;

        int sol_day = (abs_day % 31) + 1;
        int lun_day = ((lunar_base + abs_day) % 30) + 1;

        render_calendar_matrix(stdplane, 5, 1, matrix_h, matrix_w, 0x50FA7B, "📅 MONTHLY (1-1)", 31, sol_day);
        render_calendar_matrix(stdplane, 5, 1 + matrix_w, matrix_h, dimx - 2 - matrix_w, 0x8BE9FD, "🌙 LUNAR (1-1)", 30, lun_day);

        // 3. Chrono Metrics Table
        double diurnal     = std::fmod(static_cast<double>(abs_day), 1.0);
        double annual      = std::fmod(static_cast<double>(abs_day) / 365.2422, 1.0);
        double epoch       = std::fmod(static_cast<double>(abs_day) / 3652.422, 1.0);
        double horizon     = std::fmod(static_cast<double>(abs_day) / 50000.0, 1.0);
        double lunar_cycle = std::fmod(static_cast<double>(lunar_base + abs_day) / 29.53058886, 1.0);
        double grand_cyc   = std::fmod(epoch * 1.6180339887, 1.0);

        int metrics_y = 5 + matrix_h;
        int metrics_h = 8;
        render_metrics_panel(stdplane, metrics_y, 1, metrics_h, dimx - 2, diurnal, annual, epoch, horizon, lunar_cycle, grand_cyc);

        // 4. Resonance Wave
        int wave_y = metrics_y + metrics_h;
        int wave_h = std::max(4, static_cast<int>(dimy - wave_y - 1));
        render_wave_panel(stdplane, wave_y, 1, wave_h, dimx - 2, diurnal, lunar_cycle);

        // Status Bar
        ncplane_set_fg_rgb8(stdplane, 98, 114, 164);
        std::stringstream status_ss;
        status_ss << days_of_week[abs_day % 7] << " | ABS_DAY: #" << abs_day << " | ARROWS: Move Day/Month";
        std::string status = status_ss.str();
        ncplane_putstr_yx(stdplane, dimy - 1, std::max(1, static_cast<int>(dimx - status.length()) / 2), status.c_str());

        notcurses_render(nc);

        // لا توجد زيادة تلقائية إلا إذا قمت بتمكين is_running عبر المسافة
        if (is_running) {
            abs_day++;
        }

        ncinput ni;
        uint32_t key = notcurses_get_nblock(nc, &ni);
        if (key != 0 && key != (uint32_t)-1) {
            if (ni.evtype == NCTYPE_PRESS) {
                if (key == ' ') { is_running = !is_running; }
                else if (key == NCKEY_UP) { abs_day += 1; }
                else if (key == NCKEY_DOWN) { abs_day -= 1; }
                else if (key == NCKEY_RIGHT) { abs_day += 30; }
                else if (key == NCKEY_LEFT) { abs_day -= 30; }
                else if (key == NCKEY_PGUP) { abs_day += 365; }
                else if (key == NCKEY_PGDOWN) { abs_day -= 365; }
                else if (key == 'q' || key == 'Q' || key == NCKEY_ESC) { break; }
            }
        }

        usleep(100000);
    }

    notcurses_stop(nc);
    return 0;
}
