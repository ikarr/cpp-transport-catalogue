#pragma once
#include "json.h"
#include "request_handler.h"
#include "transport_catalogue.h"

namespace catalogue {
    
namespace json_rd {
using namespace handler;
using namespace renderer;
    
struct IdentifiedRequests {
    std::vector<json::Node> new_stops;
    std::vector<json::Node> new_buses;
};

Stop GetStop(const json::Dict& raw_stop);
    
Stops GetAllStops(const json::Dict& raw_bus);
   
Bus GetBus(TransportCatalogue& catalogue, const json::Dict& raw_bus);
    
void SetRealDistance(TransportCatalogue& catalogue, const json::Dict& new_stops);
    
void SetRenderSettings(MapRenderer& renderer, const json::Node& map_settings);
    
void ReadBaseRequests(TransportCatalogue& catalogue, const json::Node& base_requests);
    
void ReadJSON(std::istream& input,
              std::ostream& out,
              TransportCatalogue& catalogue,
              MapRenderer& renderer,
              RequestHandler& handler);
    
} // namespace json_rd
} // namespace catalogue