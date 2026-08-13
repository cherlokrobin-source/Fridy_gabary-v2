#ifndef DASHBOARD_LAYOUT_HPP
#define DASHBOARD_LAYOUT_HPP

#include <ftxui/dom/elements.hpp>
#include <string>

using namespace ftxui;

inline Element RenderBottomPanel(int64_t dayId) {
    float horizonPercent = static_cast<float>(dayId) / (50000.0f * 365.2425f);
    if (horizonPercent > 1.0f) horizonPercent = 1.0f;

    return vbox({
        window(text(" 🌌 50,000-YEAR HORIZON POSITION ") | bold | color(Color::Cyan),
            vbox({
                hbox({
                    text("0Y "),
                    gauge(horizonPercent) | color(Color::Cyan),
                    text(" 50,000Y")
                }),
                hbox({
                    text("Current Index: ") | dim,
                    text("#" + std::to_string(dayId)) | color(Color::Green) | bold,
                    text("  |  Horizon Span: ") | dim,
                    text(std::to_string(static_cast<int>(horizonPercent * 100)) + "% Completed") | color(Color::Yellow)
                })
            })
        ),
        window(text(" 🔭 ASTRO DYNAMICS & TEMPORAL PATTERNS ") | bold | color(Color::Green),
            hbox({
                vbox({
                    text("• Equinox/Solstice : Cycle Sync Active") | bold,
                    text("• Lunar Sync Ratio : 1:29.53 Days") | color(Color::Magenta)
                }) | flex,
                separator(),
                vbox({
                    text("• Day Classification: Standard Solar Day"),
                    text("• Core Latency      : < 0.001 ms") | color(Color::Green)
                }) | flex
            })
        ) | flex
    });
}

#endif
