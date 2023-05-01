#pragma once

#include <iomanip>
#include <string>
#include <vector>

#include "input_reader.h"
#include "transport_catalogue.h"

namespace transport_catalogue {
namespace stat_reader {

void RequestProcess(TransportCatalogue& catalogue, std::istream& input,std::ostream& output);
    
void PrintBusInfo(TransportCatalogue& catalogue, std::string_view name, std::ostream& output);
    
void PrintStop(TransportCatalogue& catalogue, std::string_view name, std::ostream& output);

} // end of namespace stat_reader
} // end of namespace transport_catalogue