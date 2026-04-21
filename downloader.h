#pragma once

#include <QDate>

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

// Simple container for OHLCV market data
struct OHLCV
{
    std::vector<long long> time;
    std::vector<double> open, high, low, close;
    std::vector<long long> volume;
};

// Build Yahoo Finance request URL.
// Uses either 'range' OR (startDate, endDate) depending on mode.
std::string build_url(const std::string& ticker,
                      const std::string& range,
                      const std::string& interval,
                      const QDate& startDate,
                      const QDate& endDate);

// Perform HTTP request and return raw JSON response as string
std::string fetch_data(const std::string& url);

// Parse JSON response into OHLCV structure
OHLCV extract_ohlcv(const nlohmann::json& j);

// Write OHLCV data to CSV file
void write_csv(const std::string& filename, const OHLCV& data);

// Convert UNIX timestamp to formatted date string
std::string format_timestamp(long long ts);