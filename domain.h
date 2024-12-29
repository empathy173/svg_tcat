#pragma once

#include "geo.h"

#include <string>
#include <vector>
#include <array>
#include <cstdint>
#include <variant>

namespace domain {

struct Stop {
    std::string stop_name;
    geo::Coordinates stop_coord;
};

struct Bus {
    std::string bus_name;
    std::vector<Stop*> stops_for_bus;
    bool is_roundtrip;
};

struct BusStat{
    int all_stop_count 	= 0;
    int uniq_stop_count = 0;
    double route_geo_lenght = 0.0;
    double route_lenght = 0.0;
};

class Rgba {
public:
    Rgba() = default;
    Rgba(uint8_t r, uint8_t g, uint8_t b, double o = 1.0);

    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    double opacity = 1.0;
};
    using Color = std::variant<std::monostate, std::string, domain::Rgba>;

    struct RenderSettings{
    std::vector<Color> color_palette = {};
    std::array<double, 2> bus_label_offset ={};
    std::array<double, 2> stop_label_offset= {};
    Color underlayer_color = {};
    double width = 0;
    double height = 0;
    double padding = 0;
    double line_width = 0;
    double stop_radius = 0;
    double underlayer_width= 0;
    int bus_label_font_size = 0;
    int stop_label_font_size = 0;
};

} // namespace Domain