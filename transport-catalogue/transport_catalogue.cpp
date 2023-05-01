#include "transport_catalogue.h"

namespace transport_catalogue {
using namespace std;

void TransportCatalogue::AddStop(Stop stop) {
    stops_.push_back(stop);
    pointers_to_stops_[stops_.back().name] = &stops_.back();
}

void TransportCatalogue::AddBus(Bus bus) {
    buses_.push_back(bus);
    pointers_to_buses_[buses_.back().name] = &buses_.back();
}
    
void TransportCatalogue::SetDistanceBetweenStops(const Stop* src, const Stop* dst, int64_t distance) {
    stops_distance_[{src, dst}] = distance;
}

std::optional<std::vector<std::string_view>> TransportCatalogue::GetBusesByStopName(string_view stop_name) const {
    if (GetStopByName(stop_name) == nullptr) { return {}; }
    vector<string_view> res;
    for_each (pointers_to_buses_.begin(), pointers_to_buses_.end(),
            [stop_name, &res](const auto value) {
                for (const auto stop : value.second->stops) {
                    if (stop->name == stop_name) {
                        res.push_back(value.second->name);
                        break;
                    }
                }
            });
    return res;
}

const TransportCatalogue::Stop* TransportCatalogue::GetStopByName(std::string_view stop_name) const {
    if (pointers_to_stops_.count(stop_name) != 0) {
        return pointers_to_stops_.at(stop_name);
    } else {
        return nullptr;
    }
}

const TransportCatalogue::Bus* TransportCatalogue::GetBusByName(std::string_view bus_name) const {
    if (pointers_to_buses_.count(bus_name) != 0) {
        return pointers_to_buses_.at(bus_name);
    } else {
        return nullptr;
    }
}

int64_t TransportCatalogue::GetDistance(const Stop* src, const Stop* dst) const {
    if (stops_distance_.count({src, dst}) != 0) {
        return stops_distance_.at({src, dst});
    } else {
        return stops_distance_.at({dst, src});
    }
}
    
TransportCatalogue::RouteInfo TransportCatalogue::GetRouteInfoByBus(const Bus* bus) const {
    RouteInfo route_info;
    route_info.route_length = 0;
    route_info.real_route_length = 0;
    vector<const Stop*> stops;
    stops.insert(stops.begin(), bus->stops.begin(), bus->stops.end());
    
    if (bus->route_is_circular)  {
        for (size_t i = (stops.size() - 2); i > 0; --i) {
            stops.push_back(stops[i]);
        }
    }
    
    unordered_set<const Stop*> unique_stops(stops.begin(), stops.end());
    route_info.stops_count = stops.size();
    route_info.unique_stops_count = unique_stops.size();
    
    for (size_t i = 0; i < (stops.size() - 1); ++i) {
        route_info.route_length += ComputeDistance(stops[i]->coordinates, stops[i + 1]->coordinates);
        route_info.real_route_length += GetDistance(stops[i], stops[i + 1]);
    }
    return route_info;
}

const std::unordered_map<std::string_view, const TransportCatalogue::Stop*>& TransportCatalogue::GetAllStops() const {
    return pointers_to_stops_;
}

} // end of namespace transport_catalogue