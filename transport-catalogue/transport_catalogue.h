#pragma once

#include <algorithm>
#include <deque>
#include <map>
#include <string>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "geo.h"

namespace transport_catalogue {
namespace detail {

template <typename T>
struct PairHasher {
    std::hash<T> hasher;
    
    size_t operator()(const std::pair<T, T> pair_to_hash) const {
        return hasher(pair_to_hash.first) + hasher(pair_to_hash.second) * 37;
    }
};
    
} // end of namespace detail

class TransportCatalogue {
public:
    struct Stop {
    std::string name;
    Coordinates coordinates;
    };

    struct Bus {
    std::string name;
    std::vector<const Stop*> stops;
    bool route_is_circular;
    };

    struct RouteInfo {
    size_t stops_count;
    size_t unique_stops_count;
    double route_length;
    double real_route_length;
    };
    
    void AddStop(const Stop& stop);
    
    void AddBus(const Bus& bus);
    
    void SetDistanceBetweenStops(const Stop* src, const Stop* dst, int64_t distance);
    
    const Stop* GetStopByName(std::string_view stop_name) const;
    
    const Bus* GetBusByName(std::string_view bus_name) const;
    
    std::optional<std::vector<std::string_view>>  GetBusesByStopName(std::string_view stop_name) const;
    
    int64_t GetDistance(const Stop* src, const Stop* dst) const;
    
    RouteInfo GetRouteInfoByBus(const Bus* bus) const;
    
    const std::unordered_map<std::string_view, const Stop*>& GetAllStops() const;
    
private:
    std::deque<Bus> buses_;
    std::deque<Stop> stops_;
    std::map<std::string_view, const Bus*> pointers_to_buses_;
    std::unordered_map<std::string_view, const Stop*> pointers_to_stops_;
    std::unordered_map<std::pair<const Stop*, const Stop*>, int64_t, detail::PairHasher<const Stop*>> stops_distance_;
};

} // end of namespace transport_catalogue