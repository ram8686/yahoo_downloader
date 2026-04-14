#include "csv_writer.h"

#include <fstream>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>

// ---------- Helpers ----------

// Format date (YYYY-MM-DD)
static std::string format_date(long long ts)
{
    const std::time_t t = static_cast<std::time_t>(ts);
    std::tm* lt = std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(lt, "%Y-%m-%d");
    return oss.str();
}

// Format datetime (YYYY-MM-DD HH:MM:SS)
static std::string format_datetime(long long ts)
{
    const std::time_t t = static_cast<std::time_t>(ts);
    std::tm* lt = std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(lt, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

// ---------- Main Function ----------

void write_csv(const std::string& filename,
               const OHLCV& data,
               const std::string& ticker,
               const std::string& interval)
{
    std::ofstream file(filename);

    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filename << "\n";
        return;
    }

    // Header (AmiBroker format)
    file << "Ticker,Date,Open,High,Low,Close,Volume\n";

    const bool intraday = (!interval.empty() &&
                          (interval.back() == 'm' || interval.back() == 'h'));

    const size_t n = data.time.size();

    file << std::fixed << std::setprecision(2);

    for (size_t i = 0; i < n; ++i)
    {
        const std::string dt = intraday
            ? format_datetime(data.time[i])
            : format_date(data.time[i]);

        file << ticker << ","
             << dt << ","
             << data.open[i] << ","
             << data.high[i] << ","
             << data.low[i] << ","
             << data.close[i] << ","
             << data.volume[i] << "\n";
    }
}