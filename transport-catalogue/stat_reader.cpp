#include "input_reader.h"
#include "stat_reader.h"

namespace transport_catalogue {
namespace stat_reader {
using namespace std;

void RequestProcess(TransportCatalogue& catalogue, istream& input, ostream& output) {
    int request_num = detail::ReadLineWithNumber(input);
    string request;
    for (int i = 0; i < request_num; ++i) {
        getline(input, request);
        input_reader::RequestType request_type = input_reader::GetRequestType(request);
        switch (request_type) {
        case input_reader::RequestType::BUS:
            PrintBusInfo(catalogue, request.substr(4, request.npos), output);
            break;
        case input_reader::RequestType::STOP:
            PrintStop(catalogue, request.substr(5, request.npos), output);
            break;
        default:
            output << "Unable to process request: "s + request << endl;
        }
    }
}

void PrintBusInfo(TransportCatalogue& catalogue, string_view name, ostream& output) {
    const TransportCatalogue::Bus* bus = catalogue.GetBusByName(name);
    if (bus == nullptr) {
        output << "Bus "s << name << ": not found"s << endl;
    } else {
        TransportCatalogue::RouteInfo route_info = catalogue.GetRouteInfoByBus(bus);
        output << "Bus "s << name << ": "s
                        << route_info.stops_count << " stops on route, "s
                        << route_info.unique_stops_count << " unique stops, "s
                        << setprecision(6)
                        << route_info.real_route_length << " route length, "s
                        << route_info.real_route_length / route_info.route_length << " curvature"s << endl;
    }
}

void PrintStop(TransportCatalogue& catalogue, string_view name, ostream& output) {
    optional<vector<string_view>> buses = catalogue.GetBusesByStopName(name);
    if (buses) {
        if (buses.value().size() != 0) {
            output << "Stop "s << name << ": buses "s;
            for(string_view bus : buses.value()) {
                output << bus << " "s ;
            }
        } else {
            output << "Stop "s << name << ": no buses"s;
        }
        output << endl;
    } else {
        output << "Stop "s << name << ": not found"s << endl;
    }
}

} // end of namespace stat_reader
} // end of namespace transport_catalogue