#pragma once
#include "domain.h"
#include "geo.h"

#include <algorithm>
#include <deque>
#include <map>
#include <unordered_map>
#include <unordered_set>

namespace catalogue {
    
namespace detail {
template <typename T>
struct PairHasher {
    std::hash<T> hasher;
    
    size_t operator()(const std::pair<T, T> pair_to_hash) const {
        return hasher(pair_to_hash.first) + hasher(pair_to_hash.second) * 37;
    }
};
} // namespace detail
    
class TransportCatalogue {
public:
    void AddStop(const Stop& stop);
    
    void AddBus(const Bus& bus);
    
    void SetDistance(const Stop* src, const Stop* dst, int64_t distance);
    
    const Stop* GetStop(std::string_view stop_name) const;
    
    const Bus* GetBus(std::string_view bus_name) const;
    
    Buses GetBusesByStop(std::string_view stop_name) const;
    
    int64_t GetDistance(const Stop* src, const Stop* dst) const;
    
    BusStat GetBusStat(const Bus* bus) const;
    
    const std::map<std::string_view, const Bus*>& GetAllBuses() const;
    
    const std::unordered_map<std::string_view, const Stop*>& GetAllStops() const;
    
    size_t GetStopsCount() const;
    
private:
    std::deque<Bus> buses_;
    std::deque<Stop> stops_;
    std::map<std::string_view, const Bus*> bus_names_to_ptrs_;
    std::unordered_map<std::string_view, const Stop*> stop_names_to_ptrs_;
    std::unordered_map<std::pair<const Stop*, const Stop*>, int64_t, detail::PairHasher<const Stop*>> stop_distances_;
};

} // namespace catalogue