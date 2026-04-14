#include "downloader_worker.h"

#include <QElapsedTimer>

#include "downloader.h"
#include "csv_writer.h"
#include "file_utils.h"

#include <nlohmann/json.hpp>

DownloaderWorker::DownloaderWorker(std::vector<std::string> tickers,
                                   std::string interval,
                                   std::string range,
                                   std::function<void(const std::string&)> progress_cb,
                                   std::function<void(const std::string&)> done_cb)
    : m_tickers(std::move(tickers)),
      m_interval(std::move(interval)),
      m_range(std::move(range)),
      m_progress(std::move(progress_cb)),
      m_done(std::move(done_cb))
{
}

void DownloaderWorker::process()
{
    std::vector<std::string> failed;
    QElapsedTimer totalTimer;
    totalTimer.start();

    const int total = static_cast<int>(m_tickers.size());
    int count = 0;

    for (const auto& t : m_tickers)
    {
        ++count;

        // Build Yahoo ticker
        const std::string yahoo_ticker = t + ".NS";

        // Fetch and parse data
        const std::string url = build_url(yahoo_ticker, m_range, m_interval);
        const std::string response = fetch_data(url);

        auto j = nlohmann::json::parse(response);
        OHLCV data = extract_ohlcv(j);

        // ---- HANDLE FAILURE ----
        if (data.time.empty())
        {
            failed.push_back(t);
            continue;
        }

        // ---- WRITE FILE ----
        std::string filename = build_filepath(t, m_interval);
        write_csv(filename, data, t, m_interval);

        // Progress update (every 10 items or single ticker)
        if (m_progress && (count % 10 == 0 || total == 1))
        {
            const double elapsed = totalTimer.elapsed() / 1000.0;
            const double avg = elapsed / count;
            const double remaining = avg * (total - count);

            const std::string msg =
                "Downloading: " + t +
                " (" + std::to_string(count) + "/" + std::to_string(total) +
                ") ETA: " + std::to_string(static_cast<int>(remaining)) + " sec";

            m_progress(msg);
        }
    }

    // Final completion message
    double totalTime = totalTimer.elapsed() / 1000.0;

    std::string msg = "Done in " + std::to_string((int)totalTime) + " sec";

    // ---- ADD FAILED LIST ----
    if (!failed.empty())
    {
        msg += " | Failed: ";

        for (size_t i = 0; i < failed.size(); ++i)
        {
            msg += failed[i];
            if (i != failed.size() - 1)
                msg += ", ";
        }
    }

    if (m_done)
        m_done(msg);
    }