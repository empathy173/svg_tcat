#include "transport_catalogue.h"

#include <unordered_set>

// Хеширование дистанции между остановками
size_t trans_cat::TransportCatalogue::DistHasher::operator()(const std::pair<const domain::Stop*, const domain::Stop*>& stops) const {
    size_t h_first_stop = dist_hasher_(stops.first);
    size_t h_sec_stop = dist_hasher_(stops.second);
    return h_first_stop + h_sec_stop * 37;
}

// добавление в БД автобусов и остановок.
void trans_cat::TransportCatalogue::AddBus (const std::string& bus_name, std::vector<domain::Stop*>& stops_for_bus, bool is_roundtrip){
    bus_data_.push_back(domain::Bus{bus_name, stops_for_bus, is_roundtrip});    // Добавление в БД автобуса
    bus_directory_[bus_data_.back().bus_name] = &bus_data_.back();              // Обновление справочника автобуса

    for(const domain::Stop* stop : bus_data_.back().stops_for_bus){
        bus_list_for_stop_[stop->stop_name].insert(bus_data_.back().bus_name);  // Добавление автобуса в справочник для конкретной остановки
    }
}

// Добавление расстояния между двумя остановками
void trans_cat::TransportCatalogue::SetDistBetweenStops(const domain::Stop* stop_from, const domain::Stop* stop_to, size_t distance) {
    std::pair<const domain::Stop*, const domain::Stop*> p = {stop_from, stop_to};
    dist_directory_[p] = distance;
}
	
void trans_cat::TransportCatalogue::AddStop (const std::string& stop_name, geo::Coordinates stop_coord){
    stop_data_.push_back(domain::Stop{stop_name, stop_coord});                  // Добавление в БД остановки
    stop_directory_[stop_data_.back().stop_name] = &stop_data_.back();          // Обновление справочника остановок
    bus_list_for_stop_[stop_data_.back().stop_name] = {};                       // Обновление справочника для конкретной останови
}

// Запросы к БД
domain::Bus* trans_cat::TransportCatalogue::GetBusByName (std::string_view bus_name) const {
    auto it = bus_directory_.find(bus_name);
    
    if(it != bus_directory_.end()){
        return it -> second;
    }

    return nullptr;
}
	
domain::Stop* trans_cat::TransportCatalogue::GetStopByName (std::string_view stop_name) const {
    auto it = stop_directory_.find(stop_name);

    if(it != stop_directory_.end()){
        return it->second;
    }

    return nullptr;
}

domain::BusStat trans_cat::TransportCatalogue::GetBusPropertyByName (std::string_view bus_name) const {
    domain::BusStat res;
    const domain::Bus* bus = GetBusByName(bus_name);

    if(!bus){
        res = {0, 0, 0.0, 0.0};
    } else {
        res.all_stop_count 	= GetBusAllStopCount(*bus);
        res.uniq_stop_count = GetBusUniqStopCount(*bus);

        auto [geo_route_lenght, route_lenght] = GetBusRouteLength(*bus);

        res.route_geo_lenght = geo_route_lenght;
        res.route_lenght = route_lenght;
    }

    // return {GetBusAllStopCount(*bus), GetBusUniqStopCount(*bus), GetBusGeoRouteLength(*bus), GetBusRouteLength(*bus)};
    return res;
}

const std::set<std::string>& trans_cat::TransportCatalogue::GetStopPropertyByName(std::string_view stop_name) const {
    static std::set<std::string> result;
    auto it = bus_list_for_stop_.find(stop_name);

    if(it != bus_list_for_stop_.end()){
        result = it -> second;
    }

    return result;
}

size_t trans_cat::TransportCatalogue::GetDistBetweenStops(const domain::Stop* stop_from, const domain::Stop* stop_to) const {
    // Проверяем прямое направление
    auto it = dist_directory_.find({stop_from, stop_to});
    if (it != dist_directory_.end()) {
        return it->second;
    }

    // Проверяем обратное направление
    it = dist_directory_.find({stop_to, stop_from});
    if (it != dist_directory_.end()) {
        return it->second;
    }

    return 0;
}

int trans_cat::TransportCatalogue::GetBusUniqStopCount(const domain::Bus& bus) const {
    std::unordered_set<std::string> stops;

    for(const auto& stop : bus.stops_for_bus){
        stops.insert(stop->stop_name);
    }

    return static_cast<int>(stops.size());
}

int trans_cat::TransportCatalogue::GetBusAllStopCount(const domain::Bus& bus) const {
    if (bus.is_roundtrip) {
        return static_cast<int>(bus.stops_for_bus.size());
    }
    return static_cast<int>(bus.stops_for_bus.size() * 2 - 1);
}

// double trans_cat::TransportCatalogue::GetBusGeoRouteLength(const domain::Bus& bus) const {
//     double result = 0.0;

//     for(size_t i = 1; i < bus.stops_for_bus.size(); ++i){
//         if (bus.is_roundtrip) {
//             result += geo::ComputeDistance(bus.stops_for_bus[i - 1]->stop_coord, bus.stops_for_bus[i]->stop_coord);
//         } else {
//             result += geo::ComputeDistance(bus.stops_for_bus[i - 1]->stop_coord, bus.stops_for_bus[i]->stop_coord) * 2;
//         }
//     }
//     return result;
// }

// double trans_cat::TransportCatalogue::GetBusRouteLength (const domain::Bus& bus) const {
//     double result = 0.0;

//     for(size_t i = 0; i < bus.stops_for_bus.size() - 1; ++i){
//         const auto stop_from = bus.stops_for_bus[i];
//         const auto stop_to = bus.stops_for_bus[i + 1];
//         if (bus.is_roundtrip) {
//             result += static_cast<double>(GetDistBetweenStops(stop_from, stop_to));
//         } else {
//             result += static_cast<double>(GetDistBetweenStops(stop_from, stop_to) + GetDistBetweenStops(stop_to, stop_from));
//         }
//     }

//     return result;
// } 

std::tuple<double, double> trans_cat::TransportCatalogue::GetBusRouteLength(const domain::Bus& bus) const {
    double geo_route_length = 0.0;
    double route_length = 0.0;

    // Для пустого маршрута или маршрута с одной остановкой
    if (bus.stops_for_bus.empty() || bus.stops_for_bus.size() == 1) {
        return {0.0, 0.0};
    }

    size_t stops_count = bus.stops_for_bus.size();

    // Прямой маршрут
    for (size_t i = 1; i < stops_count; ++i) {
        const domain::Stop* from = bus.stops_for_bus[i - 1];
        const domain::Stop* to = bus.stops_for_bus[i];

        // Географическое расстояние
        geo_route_length += geo::ComputeDistance(from->stop_coord, to->stop_coord);

        // Реальное расстояние
        size_t distance = GetDistBetweenStops(from, to);
        route_length += distance;
    }

    if (!bus.is_roundtrip) {
        // Для некольцевого маршрута добавляем обратный путь
        geo_route_length *= 2;

        // Обратный маршрут
        for (size_t i = stops_count - 1; i > 0; --i) {
            const domain::Stop* from = bus.stops_for_bus[i];
            const domain::Stop* to = bus.stops_for_bus[i - 1];
            size_t distance = GetDistBetweenStops(from, to);
            route_length += distance;
        }
    }

    return {geo_route_length, route_length};
}