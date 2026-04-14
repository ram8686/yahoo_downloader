#pragma once

#include <string>
#include "downloader.h"   // For OHLCV

// Write OHLCV data to CSV (AmiBroker format)
void write_csv(const std::string& filename,
               const OHLCV& data,
               const std::string& ticker,
               const std::string& interval);