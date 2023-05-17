#pragma once
#include "geo.h"

#include <optional>
#include <string>
#include <vector>

namespace catalogue {
using Buses = std::optional<std::vector<std::string_view>>;
using Stops = std::vector<std::string_view>;
    
struct Stop {
    Stop();
    Stop(std::string stop_name, geo::Coordinates geo_data);
    
    std::string name;
    geo::Coordinates coordinates;
};
    
struct Bus {
    Bus();
    Bus(std::string bus, std::vector<const Stop*> route, bool roundtrip);
    
    std::string name;
    std::vector<const Stop*> stops;
    bool is_roundtrip;
};
    
struct BusStat {
    int stops_count;
    int unique_stops_count;
    double route_length;
    double real_route_length;
};

} // namespace catalogue