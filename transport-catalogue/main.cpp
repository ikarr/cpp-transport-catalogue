#include "request_handler.h"

using namespace std::literals;
using namespace catalogue;

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

void MakeBase() {
    TransportCatalogue catalogue;
    json::Builder builder;
    json_rd::JsonReader reader(catalogue);
    renderer::MapRenderer renderer;
    TransportRouter router(catalogue);
    Serializer serializer(catalogue, renderer, router);
    handler::RequestHandler handler(catalogue, builder, reader, renderer, router, serializer);
    handler.MakeBase(catalogue, std::cin, renderer, router, serializer);
}

void ProcessRequest() {
    TransportCatalogue catalogue;
    json::Builder builder;
    json_rd::JsonReader reader(catalogue);
    renderer::MapRenderer renderer;
    TransportRouter router(catalogue);
    Serializer serializer(catalogue, renderer, router);
    handler::RequestHandler handler(catalogue, builder, reader, renderer, router, serializer);
    handler.ProcessRequests(catalogue, std::cin, std::cout, router, serializer);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        PrintUsage();
        return 1;
    }
    const std::string_view mode(argv[1]);
    if (mode == "make_base"sv) {
        MakeBase();
    } else if (mode == "process_requests"sv) {
        ProcessRequest();
    } else {
        PrintUsage();
        return 1;
    }
}