#pragma once

#include <string>
#include <vector>

// Simple configuration loader for settings.ini
class ConfigManager
{
public:
    explicit ConfigManager(const std::string& path);

    std::string defaultIndex() const;
    std::string defaultInterval() const;
    std::string defaultRange() const;

    // Additional user-defined tickers
    std::vector<std::string> extraTickers() const;

private:
    std::string m_defaultIndex = "nifty500";
    std::string m_defaultInterval = "1d";
    std::string m_defaultRange = "1wk";

    std::vector<std::string> m_extras;

    void load(const std::string& path);
};