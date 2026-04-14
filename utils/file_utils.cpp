#include "file_utils.h"

#include <filesystem>
#include <ctime>
#include <sstream>
#include <iomanip>

// Return today's date as YYYY-MM-DD
static std::string today_date()
{
    const std::time_t t = std::time(nullptr);
    std::tm* lt = std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(lt, "%Y-%m-%d");

    return oss.str();
}

std::string build_filepath(const std::string& ticker,
                           const std::string& interval)
{
    std::string base;

    // Intraday vs EOD
    if (!interval.empty() &&
        (interval.back() == 'm' || interval.back() == 'h'))
    {
        base = "YahooIntraday/";
    }
    else
    {
        base = "YahooEOD/";
    }

    const std::string dir = base + today_date() + "/";

    // Create directory if it doesn't exist
    std::filesystem::create_directories(dir);

    return dir + ticker + ".csv";
}