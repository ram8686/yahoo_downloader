#include "downloader_worker.h"

#include <QElapsedTimer>
#include <QString>
#include <QThread>

#include "downloader.h"
#include "csv_writer.h"
#include "file_utils.h"

#include <nlohmann/json.hpp>

int completed = 0;
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
        // 🔴 Cancel check (early)
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

        const std::string url = build_url(yahoo_ticker, m_range, m_interval);
        const std::string response = fetch_data(url);

        // 🔴 Cancel check (after network call)
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

        // ---- HANDLE FAILURE ----
        if (data.time.empty())
        {
            failed.push_back(t);
            continue;
        }

        // ---- WRITE FILE ----
        std::string filename = build_filepath(t, m_interval);
        write_csv(filename, data, t, m_interval);
        completed++;

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

    // ---- COMPLETION ----
    int sec = totalTimer.elapsed() / 1000;

    emit progress(100);

    QStringList failedList;
    for (const auto &f : failed)
        failedList << QString::fromStdString(f);

    emit finished(false, failedList, sec, total, completed);
}