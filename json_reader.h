#pragma once

#include "json.h"
#include "transport_catalogue.h"

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace json_reader {

enum class RequestType {
    Bus,
    Stop,
    Unknown
};

struct StatRequest {
    int id;                        // id запроса
    json_reader::RequestType type; // Тип запроса, bus - информация о маршруте, stop - информация об остановке
    std::string name;              // Название остановки / маршрута 
};

RequestType GetRequestType (std::string request);


class JsonReader {
public:
    // JsonReader(std::istream input)
    //     : input_(json::Load(input)){
    // };

    JsonReader() = default;

    // Обработка запроса
    void RequestProcessing (trans_cat::TransportCatalogue& catalogue, std::istream& in, std::ostream& out);

    // Заполнение каталога из json файла
    std::pair<std::string, geo::Coordinates> GetStopFromRequest (const json::Dict& request);
    std::tuple<std::string, std::vector<domain::Stop*>, bool> GetBusFromRequest (trans_cat::TransportCatalogue& catalogue, const json::Dict& request);
    void SetDistanceFromRequest (trans_cat::TransportCatalogue& catalogue, json::Array& request);

private:
    //json::Document input_;

    void BaseRequestProcessing (trans_cat::TransportCatalogue& catalogue, const json::Array& request);
    json::Document StatRequestProcessing (trans_cat::TransportCatalogue& catalogue, const std::vector<StatRequest>& stat_request);
    std::vector<domain::Stop*> GetStopsForBusFromRequest (trans_cat::TransportCatalogue& catalogue, const json::Dict& request);
};

} // namespace json_reader
