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

constexpr long long MAX_SOLAR_DAYS = 1825250;
constexpr long long MAX_LUNAR_DAYS = 1725050;

struct SolarLunarCoords {
    double solar_longitude;
    double lunar_longitude;
    double lunar_phase;
    double sync_coefficient;
    double julian_day;
};

class ChronologyEngine {
public:
    static SolarLunarCoords computeVectorAccurate(long long solar_abs_day) {
        SolarLunarCoords coords;

        constexpr double LUNAR_CYCLE_DAYS = 11697.85; 
        double cycle_progress = std::fmod(static_cast<double>(solar_abs_day), LUNAR_CYCLE_DAYS) / LUNAR_CYCLE_DAYS;
        if (cycle_progress < 0) cycle_progress += 1.0;

        constexpr double MEAN_LUNAR_MONTH = 29.53058886;
        double total_lunar_months = static_cast<double>(solar_abs_day) / MEAN_LUNAR_MONTH;
        
        coords.lunar_phase = std::fmod(total_lunar_months, 1.0);
        if (coords.lunar_phase < 0) coords.lunar_phase += 1.0;
        coords.lunar_longitude = coords.lunar_phase * 360.0;

        constexpr double TROPICAL_YEAR = 365.2422;
        double total_solar_years = static_cast<double>(solar_abs_day) / TROPICAL_YEAR;
        coords.solar_longitude = std::fmod(total_solar_years * 360.0, 360.0);
        if (coords.solar_longitude < 0) coords.solar_longitude += 360.0;

        double sync_diff = std::abs(coords.solar_longitude - coords.lunar_longitude);
        if (sync_diff > 180.0) sync_diff = 360.0 - sync_diff;
        coords.sync_coefficient = (180.0 - sync_diff) / 180.0;

        coords.julian_day = 2451545.0 + solar_abs_day;
        return coords;
    }
};

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

