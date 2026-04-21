#include "config_manager.h"

#include <fstream>
#include <sstream>

// Trim helper (whitespace on both ends)
static std::string trim(const std::string& s)
{
    const auto start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return {};

    const auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

ConfigManager::ConfigManager(const std::string& path)
{
    load(path);
}

void ConfigManager::load(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
        return;

    std::string line;
    std::string section;

    while (std::getline(file, line))
    {
        line = trim(line);

        if (line.empty() || line[0] == '#')
            continue;

        if (line.front() == '[' && line.back() == ']')
        {
            section = line;
            continue;
        }

        const auto pos = line.find('=');
        if (pos == std::string::npos)
            continue;

        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));

        if (section == "[General]")
        {
            if (key == "default_index")     m_defaultIndex = value;
            else if (key == "default_interval") m_defaultInterval = value;
            else if (key == "default_range")    m_defaultRange = value;
        }
        else if (section == "[Tickers]")
        {
            if (key == "extra")
            {
                std::stringstream ss(value);
                std::string item;

                while (std::getline(ss, item, ','))
                {
                    item = trim(item);
                    if (!item.empty())
                        m_extras.push_back(item);
                }
            }
        }
    }
}

std::string ConfigManager::defaultIndex() const
{
    return m_defaultIndex;
}

std::string ConfigManager::defaultInterval() const
{
    return m_defaultInterval;
}

std::string ConfigManager::defaultRange() const
{
    return m_defaultRange;
}

std::vector<std::string> ConfigManager::extraTickers() const
{
    return m_extras;
}