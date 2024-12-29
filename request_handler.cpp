
#include "request_handler.h"
#include <iostream>
/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

namespace req_handl {

// Получение типа запроса

//json::Document RequestHandler::StatRequestProcessing (trans_cat::TransportCatalogue& catalogue, const std::vector<StatRequest>& stat_request) {
// void RequestHandler::StatRequestProcessing () {
//     using namespace std::literals;
//     json::Array result;

//     for (const auto& query : stat_request_.AsArray()) {
//         int cmd_id = query.AsMap().at("id").AsInt();
//         json_reader::RequestType cmd_type = json_reader::GetRequestType(query.AsMap().at("type").AsString());
//         std::string cmd_name = query.AsMap().at("name").AsString();
        
//         comand_requests_.push_back({cmd_id, cmd_type, cmd_name});
//     }
//     // json::Document responce = StatRequestProcessing (catalogue, comand_requests_);
//     // json::Print(responce, out);        
    
//     //json::Array result;

//     for (size_t i = 0; i < comand_requests_.size(); ++i) {
//         json::Dict dict;
//         dict["request_id"] = comand_requests_[i].id;

//         switch (comand_requests_[i].type) {
//             case json_reader::RequestType::Stop : {
//                 const auto& stop = catalogue_.GetStopByName(comand_requests_[i].name);
//                 json::Array buses;

//                 if (stop != nullptr) {
//                     for (const auto& bus_name : catalogue_.GetStopPropertyByName(stop->stop_name)) {
//                         buses.push_back(json::Node(bus_name));
//                     }
//                     dict["buses"] = buses;
//                 } else {
//                     dict["error_message"]     = "not dound"s;
//                 }
//                 break;
//             }
//             case json_reader::RequestType::Bus : {
//                 const auto& bus = catalogue_.GetBusByName(comand_requests_[i].name);
                
//                 if (bus != nullptr) {
//                     domain::BusStat bus_property = catalogue_.GetBusPropertyByName(bus->bus_name);
//                     dict["curvature"]         = bus_property.route_lenght / bus_property.route_geo_lenght;
//                     dict["route_length"]      = bus_property.route_lenght;
//                     dict["stop_count"]        = bus_property.all_stop_count;
//                     dict["unique_stop_count"] = bus_property.uniq_stop_count;
//                 } else {
//                     dict["error_message"]     = "not dound"s;
//                 }
//                 break;
//             }
//             case json_reader::RequestType::Unknown : {
//                 break;
//             }
//         }
//         result.push_back(dict);
//     }
//     json::Print(json::Document(result), std::cout);
// }

} // namespace req_handl