#include "config_manager.h"

#include <fstream>
#include <sstream>

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
        if (line.empty() || line[0] == '#')
            continue;

        if (line[0] == '[')
        {
            section = line;
            continue;
        }

        auto pos = line.find('=');
        if (pos == std::string::npos)
            continue;

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        if (section == "[General]")
        {
            if (key == "default_index") m_defaultIndex = value;
            if (key == "default_interval") m_defaultInterval = value;
            if (key == "default_range") m_defaultRange = value;
        }
        else if (section == "[Tickers]")
        {
            if (key == "extra")
            {
                std::stringstream ss(value);
                std::string item;

                while (std::getline(ss, item, ','))
                {
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