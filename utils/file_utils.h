#pragma once

#include <string>

// Build output file path based on ticker and interval
std::string build_filepath(const std::string& ticker,
                           const std::string& interval);