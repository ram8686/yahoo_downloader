#include "downloader_worker.h"

#include <QElapsedTimer>
#include <QString>
#include <QThread>

#include "downloader.h"
#include "csv_writer.h"
#include "file_utils.h"

#include <nlohmann/json.hpp>

// ⚠️ Global state (shared across runs) — consider making this a member later
int completed = 0;

DownloaderWorker::DownloaderWorker(std::vector<std::string> tickers,
                                   std::string interval,
                                   std::string range,
                                   QDate startDate,
                                   QDate endDate)
    : m_tickers(std::move(tickers)),
      m_interval(std::move(interval)),
      m_range(std::move(range)),
      m_startDate(startDate),
      m_endDate(endDate)
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
        // Check for cancellation before starting next item
        if (stopRequested || QThread::currentThread()->isInterruptionRequested())
        {
            int sec = totalTimer.elapsed() / 1000;

            emit status("Cancelled");

            QStringList failedList;
            for (const auto &f : failed)
                failedList << QString::fromStdString(f);

            emit finished(true, failedList, sec, total, completed);
            return;
        }

        ++count;

        const std::string yahoo_ticker = t + ".NS";

        const std::string url = build_url(
            yahoo_ticker,
            m_range,
            m_interval,
            m_startDate,
            m_endDate
        );

        const std::string response = fetch_data(url);

        // Cancellation after network call (important for long requests)
        if (stopRequested)
        {
            int sec = totalTimer.elapsed() / 1000;

            emit status("Cancelled");

            QStringList failedList;
            for (const auto &f : failed)
                failedList << QString::fromStdString(f);

            emit finished(true, failedList, sec, total, completed);
            return;
        }

        nlohmann::json j;

        try
        {
            j = nlohmann::json::parse(response);
        }
        catch (...)
        {
            failed.push_back(t);
            continue;
        }

        OHLCV data = extract_ohlcv(j);

        // No usable data → treat as failure
        if (data.time.empty())
        {
            failed.push_back(t);
            continue;
        }

        // Write CSV output
        std::string filename = build_filepath(t, m_interval);
        write_csv(filename, data, t, m_interval);

        completed++;

        // Progress update (percentage)
        int percent = (count * 100) / total;
        emit progress(percent);

        // Basic ETA estimation
        const double elapsed = totalTimer.elapsed() / 1000.0;
        const double avg = elapsed / count;
        const double remaining = avg * (total - count);

        const std::string msg =
            "Downloading: " + t +
            " (" + std::to_string(count) + "/" + std::to_string(total) +
            ") ETA: " + std::to_string(static_cast<int>(remaining)) + " sec";

        emit status(QString::fromStdString(msg));
    }

    // Final completion
    int sec = totalTimer.elapsed() / 1000;

    emit progress(100);

    QStringList failedList;
    for (const auto &f : failed)
        failedList << QString::fromStdString(f);

    emit finished(false, failedList, sec, total, completed);
}