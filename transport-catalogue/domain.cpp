#include "domain.h"

namespace catalogue {
    
    Stop::Stop() = default;
    Stop::Stop(std::string stop_name, geo::Coordinates geo_data)
        : name(std::move(stop_name))
        , coordinates(geo_data) {}
    
    Bus::Bus() = default;
    Bus::Bus(std::string bus, std::vector<const Stop*> route, bool roundtrip)
        : name(std::move(bus))
        , stops(std::move(route))
        , is_roundtrip(roundtrip) {}

} // namespace catalogue