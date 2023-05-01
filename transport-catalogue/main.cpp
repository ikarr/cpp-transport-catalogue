#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

int main() {
    transport_catalogue::TransportCatalogue catalogue;
    transport_catalogue::input_reader::RequestProcess(catalogue, std::cin);
    transport_catalogue::stat_reader::RequestProcess(catalogue, std::cin, std::cout);
}