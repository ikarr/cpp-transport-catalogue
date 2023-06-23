#pragma once
#include "json_builder.h"
#include "json_reader.h"
#include "map_renderer.h"

namespace catalogue {
namespace handler {
    
using namespace json_rd;
using namespace renderer;
    
class RequestHandler {
public:
    RequestHandler(const TransportCatalogue& db,
                   json::Builder& builder,
                   JsonReader& reader,
                   MapRenderer& renderer,
                   TransportRouter& router);
    
    void MakeResponse(std::ostream& out, const json::Node& stat_requests);
    
    void ReadJSON(std::istream& input, std::ostream& out);
    
    void BuildBusStat(json::Builder& builder, int request_id, const std::optional<BusStat>& bus_stat);
    
    void BuildStopInfo(json::Builder& builder, int request_id, const Buses& buses);
    
    void BuildRenderredMap(json::Builder& builder, int request_id, const std::ostringstream& map);
    
    void InsertRouteItem(json::Builder& builder, const Item& item);
    
    void BuildRoutes(json::Builder& builder, int request_id, const std::optional<RouteItems>& items);
    
    std::ostringstream PrintMap() const;
    
    std::optional<BusStat> GetBusStat(std::string_view bus_name) const;
    
    Buses GetBusesByStop(std::string_view stop) const;
    
    std::optional<RouteItems> GetRouteByStops(std::string_view start_stop, std::string_view finish_stop) const;
    
private:
    const TransportCatalogue& db_;
    json::Builder& builder_;
    JsonReader& json_rd_;
    MapRenderer& renderer_;
    TransportRouter& router_;
};
    
} // namespace handler
} // namespace catalogue