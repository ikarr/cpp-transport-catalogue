#include "request_handler.h"

namespace catalogue {
namespace handler {

RequestHandler::RequestHandler(const TransportCatalogue& db,
                               json::Builder& builder,
                               JsonReader& reader,
                               MapRenderer& renderer)
    : db_(db), builder_(builder), json_rd_(reader), renderer_(renderer) {}
    
void RequestHandler::MakeResponse(std::ostream& out, const json::Node& stat_requests) {
    json::Array requests_array = stat_requests.AsArray();
    if (stat_requests.AsArray().empty()) { return; }
    
    json::Builder builder;
    builder.StartArray();
    for (const auto& element : requests_array) {
        json::Dict request = element.AsDict();
        if (request["type"].AsString() == "Bus") {
            BuildBusStat(builder, request["id"].AsInt(), GetBusStat(request["name"].AsString()));
        } else if (request["type"].AsString() == "Stop") {
            BuildStopInfo(builder, request["id"].AsInt(), GetBusesByStop(request["name"].AsString()));
        } else if (request["type"].AsString() == "Map") {
            BuildRenderredMap(builder, request["id"].AsInt(), PrintMap());
        }
    }
    builder.EndArray();
    json::Print(json::Document{builder.Build()}, out);
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
    
void RequestHandler::BuildBusStat(json::Builder& builder, int request_id, const std::optional<BusStat>& bus_stat) {
    using namespace std::literals;
    if (bus_stat) {
        builder.StartDict()
            .Key("curvature"s).Value(bus_stat.value().real_route_length / bus_stat.value().route_length)
            .Key("request_id"s).Value(request_id)
            .Key("route_length"s).Value(bus_stat.value().real_route_length)
            .Key("stop_count"s).Value(bus_stat.value().stops_count)
            .Key("unique_stop_count"s).Value(bus_stat.value().unique_stops_count)
            .EndDict();
    } else {
        builder.StartDict()
            .Key("request_id"s).Value(request_id)
            .Key("error_message"s).Value("not found"s)
            .EndDict();
    }
}

void RequestHandler::BuildStopInfo(json::Builder& builder, int request_id, const Buses& buses) {
    using namespace std::literals;
    if (buses) {
        builder.StartDict();
        builder.Key("buses"s);
        builder.StartArray();
        for (const auto& bus : buses.value()) {
            builder.Value(std::string(bus));
        }
        builder.EndArray();
        builder.Key("request_id"s).Value(request_id);
        builder.EndDict();
    } else {
        builder.StartDict()
            .Key("request_id"s).Value(request_id)
            .Key("error_message"s).Value("not found"s)
            .EndDict();
    }
}

void RequestHandler::BuildRenderredMap(json::Builder& builder, int request_id, const std::ostringstream& map) {
    using namespace std::literals;
    if (map) {
        builder.StartDict()
            .Key("map"s).Value(map.str())
            .Key("request_id"s).Value(request_id)
            .EndDict();
    } else {
        builder.StartDict()
        .Key("request_id"s).Value(request_id)
        .Key("error_message"s).Value("not found"s)
        .EndDict();
    }
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