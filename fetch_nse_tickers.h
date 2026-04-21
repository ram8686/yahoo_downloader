#pragma once

#include <vector>
#include <string>

#include "config_manager.h"

// Fetch ticker symbols for a given NSE index (e.g., nifty50)
std::vector<std::string> fetch_nse_tickers(const std::string& index);

// Build final ticker list including user-defined extras from config
std::vector<std::string> buildTickerList(const std::string& index, ConfigManager& config);