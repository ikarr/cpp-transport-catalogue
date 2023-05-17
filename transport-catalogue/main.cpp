#include "json_reader.h"

int main() {
    using namespace catalogue;
    using namespace json_rd;
    using namespace handler;
    using namespace renderer;
    TransportCatalogue catalogue;
    MapRenderer renderer;
    RequestHandler handler(catalogue, renderer);
    ReadJSON(std::cin, std::cout, catalogue, renderer, handler);
}