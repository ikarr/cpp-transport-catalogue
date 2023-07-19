#include "serialization.h"
#include "transport_catalogue.pb.h"

#include <fstream>

namespace catalogue {

Serializer::Serializer(TransportCatalogue& db, renderer::MapRenderer& renderer, TransportRouter& router)
    : db_(db), renderer_(renderer), router_(router) {}
    
void Serializer::SetSettings(std::string filename) {
    filename_ = std::move(filename);
}

tcat_serialized::Stop SerializeStop(const Stop* stop_ptr) {
    tcat_serialized::Stop stop;
    stop.set_name(stop_ptr->name);
    stop.mutable_coordinates()->set_lng(stop_ptr->coordinates.lng);
    stop.mutable_coordinates()->set_lat(stop_ptr->coordinates.lat);
    return stop;
}

tcat_serialized::Bus SerializeBus(const Bus* bus_ptr) {
    tcat_serialized::Bus bus;
    bus.set_name(bus_ptr->name);
    bus.set_is_roundtrip(bus_ptr->is_roundtrip);
    for (const Stop* stop_ptr : bus_ptr->stops) {
        bus.add_stops(reinterpret_cast<uint64_t>(stop_ptr));
    }
    return bus;
}

tcat_serialized::Distance SerializeDistance(const Stop* src, const Stop* dst, int64_t distance) {
    tcat_serialized::Distance stop_dist;
    stop_dist.set_src(reinterpret_cast<uint64_t>(src));
    stop_dist.set_dst(reinterpret_cast<uint64_t>(dst));
    stop_dist.set_distance(distance);
    return stop_dist;
}

tcat_serialized::Color SerializeColor(svg::Color raw_color) {
    tcat_serialized::Color color;
    if (std::holds_alternative<std::string>(raw_color)) {
        color.set_string_value(std::get<std::string>(raw_color));
        
    } else if (std::holds_alternative<svg::Rgb>(raw_color)) {
        svg::Rgb rgb_color = std::get<svg::Rgb>(raw_color);
        color.mutable_rgb_value()->set_r(rgb_color.red);
        color.mutable_rgb_value()->set_g(rgb_color.green);
        color.mutable_rgb_value()->set_b(rgb_color.blue);
        
    } else if (std::holds_alternative<svg::Rgba>(raw_color)) {
        svg::Rgba rgb_color = std::get<svg::Rgba>(raw_color);
        color.mutable_rgba_value()->set_r(rgb_color.red);
        color.mutable_rgba_value()->set_g(rgb_color.green);
        color.mutable_rgba_value()->set_b(rgb_color.blue);
        color.mutable_rgba_value()->set_opacity(rgb_color.opacity);
    }
    return color;
}

tcat_serialized::RenderSettings SerializeRenderSettings(renderer::RenderSettings settings) {
    tcat_serialized::RenderSettings render_settings;
    render_settings.set_width(settings.width);
    render_settings.set_height(settings.height);
    render_settings.set_padding(settings.padding);
    render_settings.set_line_width(settings.line_width);
    render_settings.set_stop_radius(settings.stop_radius);
    render_settings.set_bus_label_font_size(settings.bus_label_font_size);
    render_settings.mutable_bus_label_offset()->set_x(settings.bus_label_offset.x);
    render_settings.mutable_bus_label_offset()->set_y(settings.bus_label_offset.y);
    render_settings.set_stop_label_font_size(settings.stop_label_font_size);
    render_settings.mutable_stop_label_offset()->set_x(settings.stop_label_offset.x);
    render_settings.mutable_stop_label_offset()->set_y(settings.stop_label_offset.y);
    *render_settings.mutable_underlayer_color() = SerializeColor(settings.underlayer_color);
    render_settings.set_underlayer_width(settings.underlayer_width);
    for (const auto& color : settings.color_palette) {
        *render_settings.add_color_palette() = SerializeColor(color);
    }
    return render_settings;
}

tcat_serialized::RouterSettings SerializeRouterSettings(RouterSettings settings) {
    tcat_serialized::RouterSettings router_settings;
    router_settings.set_time(settings.time);
    router_settings.set_velocity(settings.velocity);
    return router_settings;
}

Stop DeserializeStop(const tcat_serialized::Stop& stop) {
    return {stop.name(), {stop.coordinates().lat(), stop.coordinates().lng()}};
}

Bus DeserializeBus(const TransportCatalogue& db, tcat_serialized::TransportCatalogue& tcat, const tcat_serialized::Bus& bus) {
    Bus bus_result;
    bus_result.name = bus.name();
    bus_result.is_roundtrip = bus.is_roundtrip();
    for (const auto stops_id : bus.stops()) {
        bus_result.stops.push_back(db.GetStop((*tcat.mutable_stops())[stops_id].name()));
    }
    return bus_result;
    
}

const Stop* GetStopPtr(const TransportCatalogue& db, tcat_serialized::TransportCatalogue& tcat, uint64_t stops_id) {
    return db.GetStop((*tcat.mutable_stops())[stops_id].name());
}

svg::Color DeserializeColor(tcat_serialized::Color color) {
    if (color.has_rgb_value()) {
        svg::Rgb rgb_color;
        rgb_color.red = color.rgb_value().r();
        rgb_color.green = color.rgb_value().g();
        rgb_color.blue = color.rgb_value().b();
        return rgb_color;
    } else if (color.has_rgba_value()) {
        svg::Rgba rgba_color;
        rgba_color.red = color.rgba_value().r();
        rgba_color.green = color.rgba_value().g();
        rgba_color.blue = color.rgba_value().b();
        rgba_color.opacity = color.rgba_value().opacity();
        return rgba_color;
    } else {
        std::string string_color = color.string_value();
        if (string_color.size() != 0) {
            return string_color;
        } else {
            return {};
        }
    }
}

renderer::RenderSettings DeserializeRenderSettings(const tcat_serialized::RenderSettings& settings) {
    renderer::RenderSettings render_settings;
    render_settings.width = settings.width();
    render_settings.height = settings.height();
    render_settings.padding = settings.padding();
    render_settings.line_width = settings.line_width();
    render_settings.stop_radius = settings.stop_radius();
    render_settings.bus_label_font_size = settings.bus_label_font_size();
    render_settings.bus_label_offset = {settings.bus_label_offset().x(),settings.bus_label_offset().y()};
    render_settings.stop_label_font_size = settings.stop_label_font_size();
    render_settings.stop_label_offset = {settings.stop_label_offset().x(),settings.stop_label_offset().y()};
    render_settings.underlayer_color = DeserializeColor(settings.underlayer_color());
    render_settings.underlayer_width = settings.underlayer_width();
    for (const auto& color : settings.color_palette()) {
        render_settings.color_palette.push_back(DeserializeColor(color));
    }
    return render_settings;
}
    
RouterSettings DeserializeRouterSettings(const tcat_serialized::RouterSettings& settings) {
    return {settings.time(), settings.velocity()};
}
    
void Serializer::SerializeBase() {
    std::ofstream output(filename_, std::ios::binary);
    
    tcat_serialized::TransportCatalogue catalogue;
    for (const auto& [name, stop_ptr] : db_.GetAllStops()) {
        (*catalogue.mutable_stops())[reinterpret_cast<uint64_t>(stop_ptr)] = SerializeStop(stop_ptr);
    }
    
    for (const auto& [name, bus_ptr] : db_.GetAllBuses()) {
        *catalogue.add_buses() = SerializeBus(bus_ptr);
    }
    
    for (const auto& [stop_ptrs, distance] : db_.GetAllDistances()) {
        *catalogue.add_distances() = SerializeDistance(stop_ptrs.first, stop_ptrs.second, distance);
    }
    
    *catalogue.mutable_render_settings() = SerializeRenderSettings(renderer_.GetSettings());
    *catalogue.mutable_router_settings() = SerializeRouterSettings(router_.GetSettings());
    
    catalogue.SerializeToOstream(&output);
}

void Serializer::DeserializeBase() {
    std::ifstream input(filename_, std::ios::binary);
    tcat_serialized::TransportCatalogue serialized_catalogue;
    
    serialized_catalogue.ParseFromIstream(&input);
    
    for (const auto& [stop_ptr, stop] : serialized_catalogue.stops()) {
        db_.AddStop(DeserializeStop(stop));
    }
    
    for (const auto& bus : serialized_catalogue.buses()) {
        db_.AddBus(DeserializeBus(db_, serialized_catalogue, bus));
    }
    
    for (const auto& dist : serialized_catalogue.distances()) {
        db_.SetDistance(GetStopPtr(db_, serialized_catalogue, dist.src()),
                                    GetStopPtr(db_, serialized_catalogue, dist.dst()),
                                    dist.distance());
    }
    
    renderer_.SetSettings(DeserializeRenderSettings(serialized_catalogue.render_settings()));
    router_.SetSettings(DeserializeRouterSettings(serialized_catalogue.router_settings()));
}
} // namespace catalogue