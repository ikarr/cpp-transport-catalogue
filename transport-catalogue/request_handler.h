#pragma once
#include "json.h"
#include "map_renderer.h"
#include "transport_catalogue.h"

namespace catalogue {
    
namespace handler {

class RequestHandler {
public:
    RequestHandler(const TransportCatalogue& db, const renderer::MapRenderer& renderer);
    
    void MakeResponse(std::ostream& out, const json::Node& stat_requests);
    
    json::Dict BuildBusStat(int request_id, const std::optional<BusStat>& bus_stat);
    
    json::Dict BuildStopInfo(int request_id, const Buses& buses);
    
    json::Dict BuildRenderredMap(int request_id, const std::ostringstream& map);
    
    std::ostringstream PrintMap() const;
    
    std::optional<BusStat> GetBusStat(std::string_view bus_name) const;
    
    Buses GetBusesByStop(std::string_view stop) const;
    
private:
    const TransportCatalogue& db_;
    const renderer::MapRenderer& renderer_;
};
    
} // namespace handler
} // namespace catalogue