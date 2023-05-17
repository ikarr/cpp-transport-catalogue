#pragma once
#include "json_reader.h"
#include "map_renderer.h"

namespace catalogue {
    
namespace handler {
using namespace json_rd;
using namespace renderer;
    
class RequestHandler {
public:
    RequestHandler(const TransportCatalogue& db, JsonReader& reader, MapRenderer& renderer);
    
    void MakeResponse(std::ostream& out, const json::Node& stat_requests);
    
    void ReadJSON(std::istream& input, std::ostream& out);
    
    json::Dict BuildBusStat(int request_id, const std::optional<BusStat>& bus_stat);
    
    json::Dict BuildStopInfo(int request_id, const Buses& buses);
    
    json::Dict BuildRenderredMap(int request_id, const std::ostringstream& map);
    
    std::ostringstream PrintMap() const;
    
    std::optional<BusStat> GetBusStat(std::string_view bus_name) const;
    
    Buses GetBusesByStop(std::string_view stop) const;
    
private:
    const TransportCatalogue& db_;
    JsonReader& json_rd_;
    MapRenderer& renderer_;
};
    
} // namespace handler
} // namespace catalogue