#include "input_reader.h"

namespace transport_catalogue {
using namespace std;
namespace detail {

string ReadLine(istream& input) {
    string s;
    getline(input, s);
    return s;
}

int ReadLineWithNumber(istream& input) {
    int result;
    input >> result;
    ReadLine(input);
    return result;
}

} // end of namespace detail

namespace input_reader {

string_view GetHeader(string_view request) {
    int64_t space = request.find(' ');
    return request.substr(0, space);
}
    
RequestType GetRequestType(string_view request) {
    string_view request_header = GetHeader(request);
    if (request_header == "Bus"s) {
        return RequestType::BUS;
    } else if (request_header == "Stop"s) {
        return RequestType::STOP;
    }
    return RequestType::UNKNOWN;
}
    
string GetNameFromRequest(string_view& request) {
    int64_t space = request.find(' ');
    request.remove_prefix(space + 1);
    int64_t name_end = request.find(':');
    string stop_name = string(request.substr(0,name_end));
    request.remove_prefix(name_end + 2);
    return stop_name;
}
    
TransportCatalogue::Bus GetBusFromRequest(TransportCatalogue& catalogue, string_view request) {
    TransportCatalogue::Bus res;
    res.name = GetNameFromRequest(request);
    for (const string_view bus_stop : GetAllStops(request)) {
        res.stops.push_back(catalogue.GetStopByName(bus_stop));
    }
    return res;
}
    
TransportCatalogue::Stop GetStopFromRequest(string_view request) {
    string stop_name = GetNameFromRequest(request);
    int64_t comma_pos = request.find(',');
    double latitude = stod(string(request.substr(0, comma_pos)));
    request.remove_prefix(comma_pos + 2);
    double longitude = stod(string(request.substr(0)));
    return {stop_name, latitude, longitude};
}

vector<string_view> GetStopByEnumChar(string_view request, char enum_char) {
    vector<string_view> res;
    const int64_t pos_end = request.npos;
    while (true) {
        int64_t char_pos = request.find(enum_char);
        res.push_back(request.substr(0, char_pos - 1));
        if (char_pos == pos_end) {
            break;
        } else {
            request.remove_prefix(char_pos + 2);
        }
    }
    return res;
}

vector<string_view> GetAllStops(string_view request) {
    if (request.find('>') != request.npos) {
        return GetStopByEnumChar(request, '>');
    } else {
        vector<string_view> res = GetStopByEnumChar(request, '-');
        for(int i = static_cast<int>(res.size() - 2); i >= 0; --i) {
            res.push_back(res[i]);
        }
        return res;
    }
}

void RequestProcess(TransportCatalogue& catalogue, istream& input) {
    Requests requests;
    int request_num = detail::ReadLineWithNumber(input);
    string request;
    for (int i = 0; i < request_num; ++i) {
        getline(input, request);
        RequestType request_type = GetRequestType(request);
        switch (request_type) {
        case RequestType::BUS:
            requests.buses.push_back(move(request));
            break;
        case RequestType::STOP:
            catalogue.AddStop(GetStopFromRequest(request));
            requests.stops.push_back(move(request));
            break;
        default:
            cerr << "Bad request: "s + request << endl;
        }
    }
    for (const auto& add_stop_request : requests.stops) {
        SetRoadDistance(catalogue, add_stop_request);
    }
    for (const auto& add_bus_request : requests.buses) {
        catalogue.AddBus(GetBusFromRequest(catalogue, add_bus_request));
    }
}

void SetRoadDistance(TransportCatalogue& catalogue, string_view request) {
    string stop_name = GetNameFromRequest(request);
    const TransportCatalogue::Stop* src_stop = catalogue.GetStopByName(stop_name);
    int64_t comma_pos = request.find(',');
    request.remove_prefix(comma_pos + 2);
    comma_pos = request.find(',');
    if (comma_pos != request.npos) {
        request.remove_prefix(comma_pos + 2);
        while (comma_pos != request.npos) {
            int64_t m_pos = request.find('m'); // find end of distance
            int64_t dist = stoi(string(request.substr(0, m_pos)));
            request.remove_prefix(m_pos + 5); // go to stop
            comma_pos = request.find(','); // find end of name / request
            string_view dst_stop_name = request.substr(0, comma_pos);
            request.remove_prefix(comma_pos + 2);
            const TransportCatalogue::Stop* dst_stop = catalogue.GetStopByName(dst_stop_name);
            catalogue.SetDistanceBetweenStops(src_stop, dst_stop, dist);
        } 
    }
}
    
} // end of namespace input_reader
} // end of namespace transport_catalogue