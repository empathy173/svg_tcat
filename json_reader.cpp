#include "json_reader.h"
#include "request_handler.h"

#include <tuple>

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

namespace json_reader {

RequestType GetRequestType (std::string request){
    if(request == "Stop"){
        return RequestType::Stop;
    } else if (request == "Bus"){
        return RequestType::Bus;
    }
    return RequestType::Unknown;
}

// Получение остановки из запроса
std::pair<std::string, geo::Coordinates> JsonReader::GetStopFromRequest (const json::Dict& request) {
    std::string stop_name = request.at("name").AsString();
    geo::Coordinates stop_coord = {request.at("latitude").AsDouble(), request.at("longitude").AsDouble()};
    return std::make_pair(stop_name, stop_coord);
}

// Получение списка остановок для маршрута
std::vector<domain::Stop*> JsonReader::GetStopsForBusFromRequest (trans_cat::TransportCatalogue& catalogue, const json::Dict& request) {
    std::vector<domain::Stop*> result;
    
    for (const auto& stop : request.at("stops").AsArray()) {
        result.push_back(catalogue.GetStopByName(stop.AsString()));
    }
    return result;
}

// Получение автобуса из запроса
std::tuple<std::string, std::vector<domain::Stop*>, bool> JsonReader::GetBusFromRequest (trans_cat::TransportCatalogue& catalogue, const json::Dict& request) {
    std::string bus_name = request.at("name").AsString();
    std::vector<domain::Stop*> stops_for_bus;

    for (const auto& stop : GetStopsForBusFromRequest(catalogue, request)) {
        stops_for_bus.push_back(stop);
    }
    
    bool is_roundtrip = request.at("is_roundtrip").AsBool();
    return std::make_tuple(bus_name, stops_for_bus, is_roundtrip);
}

// Заполнение справочника расстояний между остановками dist_directory_
    void JsonReader::SetDistanceFromRequest(trans_cat::TransportCatalogue& catalogue, json::Array& request) {
        for (const auto& stops_dict : request) {
            const std::string& stop_from = stops_dict.AsMap().at("name").AsString();
            const auto& distances = stops_dict.AsMap().at("road_distances").AsMap();

            for (const auto& [stop_to, distance] : distances) {
                size_t dist = static_cast<size_t>(distance.AsInt());
                catalogue.SetDistBetweenStops(
                        catalogue.GetStopByName(stop_from),
                        catalogue.GetStopByName(stop_to),
                        dist
                );
            }
        }
    }

// Обработчик запроса base_request
void JsonReader::BaseRequestProcessing (trans_cat::TransportCatalogue& catalogue, const json::Array& request) {
    std::vector<json::Node> bus_buffer;
    std::vector<json::Node> stop_buffer;
    
    for(const auto& node : request){
        switch (GetRequestType(node.AsMap().at("type").AsString())){
            case RequestType::Stop : {
                std::pair<std::string, geo::Coordinates> stop = GetStopFromRequest(node.AsMap());
                stop_buffer.push_back(node);
                catalogue.AddStop(stop.first, stop.second);
                break;
            }
            case RequestType::Bus : {
                bus_buffer.push_back(node);
                break;
            }
            case RequestType::Unknown : {
                break;
            }
        }
    }
    SetDistanceFromRequest(catalogue, stop_buffer);

    for(const auto& bus_node : bus_buffer){
        auto [bus_name, stops_for_bus, is_roundtrip] = GetBusFromRequest(catalogue, bus_node.AsMap());
        catalogue.AddBus(bus_name, stops_for_bus, is_roundtrip);
    }
}

json::Document JsonReader::StatRequestProcessing (trans_cat::TransportCatalogue& catalogue, const std::vector<StatRequest>& stat_request) {
//void JsonReader::StatRequestProcessing () {
    using namespace std::literals;
    json::Array result;

    // for (const auto& query : stat_request_.AsArray()) {
    //     int cmd_id = query.AsMap().at("id").AsInt();
    //     json_reader::RequestType cmd_type = json_reader::GetRequestType(query.AsMap().at("type").AsString());
    //     std::string cmd_name = query.AsMap().at("name").AsString();
        
    //     comand_requests_.push_back({cmd_id, cmd_type, cmd_name});
    // }
    // json::Document responce = StatRequestProcessing (catalogue, comand_requests_);
    // json::Print(responce, out);        
    
    //json::Array result;

    for (size_t i = 0; i < stat_request.size(); ++i) {
        json::Dict dict;
        dict["request_id"] = stat_request[i].id;

        switch (stat_request[i].type) {
            case json_reader::RequestType::Stop : {
                const auto& stop = catalogue.GetStopByName(stat_request[i].name);
                json::Array buses;

                if (stop != nullptr) {
                    for (const auto& bus_name : catalogue.GetStopPropertyByName(stop->stop_name)) {
                        buses.push_back(json::Node(bus_name));
                    }
                    dict["buses"] = buses;
                } else {
                    dict["error_message"]     = "not found"s;
                }
                break;
            }
            case json_reader::RequestType::Bus : {
                const auto& bus = catalogue.GetBusByName(stat_request[i].name);
                
                if (bus != nullptr) {
                    domain::BusStat bus_property = catalogue.GetBusPropertyByName(bus->bus_name);
                    dict["curvature"]         = bus_property.route_lenght / bus_property.route_geo_lenght;
                    dict["route_length"]      = bus_property.route_lenght;
                    dict["stop_count"]        = bus_property.all_stop_count;
                    dict["unique_stop_count"] = bus_property.uniq_stop_count;
                } else {
                    dict["error_message"]     = "not found"s;
                }
                break;
            }
            case json_reader::RequestType::Unknown : {
                break;
            }
        }
        result.push_back(dict);
    }
    return json::Document(result);
}

void JsonReader::RequestProcessing (trans_cat::TransportCatalogue& catalogue, std::istream& in, std::ostream& out) {
    using namespace std::literals;
    json::Document doc = json::Load(in);
    
    for (const auto& request : doc.GetRoot().AsMap()) {
        if (request.first == "base_requests") {
            BaseRequestProcessing(catalogue, request.second.AsArray());
        }

        if (request.first == "stat_requests") {
            std::vector<StatRequest> stat_request;
            for (const auto& query : request.second.AsArray()) {
                int cmd_id = query.AsMap().at("id").AsInt();
                RequestType cmd_type = GetRequestType(query.AsMap().at("type").AsString());
                std::string cmd_name = query.AsMap().at("name").AsString();
                stat_request.push_back({cmd_id, cmd_type, cmd_name});
            }
            json::Document responce = StatRequestProcessing (catalogue, stat_request);
            json::Print(responce, out);        
        }
    }
}

} //namespace json_reader