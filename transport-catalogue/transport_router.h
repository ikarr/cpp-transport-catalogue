#pragma once
#include "transport_catalogue.h"
#include "router.h"

#include <memory>

namespace catalogue {

enum class ItemType {
    BUS,
    WAIT
    };

struct Item {
    ItemType type;
    std::string_view name;
    double time;
    int span_count;
};

struct RouteItems {
    double total_time;
    std::vector<Item> items;
};

struct RouterSettings {
    int time;
    double velocity;
};
    
class TransportRouter {
public:
    TransportRouter(const TransportCatalogue& db);
    
    void BuildAllRoutes();
    
    void SetSettings(RouterSettings settings);
    
    std::optional<RouteItems> GetRoute(const Stop* start_stop, const Stop* finish_stop) const;
    
    RouterSettings GetSettings() const;
    
private:
    void AddRouteToGraph(const std::string_view bus_name, const Bus* bus_ptr);
    
    void CreateCarcass();
    
    void AddEdgeToItem(graph::VertexId start_vertex, graph::VertexId stop_vertex, Item item);
    
    void AddBusEdge(const Stop* start_stop, const Stop* finish_stop, std::string_view bus_name, int span, double distance);
    
    graph::VertexId GetStartWaitVertex(const Stop* stop_ptr) const;
    
    graph::VertexId GetStartBusVertex(const Stop* stop_ptr) const;
    
    const TransportCatalogue& db_;
    int bus_wait_time_;
    double bus_velocity_;
    std::unique_ptr<graph::DirectedWeightedGraph<double>> route_graph_;
    std::unique_ptr<graph::Router<double>> graph_router_;
    std::map<const Stop*, std::pair<int, int>> stop_to_vertexes_;
    std::map<graph::EdgeId, Item> edge_to_item_;
};
} // namespace catalogue