void render_horizon_tracker(struct ncplane* plane, int y, int x, int h, int w, long long cur_solar_day, long long cur_lunar_day, bool is_running) {
    std::stringstream title_ss;
    title_ss << "🌌 DUAL DYNAMIC HORIZON TRACKER " << (is_running ? "[AUTO]" : "[MANUAL]");
    draw_panel_box(plane, y, x, h, w, is_running ? 0xBD93F9 : 0x50FA7B, title_ss.str());
    if (h < 4) return;

    double solar_pct = std::fmod((static_cast<double>(cur_solar_day) / 365.2422) * 100.0, 100.0);
    double lunar_pct = std::fmod((static_cast<double>(cur_lunar_day) / 354.367) * 100.0, 100.0);

    if (solar_pct == 0.0 && cur_solar_day > 0) solar_pct = 100.0;
    if (lunar_pct == 0.0 && cur_lunar_day > 0) lunar_pct = 100.0;

    auto draw_bar = [&](int row, const std::string& label, double pct, uint32_t color) {
        ncplane_set_fg_rgb8(plane, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
        ncplane_putstr_yx(plane, y + row, x + 2, label.c_str());
        
        int bar_x = x + 2 + label.length();
        int max_bar_w = w - label.length() - 16;
        if (max_bar_w > 4) {
            ncplane_putstr_yx(plane, y + row, bar_x, "[");
            int filled = std::max(1, static_cast<int>((pct / 100.0) * (max_bar_w - 2)));
            filled = std::clamp(filled, 1, max_bar_w - 2);
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

    std::stringstream l_lbl, s_lbl;
    l_lbl << "🌙 Lunar (" << cur_lunar_day << "/" << MAX_LUNAR_DAYS << "d): ";
    s_lbl << "☀️ Solar (" << cur_solar_day << "/" << MAX_SOLAR_DAYS << "d): ";

    draw_bar(1, l_lbl.str(), lunar_pct, 0x8BE9FD);
    draw_bar(2, s_lbl.str(), solar_pct, 0xF1FA8C);
}

void render_calendar_matrix(struct ncplane* plane, int y, int x, int h, int w, uint32_t color, const std::string& type_name, int year_num, int month_num, int current_day, int max_days) {
    std::stringstream title_ss;
    title_ss << type_name << " (Y:" << year_num << " M:" << month_num << " D:" << current_day << ")";
    draw_panel_box(plane, y, x, h, w, color, title_ss.str());
    if (h < 5 || w < 20) return;

    int pad_x = std::max(2, (w - 20) / 2);

    ncplane_set_fg_rgb8(plane, 139, 233, 253);
    ncplane_putstr_yx(plane, y + 1, x + pad_x, "Fr Sa Su Mo Tu We Th");

    int row = y + 2;
    int col = x + pad_x;
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

void render_metrics_panel(struct ncplane* plane, int y, int x, int h, int w, const SolarLunarCoords& coords, double diurnal, double horizon, long long s_day, long long l_day) {
    std::stringstream title_ss;
    title_ss << "⚙️ DYNAMIC VECTOR METRICS [SOLAR: #" << s_day << " | LUNAR: #" << l_day << "]";
    draw_panel_box(plane, y, x, h, w, 0x50FA7B, title_ss.str());
    if (h < 4) return;

    auto print_row = [&](int line, uint32_t color, const std::string& label, double deg, double ratio) {
        if (y + line >= y + h - 1) return;
        
        ratio = std::clamp(ratio, 0.0, 1.0);
        if (ratio == 0.0) ratio = 0.05;

        std::stringstream ss;
        ss << std::left << std::setw(12) << label
           << std::right << std::setw(5) << std::fixed << std::setprecision(1) << deg << "° "
           << std::right << std::setw(5) << std::setprecision(3) << ratio << " ";
        
        ncplane_set_fg_rgb8(plane, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
        ncplane_putstr_yx(plane, y + line, x + 2, ss.str().c_str());

        int rem_w = w - ss.str().length() - 4;
        if (rem_w > 4) {
            std::string bar = "[";
            int filled = static_cast<int>(ratio * (rem_w - 2));
            filled = std::clamp(filled, 1, rem_w - 2);
            for (int b = 0; b < rem_w - 2; ++b) {
                bar += (b < filled) ? "█" : "░";
            }
            bar += "]";
            ncplane_putstr_yx(plane, y + line, x + 2 + ss.str().length(), bar.c_str());
        }
    };

    double annual_ratio = coords.solar_longitude / 360.0;
    double lunar_ratio  = coords.lunar_phase;
    double horizon_ratio = static_cast<double>(s_day) / MAX_SOLAR_DAYS;

    print_row(1, 0x50FA7B, "DIURNAL [A]", diurnal * 360.0, diurnal);
    print_row(2, 0xF1FA8C, "SOL-LONG [S]", coords.solar_longitude, annual_ratio);
    print_row(3, 0xFFB86C, "LUN-LONG [M]", coords.lunar_longitude, coords.lunar_longitude / 360.0);
    print_row(4, 0xBD93F9, "HORIZON [H]", horizon * 360.0, horizon_ratio);
    print_row(5, 0x8BE9FD, "LUN-PHASE[☾]", coords.lunar_phase * 360.0, lunar_ratio);
    print_row(6, 0xFF79C6, "SYNC-COEF[G]", coords.sync_coefficient * 180.0, coords.sync_coefficient);
}

void render_dual_line_gauge(struct ncplane* plane, int y, int x, int h, int w, const SolarLunarCoords& coords) {
    draw_panel_box(plane, y, x, h, w, 0x50FA7B, "🎛️ ASTRONOMICAL VECTOR OSCILLOSCOPE");
    if (h < 5 || w < 10) return;

    int inner_x = x + 1;
    int inner_y = y + 1;
    int inner_w = w - 2;
    int inner_h = h - 2;

    int mid_y = inner_y + inner_h / 2;
    double amplitude = (inner_h - 2) / 2.0;

    ncplane_set_fg_rgb8(plane, 60, 64, 92);
    for (int r = 0; r < inner_h; ++r) {
        for (int c = 0; c < inner_w; ++c) {
            if (inner_y + r == mid_y) {
                ncplane_putstr_yx(plane, inner_y + r, inner_x + c, "─");
            } else if (c == inner_w / 2) {
                ncplane_putstr_yx(plane, inner_y + r, inner_x + c, "│");
            } else if (r % 2 == 0 && c % 6 == 0) {
                ncplane_putstr_yx(plane, inner_y + r, inner_x + c, "·");
            }
        }
    }
    ncplane_putstr_yx(plane, mid_y, inner_x + inner_w / 2, "┼");

    double sol_rad = coords.solar_longitude * M_PI / 180.0;
    double lun_rad = coords.lunar_longitude * M_PI / 180.0;

    std::vector<int> solar_y(inner_w);
    std::vector<int> lunar_y(inner_w);

    for (int c = 0; c < inner_w; ++c) {
        double norm_c = static_cast<double>(c) / inner_w;
        double wave_s = std::sin(sol_rad + (norm_c * 2.0 * M_PI));
        double wave_l = std::sin(lun_rad + (norm_c * 2.0 * M_PI));

        int wy_s = static_cast<int>(mid_y - (wave_s * amplitude));
        int wy_l = static_cast<int>(mid_y - (wave_l * amplitude));

        solar_y[c] = std::clamp(wy_s, inner_y, inner_y + inner_h - 1);
        lunar_y[c] = std::clamp(wy_l, inner_y, inner_y + inner_h - 1);
    }

    ncplane_set_fg_rgb8(plane, 139, 233, 253);
    for (int c = 0; c < inner_w; ++c) {
        int py = lunar_y[c];
        int prev_y = (c > 0) ? lunar_y[c - 1] : py;
        const char* symbol = (c % 2 == 0) ? "·" : " ";
        if (py < prev_y) symbol = "◜";
        else if (py > prev_y) symbol = "◟";
        ncplane_putstr_yx(plane, py, inner_x + c, symbol);
    }

    ncplane_set_fg_rgb8(plane, 80, 250, 123);
    for (int c = 0; c < inner_w; ++c) {
        int py = solar_y[c];
        int prev_y = (c > 0) ? solar_y[c - 1] : py;
        const char* symbol = "═";
        if (py < prev_y) symbol = "╱";
        else if (py > prev_y) symbol = "╲";

        if (std::abs(py - lunar_y[c]) <= 1) {
            ncplane_set_fg_rgb8(plane, 255, 184, 108);
            symbol = "╪";
        } else {
            ncplane_set_fg_rgb8(plane, 80, 250, 123);
        }
        ncplane_putstr_yx(plane, py, inner_x + c, symbol);
    }

    ncplane_set_fg_rgb8(plane, 80, 250, 123);
    ncplane_putstr_yx(plane, inner_y, inner_x + 1, "═ Sol");
    ncplane_set_fg_rgb8(plane, 139, 233, 253);
    ncplane_putstr_yx(plane, inner_y, inner_x + 8, "· Lun");
    ncplane_set_fg_rgb8(plane, 255, 184, 108);
    ncplane_putstr_yx(plane, inner_y, inner_x + 15, "╪ Sync");
}

int main() {
    struct notcurses_options opts = {};
    struct notcurses* nc = notcurses_init(&opts, NULL);
    if (!nc) return 1;
    struct ncplane* stdplane = notcurses_stdplane(nc);

    long long solar_abs_day = 1048;
    long long lunar_abs_day = 1048;
    bool is_running = false;

    const char* days_of_week[] = {"Fri", "Sat", "Sun", "Mon", "Tue", "Wed", "Thu"};

    while (true) {
        ncplane_erase(stdplane);

        struct winsize w_size;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w_size);
        unsigned int dimy = w_size.ws_row > 0 ? w_size.ws_row : ncplane_dim_y(stdplane);
        unsigned int dimx = w_size.ws_col > 0 ? w_size.ws_col : ncplane_dim_x(stdplane);

        ncplane_resize_simple(stdplane, dimy, dimx);

        int solar_day   = ((solar_abs_day - 1) % 31) + 1;
        int solar_month = (((solar_abs_day - 1) / 31) % 12) + 1;
        int solar_year  = ((solar_abs_day - 1) / 372) + 1;

        int lunar_day   = ((lunar_abs_day - 1) % 30) + 1;
        int lunar_month = (((lunar_abs_day - 1) / 30) % 12) + 1;
        int lunar_year  = ((lunar_abs_day - 1) / 360) + 1;

        draw_panel_box(stdplane, 0, 0, dimy, dimx, 0x6272A4, "GABARY V2 :: DYNAMIC CHRONO MEASUREMENT DASHBOARD");

        render_horizon_tracker(stdplane, 1, 1, 4, dimx - 2, solar_abs_day, lunar_abs_day, is_running);

        int matrix_w = (dimx - 3) / 2;
        int matrix_h = 8;
        render_calendar_matrix(stdplane, 5, 1, matrix_h, matrix_w, 0x50FA7B, "📅 SOLAR", solar_year, solar_month, solar_day, 31);
        render_calendar_matrix(stdplane, 5, 1 + matrix_w, matrix_h, dimx - 2 - matrix_w, 0x8BE9FD, "🌙 LUNAR", lunar_year, lunar_month, lunar_day, 30);

        auto vector_coords = ChronologyEngine::computeVectorAccurate(solar_abs_day);

        double diurnal = std::fmod(static_cast<double>(solar_abs_day) / 31.0, 1.0);
        double horizon = static_cast<double>(solar_abs_day) / MAX_SOLAR_DAYS;

        int metrics_y = 5 + matrix_h;
        int metrics_h = 8;
        render_metrics_panel(stdplane, metrics_y, 1, metrics_h, dimx - 2, vector_coords, diurnal, horizon, solar_abs_day, lunar_abs_day);

        int gauge_y = metrics_y + metrics_h;
        int gauge_h = std::max(5, static_cast<int>(dimy - gauge_y - 1));
        render_dual_line_gauge(stdplane, gauge_y, 1, gauge_h, dimx - 2, vector_coords);

        // الشريط السفلي المحسّن للمساحات المحدودة
        std::stringstream green_info_ss;
        green_info_ss << "🗓️ " << days_of_week[(solar_abs_day - 1) % 7] 
                      << " S:Y" << solar_year << "-M" << solar_month << "-D" << solar_day 
                      << " L:Y" << lunar_year << "-M" << lunar_month << "-D" << lunar_day 
                      << " SYNC:" << std::fixed << std::setprecision(2) << vector_coords.sync_coefficient;

        std::string green_str = green_info_ss.str();
        std::string red_str   = " | dev: nemimeche benaissa";

        // اقتصاص نص البيانات لو كانت الشاشة ضيقة جدا لضمان ظهور التوقيع دائماً
        int max_green_len = std::max(5, static_cast<int>(dimx) - static_cast<int>(red_str.length()) - 3);
        if (static_cast<int>(green_str.length()) > max_green_len) {
            green_str = green_str.substr(0, max_green_len);
        }

        // طباعة البيانات باللون الأخضر
        ncplane_set_fg_rgb8(stdplane, 80, 250, 123);
        ncplane_putstr_yx(stdplane, dimy - 1, 1, green_str.c_str());

        // طباعة التوقيع مباشرة بعدها باللون الأحمر
        ncplane_set_fg_rgb8(stdplane, 255, 85, 85);
        ncplane_putstr_yx(stdplane, dimy - 1, 1 + green_str.length(), red_str.c_str());

        notcurses_render(nc);

        if (is_running) {
            if (solar_abs_day < MAX_SOLAR_DAYS) solar_abs_day++;
            if (lunar_abs_day < MAX_LUNAR_DAYS) lunar_abs_day++;
        }

        ncinput ni;
        uint32_t key = notcurses_get_nblock(nc, &ni);
        if (key != 0 && key != (uint32_t)-1) {
            if (key == NCKEY_UP || key == 'w' || key == 'W' || key == 'k' || key == 'K') { 
                if (solar_abs_day < MAX_SOLAR_DAYS) solar_abs_day++;
                if (lunar_abs_day < MAX_LUNAR_DAYS) lunar_abs_day++;
            }
            else if (key == NCKEY_DOWN || key == 's' || key == 'S' || key == 'j' || key == 'J') { 
                if (solar_abs_day > 1) solar_abs_day--;
                if (lunar_abs_day > 1) lunar_abs_day--;
            }
            else if (key == NCKEY_RIGHT || key == 'd' || key == 'D' || key == 'l' || key == 'L') { 
                solar_abs_day = std::min(solar_abs_day + 30, MAX_SOLAR_DAYS);
                lunar_abs_day = std::min(lunar_abs_day + 30, MAX_LUNAR_DAYS);
            }
            else if (key == NCKEY_LEFT || key == 'a' || key == 'A' || key == 'h' || key == 'H') { 
                solar_abs_day = std::max(solar_abs_day - 30, 1LL);
                lunar_abs_day = std::max(lunar_abs_day - 30, 1LL);
            }
            else if (key == NCKEY_PGUP || key == 'u' || key == 'U') { 
                solar_abs_day = std::min(solar_abs_day + 372, MAX_SOLAR_DAYS);
                lunar_abs_day = std::min(lunar_abs_day + 360, MAX_LUNAR_DAYS);
            }
            else if (key == NCKEY_PGDOWN || key == 'n' || key == 'N') { 
                solar_abs_day = std::max(solar_abs_day - 372, 1LL);
                lunar_abs_day = std::max(lunar_abs_day - 360, 1LL);
            }
            else if (key == ' ') { is_running = !is_running; }
            else if (key == 'q' || key == 'Q' || key == NCKEY_ESC) { break; }
        }

        usleep(80000);
    }

    notcurses_stop(nc);
    return 0;
}
