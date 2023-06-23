#include "json_reader.h"

namespace catalogue {
namespace json_rd {

JsonReader::JsonReader(TransportCatalogue& catalogue) : db_(catalogue) {}
    
Stop JsonReader::GetStop(const json::Dict& raw_stop) {
    Stop stop;
    stop.name = raw_stop.at("name").AsString();
    stop.coordinates = {raw_stop.at("latitude").AsDouble(), raw_stop.at("longitude").AsDouble()};
    return stop;
}

Stops JsonReader::GetAllStops(const json::Dict& raw_bus) {
    Stops stops;
    for (const auto& stop : raw_bus.at("stops").AsArray()) {
        stops.push_back(stop.AsString());
    }
    return stops;
}

Bus JsonReader::GetBus(const json::Dict& raw_bus) {
    Bus bus;
    bus.name = raw_bus.at("name").AsString();
    for (auto stop_name : GetAllStops(raw_bus)) {
        bus.stops.push_back(db_.GetStop(stop_name));
    }
    bus.is_roundtrip = raw_bus.at("is_roundtrip").AsBool();
    return bus;
}
    
void JsonReader::SetRealDistance(const json::Dict& new_stops) {
    std::string stop = new_stops.at("name").AsString();
    const Stop* stop_src = db_.GetStop(stop);
    for (const auto& [destination, distance] : new_stops.at("road_distances").AsDict()) {
        const Stop* stop_dst = db_.GetStop(destination);
        db_.SetDistance(stop_src, stop_dst, distance.AsInt());
    }
}
    
void JsonReader::SetRenderSettings(renderer::MapRenderer& renderer, const json::Node& map_settings) {
    json::Dict raw_settings = map_settings.AsDict();
    std::vector<svg::Color> color_palette;
    for (const auto& raw_color: raw_settings.at("color_palette").AsArray()) {
        color_palette.push_back(renderer.SetColor(raw_color));
    }
    RenderSettings render_settings{
        raw_settings.at("width").AsDouble(),
        raw_settings.at("height").AsDouble(),
        raw_settings.at("padding").AsDouble(),
        raw_settings.at("line_width").AsDouble(),
        raw_settings.at("stop_radius").AsDouble(),
        raw_settings.at("bus_label_font_size").AsInt(),
        {raw_settings.at("bus_label_offset").AsArray()[0].AsDouble()
            , raw_settings.at("bus_label_offset").AsArray()[1].AsDouble()},
        raw_settings.at("stop_label_font_size").AsInt(),
        {raw_settings.at("stop_label_offset").AsArray()[0].AsDouble()
            , raw_settings.at("stop_label_offset").AsArray()[1].AsDouble()},
        renderer.SetColor(raw_settings.at("underlayer_color")),
        raw_settings.at("underlayer_width").AsDouble(),
        color_palette
    };
    renderer.SetSettings(render_settings);
}

void JsonReader::SetRoutingSettings(TransportRouter& router, const json::Node& base_requests) {
    json::Dict routing_settings = base_requests.AsDict();
    router.SetSettings({routing_settings.at("bus_wait_time").AsInt(),
                        routing_settings.at("bus_velocity").AsDouble()});
}
    
void JsonReader::ReadBaseRequests(const json::Node& base_requests) {
    IdentifiedRequests identified;
    for (const auto& element : base_requests.AsArray()) {
        json::Dict request = element.AsDict();
        if (request["type"].AsString() == "Bus") {
            identified.new_buses.push_back(move(request));
        } else if (request["type"].AsString() == "Stop") {
            db_.AddStop(GetStop(request));
            identified.new_stops.push_back(move(request));
        }
    }
    for (const auto& new_stop : identified.new_stops) {
        SetRealDistance(new_stop.AsDict());
    }
    for (const auto& new_bus : identified.new_buses) {
        db_.AddBus(GetBus(new_bus.AsDict()));
    }
}

} // namespace json_rd
} // namespace catalogue