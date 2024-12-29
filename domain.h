#pragma once

/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области (domain)
 * вашего приложения и не зависят от транспортного справочника. Например Автобусные маршруты и Остановки. 
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
 * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
 *
 */
#include "geo.h"

#include <string>
#include <vector>

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

} // namespace Domain