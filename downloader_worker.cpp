#include "downloader_worker.h"

#include <QElapsedTimer>
#include <QString>

#include "downloader.h"
#include "csv_writer.h"
#include "file_utils.h"

#include <nlohmann/json.hpp>

DownloaderWorker::DownloaderWorker(std::vector<std::string> tickers,
                                   std::string interval,
                                   std::string range)
    : m_tickers(std::move(tickers)),
      m_interval(std::move(interval)),
      m_range(std::move(range))
{
}

void DownloaderWorker::requestStop()
{
    stopRequested = true;
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
        // 🔴 Cancel check
        if (stopRequested)
        {
            emit status("Cancelled");
            emit finished();
            return;
        }

        ++count;

        const std::string yahoo_ticker = t + ".NS";

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

        // ---- PROGRESS UPDATE ----
        int percent = (count * 100) / total;
        emit progress(percent);

        const double elapsed = totalTimer.elapsed() / 1000.0;
        const double avg = elapsed / count;
        const double remaining = avg * (total - count);

        const std::string msg =
            "Downloading: " + t +
            " (" + std::to_string(count) + "/" + std::to_string(total) +
            ") ETA: " + std::to_string(static_cast<int>(remaining)) + " sec";

        emit status(QString::fromStdString(msg));
    }

    // ---- FINAL MESSAGE ----
    double totalTime = totalTimer.elapsed() / 1000.0;

    std::string msg = "Done in " + std::to_string((int)totalTime) + " sec";

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

    emit progress(100);
    emit status(QString::fromStdString(msg));
    emit finished();
}