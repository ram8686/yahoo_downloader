#pragma once

#include <string>

// Build output file path based on ticker and interval
// (e.g., separates intraday vs EOD directories)
std::string build_filepath(const std::string& ticker,
                           const std::string& interval);