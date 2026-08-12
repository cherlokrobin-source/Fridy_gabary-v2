#ifndef CONSOLE_APP_H
#define CONSOLE_APP_H

#include <string>
#include <cstdint>
#include "SolarEngineV2.h"
#include "TemporalEngineV2.h"
#include "GlobalSolarDay.h"

class ConsoleApp {
public:
    ConsoleApp() = default;
    int run();

    void showBanner() const;
    void showDay(int64_t dayId);
    void shareDay(int64_t dayId);
    void exportDay(int64_t dayId);
    void inspectDay(int64_t dayId);

private:
    Gabary::SolarEngineV2 solarEngine;
};

#endif // CONSOLE_APP_H
