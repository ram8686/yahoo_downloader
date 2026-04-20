#pragma once

#include <QDate>

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

// OHLCV data container
struct OHLCV
{
    std::vector<long long> time;
    std::vector<double> open, high, low, close;
    std::vector<long long> volume;
};

// Build Yahoo Finance URL
std::string build_url(const std::string& ticker,
                      const std::string& range,
                      const std::string& interval,
                      const QDate& startDate,
                      const QDate& endDate);

// Fetch raw JSON response
std::string fetch_data(const std::string& url);

// Extract OHLCV from JSON
OHLCV extract_ohlcv(const nlohmann::json& j);

// Write OHLCV data to CSV
void write_csv(const std::string& filename, const OHLCV& data);

// Format UNIX timestamp to string
std::string format_timestamp(long long ts);