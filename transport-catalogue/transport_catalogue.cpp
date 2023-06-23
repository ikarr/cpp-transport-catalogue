#include "transport_catalogue.h"

namespace catalogue {
using namespace std;

void TransportCatalogue::AddStop(const Stop& stop) {
    stops_.push_back(stop);
    stop_names_to_ptrs_[stops_.back().name] = &stops_.back();
}

void TransportCatalogue::AddBus(const Bus& bus) {
    buses_.push_back(bus);
    bus_names_to_ptrs_[buses_.back().name] = &buses_.back();
}
    
void TransportCatalogue::SetDistance(const Stop* src, const Stop* dst, int64_t distance) {
    stop_distances_[{src, dst}] = distance;
}

Buses TransportCatalogue::GetBusesByStop(string_view stop_name) const {
    if (GetStop(stop_name) == nullptr) { return {}; }
    
    vector<string_view> buses;
    for_each (bus_names_to_ptrs_.begin(), bus_names_to_ptrs_.end(),
            [stop_name, &buses](const auto bus_ptr) {
                for (const auto stop : bus_ptr.second->stops) {
                    if (stop->name == stop_name) {
                        buses.push_back(bus_ptr.second->name);
                        break;
                    }
                }
            });
    return buses;
}

const Stop* TransportCatalogue::GetStop(std::string_view stop_name) const {
    if (stop_names_to_ptrs_.count(stop_name) != 0) {
        return stop_names_to_ptrs_.at(stop_name);
    } else {
        return nullptr;
    }
}

const Bus* TransportCatalogue::GetBus(std::string_view bus_name) const {
    if (bus_names_to_ptrs_.count(bus_name) != 0) {
        return bus_names_to_ptrs_.at(bus_name);
    } else {
        return nullptr;
    }
}

int64_t TransportCatalogue::GetDistance(const Stop* src, const Stop* dst) const {
    if (stop_distances_.count({src, dst}) != 0) {
        return stop_distances_.at({src, dst});
    } else {
        return stop_distances_.at({dst, src});
    }
}
    
BusStat TransportCatalogue::GetBusStat(const Bus* bus) const {
    BusStat bus_stat;
    bus_stat.route_length = 0;
    bus_stat.real_route_length = 0;
    vector<const Stop*> stops;
    stops.insert(stops.begin(), bus->stops.begin(), bus->stops.end());
    
    if (!bus->is_roundtrip) {
        for (int i = static_cast<int>(stops.size() - 2); i >= 0; --i) {
            stops.push_back(stops[i]);
        }
    }
    
    unordered_set<const Stop*> unique_stops(stops.begin(), stops.end());
    bus_stat.stops_count = static_cast<int>(stops.size());
    bus_stat.unique_stops_count = static_cast<int>(unique_stops.size());
    
    for (size_t i = 0; i < (stops.size() - 1); ++i) {
        bus_stat.route_length += ComputeDistance(stops[i]->coordinates, stops[i + 1]->coordinates);
        bus_stat.real_route_length += GetDistance(stops[i], stops[i + 1]);
    }
    return bus_stat;
}

const std::map<std::string_view, const Bus*>& TransportCatalogue::GetAllBuses() const {
    return bus_names_to_ptrs_;
}
    
const std::unordered_map<std::string_view, const Stop*>& TransportCatalogue::GetAllStops() const {
    return stop_names_to_ptrs_;
}
    
size_t TransportCatalogue::GetStopsCount() const {
    return stops_.size();
}

} // namespace catalogue