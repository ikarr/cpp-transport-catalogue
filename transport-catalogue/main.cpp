#include "request_handler.h"

int main() {
    using namespace catalogue;
    using namespace json_rd;
    using namespace handler;
    using namespace renderer;
    TransportCatalogue catalogue;
    JsonReader reader(catalogue);
    MapRenderer renderer;
    RequestHandler handler(catalogue, reader, renderer);
    handler.ReadJSON(std::cin, std::cout);
}