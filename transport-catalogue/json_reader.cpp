#include "json_reader.h"

namespace catalogue {
    
namespace json_rd {

Stop GetStop(const json::Dict& raw_stop) {
    Stop stop;
    stop.name = raw_stop.at("name").AsString();
    stop.coordinates = {raw_stop.at("latitude").AsDouble(), raw_stop.at("longitude").AsDouble()};
    return stop;
}

Stops GetAllStops(const json::Dict& raw_bus) {
    Stops stops;
    for (const auto& stop : raw_bus.at("stops").AsArray()) {
        stops.push_back(stop.AsString());
    }
    return stops;
}

Bus GetBus(TransportCatalogue& catalogue, const json::Dict& raw_bus) {
    Bus bus;
    bus.name = raw_bus.at("name").AsString();
    for (auto stop_name : GetAllStops(raw_bus)) {
        bus.stops.push_back(catalogue.GetStop(stop_name));
    }
    bus.is_roundtrip = raw_bus.at("is_roundtrip").AsBool();
    return bus;
}
    
void SetRealDistance(TransportCatalogue& catalogue, const json::Dict& new_stops) {
    std::string stop = new_stops.at("name").AsString();
    const Stop* stop_src = catalogue.GetStop(stop);
    for (const auto& [destination, distance] : new_stops.at("road_distances").AsDict()) {
        const Stop* stop_dst = catalogue.GetStop(destination);
        catalogue.SetDistance(stop_src, stop_dst, distance.AsInt());
    }
}
    
void SetRenderSettings(MapRenderer& renderer, const json::Node& map_settings) {
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

void ReadBaseRequests(TransportCatalogue& catalogue, const json::Node& base_requests) {
    IdentifiedRequests identified;
    for (const auto& element : base_requests.AsArray()) {
        json::Dict request = element.AsDict();
        if (request["type"].AsString() == "Bus") {
            identified.new_buses.push_back(move(request));
        } else if (request["type"].AsString() == "Stop") {
            catalogue.AddStop(GetStop(request));
            identified.new_stops.push_back(move(request));
        }
    }
    for (const auto& new_stop : identified.new_stops) {
        SetRealDistance(catalogue, new_stop.AsDict());
    }
    for (const auto& new_bus : identified.new_buses) {
        catalogue.AddBus(GetBus(catalogue, new_bus.AsDict()));
    }
}

void ReadJSON(std::istream& input,
              std::ostream& out,
              TransportCatalogue& catalogue,
              MapRenderer& renderer,
              RequestHandler& handler) {
    json::Document input_requests = json::Load(input);
    
    for (const auto& [type, requests] : input_requests.GetRoot().AsDict()) {
        if (type == "base_requests") {
            ReadBaseRequests(catalogue, requests);
        } else if (type == "render_settings") {
            SetRenderSettings(renderer, requests);
        } else if (type == "stat_requests") {
            handler.MakeResponse(out, requests);
        }
    }
}

} // namespace json_rd
} // namespace catalogue