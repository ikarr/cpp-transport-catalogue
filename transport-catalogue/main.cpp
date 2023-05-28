#include "request_handler.h"

int main() {
    using namespace catalogue;
    using namespace json_rd;
    using namespace handler;
    using namespace renderer;
    TransportCatalogue catalogue;
    json::Builder builder;
    JsonReader reader(catalogue);
    MapRenderer renderer;
    RequestHandler handler(catalogue, builder, reader, renderer);
    handler.ReadJSON(std::cin, std::cout);
}