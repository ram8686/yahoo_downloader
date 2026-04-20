#include "downloader.h"

#include <curl/curl.h>
#include <fstream>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>

using json = nlohmann::json;

// ------------------ CURL CALLBACK ------------------
static size_t write_callback(void* contents, size_t size, size_t nmemb, std::string* output)
{
    const size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

// ------------------ URL ------------------
long to_epoch(const QDate& date)
{
    std::tm tm = {};
    tm.tm_year = date.year() - 1900;
    tm.tm_mon  = date.month() - 1;
    tm.tm_mday = date.day();
    tm.tm_hour = 0;
    tm.tm_min  = 0;
    tm.tm_sec  = 0;

    return std::mktime(&tm);
}

std::string build_url(const std::string& ticker,
                      const std::string& range,
                      const std::string& interval,
                      const QDate& startDate,
                      const QDate& endDate)
{
    std::string base = "https://query1.finance.yahoo.com/v8/finance/chart/" + ticker;

    // 🔹 RANGE MODE
    if (!range.empty())
    {
        return base + "?range=" + range + "&interval=" + interval;
    }

    // 🔹 DATE MODE
    long start_epoch = to_epoch(startDate);
    long end_epoch   = to_epoch(endDate);

    return base +
           "?period1=" + std::to_string(start_epoch) +
           "&period2=" + std::to_string(end_epoch) +
           "&interval=" + interval;
}

// ------------------ FETCH ------------------
std::string fetch_data(const std::string& url)
{
    std::string response;

    CURL* curl = curl_easy_init();
    if (!curl)
        return response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "User-Agent: Mozilla/5.0");
    headers = curl_slist_append(headers, "Accept: application/json");

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    const CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK)
        std::cerr << "Curl error: " << curl_easy_strerror(res) << "\n";

    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    return response;
}

// ------------------ EXTRACT ------------------
OHLCV extract_ohlcv(const json& j)
{
    OHLCV data;

    // -------- VALIDATION --------
    if (!j.contains("chart") || j["chart"].is_null())
        return data;

    const auto& chart = j["chart"];

    if (!chart.contains("result") || chart["result"].is_null())
        return data;

    const auto& resultArr = chart["result"];

    if (!resultArr.is_array() || resultArr.empty())
        return data;

    const auto& result = resultArr[0];

    if (!result.contains("indicators"))
        return data;

    const auto& indicators = result["indicators"];

    if (!indicators.contains("quote") || indicators["quote"].empty())
        return data;

    const auto& quote = indicators["quote"][0];

    // -------- ORIGINAL LOGIC CONTINUES --------

    const auto& timestamps = result["timestamp"];
    const auto& opens   = quote["open"];
    const auto& highs   = quote["high"];
    const auto& lows    = quote["low"];
    const auto& closes  = quote["close"];
    const auto& volumes = quote["volume"];

    size_t n = timestamps.size();

    for (size_t i = 0; i < n; ++i)
    {
        data.time.push_back(timestamps[i].is_null() ? 0 : timestamps[i].get<long long>());

        data.open.push_back(opens[i].is_null() ? NAN : opens[i].get<double>());
        data.high.push_back(highs[i].is_null() ? NAN : highs[i].get<double>());
        data.low.push_back(lows[i].is_null() ? NAN : lows[i].get<double>());
        data.close.push_back(closes[i].is_null() ? NAN : closes[i].get<double>());

        data.volume.push_back(volumes[i].is_null() ? 0 : volumes[i].get<long long>());
    }

    return data;
}

// ------------------ TIME FORMAT ------------------
std::string format_timestamp(long long ts)
{
    const std::time_t t = static_cast<std::time_t>(ts);
    std::tm* lt = std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(lt, "%Y-%m-%d %H:%M:%S");

    return oss.str();
}

// ------------------ CSV ------------------
void write_csv(const std::string& filename, const OHLCV& data)
{
    std::ofstream file(filename);

    file << "datetime,open,high,low,close,volume\n";

    const size_t n = data.close.size();

    for (size_t i = 0; i < n; ++i)
    {
        file << format_timestamp(data.time[i]) << ","
             << data.open[i] << ","
             << data.high[i] << ","
             << data.low[i] << ","
             << data.close[i] << ","
             << data.volume[i] << "\n";
    }
}