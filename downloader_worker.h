#pragma once

#include <vector>
#include <string>
#include <functional>

// Worker responsible for downloading data and writing CSVs
class DownloaderWorker
{
public:
    DownloaderWorker(std::vector<std::string> tickers,
                     std::string interval,
                     std::string range,
                     std::function<void(const std::string&)> progress_cb,
                     std::function<void(const std::string&)> done_cb);

    void process();

private:
    std::vector<std::string> m_tickers;
    std::string m_interval;
    std::string m_range;

    std::function<void(const std::string&)> m_progress;
    std::function<void(const std::string&)> m_done;
};