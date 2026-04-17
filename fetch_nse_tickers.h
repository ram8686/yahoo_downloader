#pragma once

#include <vector>
#include <string>
#include "config_manager.h"

// Fetch ticker symbols for a given NSE index
std::vector<std::string> fetch_nse_tickers(const std::string& index);

// Build ticker list along with the extras
std::vector<std::string> buildTickerList(const std::string& index, ConfigManager& config);