#pragma once

#include "map_renderer.h"
#include "transport_router.h"

namespace catalogue {
class Serializer {
public:
    Serializer(TransportCatalogue& db, renderer::MapRenderer& renderer, TransportRouter& router);
    
    void SerializeBase();
    
    void DeserializeBase();
    
    void SetSettings(std::string filename);
    
private:
    TransportCatalogue& db_;
    renderer::MapRenderer& renderer_;
    TransportRouter& router_;
    std::string filename_;
};
} // namespace catalogue