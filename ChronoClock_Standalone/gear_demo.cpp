#include <notcurses/notcurses.h>
#include <cmath>
#include <unistd.h>

int main() {
    struct notcurses_options opts = {};
    struct notcurses* nc = notcurses_init(&opts, NULL);
    if (!nc) return 1;

    struct ncplane* stdplane = notcurses_stdplane(nc);

    unsigned int dimy, dimx;
    ncplane_dim_yx(stdplane, &dimy, &dimx);

    int cy = dimy / 2;
    int cx = dimx / 3;
    double angle = 0.0;

    while (true) {
        ncplane_erase(stdplane);

        // الترس الأول (الذهبي - كبير)
        int teeth1 = 12;
        double r1 = 9.0;
        for (double a = 0; a < 2 * M_PI; a += 0.04) {
            double r = r1 + 2.5 * sin(teeth1 * (a + angle));
            int x = cx + static_cast<int>(r * cos(a) * 2.0);
            int y = cy + static_cast<int>(r * sin(a));

            if (x >= 0 && x < static_cast<int>(dimx) && y >= 0 && y < static_cast<int>(dimy)) {
                ncplane_set_fg_rgb8(stdplane, 255, 215, 0); // لون ذهبي
                ncplane_putchar_yx(stdplane, y, x, '#');
            }
        }

        // الترس الثاني (المتعشق - أزرق - يدور بعكس الاتجاه)
        int cx2 = cx + 28;
        int teeth2 = 8;
        double r2 = 6.0;
        double angle2 = -angle * (12.0 / 8.0); // سرعة عكسية متناسبة

        for (double a = 0; a < 2 * M_PI; a += 0.04) {
            double r = r2 + 2.0 * sin(teeth2 * (a + angle2));
            int x = cx2 + static_cast<int>(r * cos(a) * 2.0);
            int y = cy + static_cast<int>(r * sin(a));

            if (x >= 0 && x < static_cast<int>(dimx) && y >= 0 && y < static_cast<int>(dimy)) {
                ncplane_set_fg_rgb8(stdplane, 0, 191, 255); // لون أزرق سماء
                ncplane_putchar_yx(stdplane, y, x, '*');
            }
        }

        // المحاور المركزية
        ncplane_set_fg_rgb8(stdplane, 255, 255, 255);
        ncplane_putchar_yx(stdplane, cy, cx, 'O');
        ncplane_putchar_yx(stdplane, cy, cx2, 'O');

        notcurses_render(nc);

        angle += 0.06;
        usleep(30000);
    }

    notcurses_stop(nc);
    return 0;
}
