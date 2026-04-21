#pragma once

#include <QObject>
#include <QDate>

#include <vector>
#include <string>
#include <atomic>
#include <QStringList>

// Worker that performs downloads in a background thread.
// No UI interaction — communicates via signals.
class DownloaderWorker : public QObject
{
    Q_OBJECT

public:
    DownloaderWorker(std::vector<std::string> tickers,
                     std::string interval,
                     std::string range,
                     QDate startDate,
                     QDate endDate);

    // Request graceful stop (checked inside processing loop)
    void requestStop();

public slots:
    void process();  // main execution entry (runs in worker thread)

signals:
    void progress(int value);     // 0–100
    void status(const QString &msg);

    // Final result summary
    void finished(bool cancelled,
                  QStringList failed,
                  int timeSec,
                  int total,
                  int completed);

private:
    std::vector<std::string> m_tickers;
    std::string m_interval;
    std::string m_range;
    QDate m_startDate;
    QDate m_endDate;

    std::atomic<bool> stopRequested{false};
};