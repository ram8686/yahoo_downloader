#include "fetch_nse_tickers.h"
#include "downloader.h"

#include <sstream>

// ------------------ PARSE CSV ------------------
static std::vector<std::string> parse_csv(const std::string& csv)
{
    std::vector<std::string> tickers;

    std::istringstream stream(csv);
    std::string line;

    // Skip header
    std::getline(stream, line);

    while (std::getline(stream, line))
    {
        std::stringstream ss(line);
        std::string col1, col2, symbol;

        std::getline(ss, col1, ',');   // Company Name
        std::getline(ss, col2, ',');   // Industry
        std::getline(ss, symbol, ','); // Symbol (3rd column)

        // Trim whitespace
        symbol.erase(0, symbol.find_first_not_of(" \t\r\n"));
        symbol.erase(symbol.find_last_not_of(" \t\r\n") + 1);

        // Remove quotes if present
        if (!symbol.empty() && symbol.front() == '"')
            symbol.erase(0, 1);
        if (!symbol.empty() && symbol.back() == '"')
            symbol.pop_back();

        if (!symbol.empty())
            tickers.push_back(symbol);
    }

    return tickers;
}

// ------------------ BUILD URL ------------------
static std::string get_index_url(const std::string& index)
{
    return "https://archives.nseindia.com/content/indices/ind_" +
           index + "list.csv";
}

// ------------------ MAIN FUNCTION ------------------
std::vector<std::string> fetch_nse_tickers(const std::string& index)
{
    const std::string url = get_index_url(index);

    const std::string csv = fetch_data(url);

    return parse_csv(csv);
}

// --------- Build ticker list along with the extras in settings.ini ---------
std::vector<std::string> buildTickerList(const std::string& index, ConfigManager& config)
{
    auto tickers = fetch_nse_tickers(index);

    const auto extras = config.extraTickers();
    for (const auto &e : extras)
        tickers.push_back(e);

    return tickers;
}