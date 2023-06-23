#pragma once
#include "json.h"
#include "map_renderer.h"
#include "transport_catalogue.h"
#include "transport_router.h"

namespace catalogue {
    
namespace json_rd {
using namespace renderer;
    
class JsonReader {
public:
    JsonReader(TransportCatalogue& catalogue);
    
    void SetRealDistance(const json::Dict& new_stops);
    
    void SetRenderSettings(MapRenderer& renderer, const json::Node& map_settings);
    
    void SetRoutingSettings(TransportRouter& router, const json::Node& base_requests);
    
    void ReadBaseRequests(const json::Node& base_requests);
    
private:
    TransportCatalogue& db_;
    
    struct IdentifiedRequests {
        std::vector<json::Node> new_stops;
        std::vector<json::Node> new_buses;
    };
    
    Stop GetStop(const json::Dict& raw_stop);
    
    Stops GetAllStops(const json::Dict& raw_bus);
   
    Bus GetBus(const json::Dict& raw_bus);
};

} // namespace json_rd
} // namespace catalogue