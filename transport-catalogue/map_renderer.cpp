#include "map_renderer.h"

namespace catalogue {
    
namespace renderer {
using namespace std::literals;

MapRenderer::MapRenderer() = default;
    
bool AlmostZero(double value) { return std::abs(value) < 1e-6; }

svg::Document MapRenderer::RenderMap(const std::map<std::string_view, const Bus*>& buses) const {
    svg::Document svg_doc;
    auto comp = [](const Stop* lhs, const Stop* rhs) {
        return lhs->name < rhs->name;
    };
    std::set<const Stop*, decltype(comp)> stop_ptr_arr(comp);
    for (const auto& [bus_name, bus_ptr] : buses) {
        stop_ptr_arr.insert(bus_ptr->stops.begin(), bus_ptr->stops.end());
    }
    SphereProjector projector(stop_ptr_arr.begin(),
                              stop_ptr_arr.end(),
                              render_settings_.width,
                              render_settings_.height,
                              render_settings_.padding);
    int route_count = 0;
    for (const auto& [bus_name, bus_ptr] : buses) {
        svg_doc.Add(RenderBusRoute(projector, bus_ptr, route_count));
        ++route_count;
    }
    route_count = 0;

    for (const auto& [bus_name, bus_ptr] : buses) {
        if (bus_ptr->stops.size() == 0) {
            break;
        }
        svg_doc.Add(RenderBusNameUnderlayer(bus_ptr, projector(bus_ptr->stops[0]->coordinates)));
        svg_doc.Add(RenderBusName(bus_ptr, projector(bus_ptr->stops[0]->coordinates), route_count));
        if (bus_ptr->stops[0] != bus_ptr->stops[bus_ptr->stops.size() - 1]) {
            svg_doc.Add(RenderBusNameUnderlayer(
                bus_ptr, projector(bus_ptr->stops[bus_ptr->stops.size() - 1]->coordinates)));
            svg_doc.Add(RenderBusName(
                bus_ptr, projector(bus_ptr->stops[bus_ptr->stops.size() - 1]->coordinates), route_count));
        }
        ++route_count;
    }
    
    for (const Stop* stop_ptr : stop_ptr_arr) {
        svg_doc.Add(RenderStopSymbol(projector(stop_ptr->coordinates)));
    }
    
    for (const Stop* stop_ptr : stop_ptr_arr) {
        svg_doc.Add(RenderStopNameUnderlayer(stop_ptr,  projector(stop_ptr->coordinates)));
        svg_doc.Add(RenderStopName(stop_ptr,  projector(stop_ptr->coordinates)));
        
    }
    return svg_doc;
}
    
svg::Polyline MapRenderer::RenderBusRoute(SphereProjector& proj, const Bus* bus, int color_num) const {
    svg::Polyline route;
    for (const auto& stop : bus->stops) {
        route.AddPoint(proj(stop->coordinates));
    }
    if (!bus->is_roundtrip)  {
        for (int i = static_cast<int>(bus->stops.size() - 2); i >= 0; --i) {
            route.AddPoint(proj(bus->stops[i]->coordinates));
        }
    }
    route.SetFillColor("none")
        .SetStrokeColor(render_settings_.color_palette[color_num % render_settings_.color_palette.size()])
        .SetStrokeWidth(render_settings_.line_width)
        .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
        .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
    return route;
}

svg::Text MapRenderer::RenderBusNameUnderlayer(const Bus* bus, svg::Point pos) const {
    svg::Text name_underlayer;
    name_underlayer.SetPosition(pos)
                .SetOffset(render_settings_.bus_label_offset)
                .SetFontSize(render_settings_.bus_label_font_size)
                .SetFontFamily("Verdana"s)
                .SetFontWeight("bold"s)
                .SetData(bus->name);
    name_underlayer.SetFillColor(render_settings_.underlayer_color)
                .SetStrokeColor(render_settings_.underlayer_color)
                .SetStrokeWidth(render_settings_.underlayer_width)
                .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
    return name_underlayer;
}

svg::Text MapRenderer::RenderStopNameUnderlayer(const Stop* stop, svg::Point pos) const {
    svg::Text name_underlayer;
    name_underlayer.SetPosition(pos)
                .SetOffset(render_settings_.stop_label_offset)
                .SetFontSize(render_settings_.stop_label_font_size)
                .SetFontFamily("Verdana"s)
                .SetData(stop->name);
    name_underlayer.SetFillColor(render_settings_.underlayer_color)
                .SetStrokeColor(render_settings_.underlayer_color)
                .SetStrokeWidth(render_settings_.underlayer_width)
                .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
    return name_underlayer;
}
    
svg::Text MapRenderer::RenderBusName(const Bus* bus, svg::Point pos, int color_num) const {
    svg::Text route_name;
    route_name.SetPosition(pos)
                .SetOffset(render_settings_.bus_label_offset)
                .SetFontSize(render_settings_.bus_label_font_size)
                .SetFontFamily("Verdana"s)
                .SetFontWeight("bold"s)
                .SetData(bus->name);
    route_name.SetFillColor(render_settings_.color_palette[color_num % render_settings_.color_palette.size()]);
    return route_name;
}

svg::Text MapRenderer::RenderStopName(const Stop* stop, svg::Point pos) const {
    svg::Text stop_name;
    stop_name.SetPosition(pos)
                .SetOffset(render_settings_.stop_label_offset)
                .SetFontSize(render_settings_.stop_label_font_size)
                .SetFontFamily("Verdana"s)
                .SetData(stop->name)
                .SetFillColor("black"s);
    return stop_name;
}
    
svg::Circle MapRenderer::RenderStopSymbol(svg::Point pos) const {
    svg::Circle stop_symbol;
    stop_symbol.SetCenter(pos)
                .SetRadius(render_settings_.stop_radius)
                .SetFillColor("white"s);
    return stop_symbol;
}
    
svg::Color MapRenderer::SetColor(const json::Node& raw_color) {
    svg::Color color;
    if (raw_color.IsString()) {
        color = raw_color.AsString();
    } else if (raw_color.AsArray().size() == 3) {
        color = svg::Rgb(raw_color.AsArray()[0].AsDouble(),
                         raw_color.AsArray()[1].AsDouble(),
                         raw_color.AsArray()[2].AsDouble());
    } else if (raw_color.AsArray().size() == 4) {
        color = svg::Rgba(raw_color.AsArray()[0].AsDouble(),
                          raw_color.AsArray()[1].AsDouble(),
                          raw_color.AsArray()[2].AsDouble(),
                          raw_color.AsArray()[3].AsDouble());
    }
    return color;
}

void MapRenderer::SetSettings(RenderSettings settings) {
    render_settings_ = settings;
}
    
RenderSettings MapRenderer::GetSettings() const {
    return render_settings_;
}

} // namespace renderer
} // namespace catalogue