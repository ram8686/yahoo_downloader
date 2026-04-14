#pragma once

#include <vector>
#include <string>

// Fetch ticker symbols for a given NSE index
std::vector<std::string> fetch_nse_tickers(const std::string& index);