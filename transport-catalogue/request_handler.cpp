#include "request_handler.h"

namespace catalogue {
    
namespace handler {

RequestHandler::RequestHandler(const TransportCatalogue& db, JsonReader& reader, MapRenderer& renderer)
    : db_(db), json_rd_(reader), renderer_(renderer) {}
    
void RequestHandler::MakeResponse(std::ostream& out, const json::Node& stat_requests) {
    json::Array requests_array = stat_requests.AsArray();
    if (stat_requests.AsArray().empty()) { return; }
    
    json::Array response;
    for (const auto& element : requests_array) {
        json::Dict request = element.AsDict();
        if (request["type"].AsString() == "Bus") {
            response.emplace_back(BuildBusStat(request["id"].AsInt(), GetBusStat(request["name"].AsString())));
        } else if (request["type"].AsString() == "Stop") {
            response.emplace_back(BuildStopInfo(request["id"].AsInt(), GetBusesByStop(request["name"].AsString())));
        } else if (request["type"].AsString() == "Map") {
            response.emplace_back(BuildRenderredMap(request["id"].AsInt(), PrintMap()));
        }
    }
    json::Print(json::Document{response}, out);
}

void RequestHandler::ReadJSON(std::istream& input, std::ostream& out) {
    json::Document input_requests = json::Load(input);
    for (const auto& [type, requests] : input_requests.GetRoot().AsDict()) {
        if (type == "base_requests") {
            json_rd_.ReadBaseRequests(requests);
        } else if (type == "render_settings") {
            json_rd_.SetRenderSettings(renderer_, requests);
        } else if (type == "stat_requests") {
            MakeResponse(out, requests);
        }
    }
}
    
json::Dict RequestHandler::BuildBusStat(int request_id, const std::optional<BusStat>& bus_stat) {
    using namespace std::literals;
    json::Dict stat;
    if (bus_stat) {
        stat.emplace("curvature"s, bus_stat.value().real_route_length / bus_stat.value().route_length);
        stat.emplace("request_id"s, request_id);
        stat.emplace("route_length"s, bus_stat.value().real_route_length);
        stat.emplace("stop_count"s, bus_stat.value().stops_count);
        stat.emplace("unique_stop_count"s, bus_stat.value().unique_stops_count);
    } else {
        stat.emplace("request_id"s, request_id);
        stat.emplace("error_message"s, "not found"s);
    }
    return stat;
}

json::Dict RequestHandler::BuildStopInfo(int request_id, const Buses& buses) {
    using namespace std::literals;
    json::Dict stop_info;
    if (buses) {
        json::Array buses_list;
        for (const auto& bus : buses.value()) {
            buses_list.emplace_back(std::string(bus));
        };
        stop_info.emplace("buses"s, buses_list);
        stop_info.emplace("request_id"s, request_id);
    } else {
        stop_info.emplace("request_id"s, request_id);
        stop_info.emplace("error_message"s, "not found"s);
    }
    return stop_info;
}

json::Dict RequestHandler::BuildRenderredMap(int request_id, const std::ostringstream& map) {
    using namespace std::literals;
    json::Dict renderred_map;
    if (map) {
        renderred_map.emplace("map"s, map.str());
        renderred_map.emplace("request_id"s, request_id);
    } else {
        renderred_map.emplace("request_id"s, request_id);
        renderred_map.emplace("error_message"s, "not found"s);
    }
    return renderred_map;
}

std::ostringstream RequestHandler::PrintMap() const {
    std::ostringstream svg;
    svg::Document doc = renderer_.RenderMap(db_.GetAllBuses());
    doc.Render(svg);
    return svg;
}
    
std::optional<BusStat> RequestHandler::GetBusStat(std::string_view bus_name) const {
    const Bus* bus = db_.GetBus(bus_name);
    if (bus != nullptr) {
        return db_.GetBusStat(bus);
    } else { return {}; }
}

Buses RequestHandler::GetBusesByStop(std::string_view stop) const {
    return db_.GetBusesByStop(stop);
}
    
} // namespace handler
} // namespace catalogue