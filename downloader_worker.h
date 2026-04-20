#pragma once

#include <QObject>
#include <vector>
#include <string>
#include <atomic>
#include <QStringList>

class DownloaderWorker : public QObject
{
    Q_OBJECT

public:
    DownloaderWorker(std::vector<std::string> tickers,
                     std::string interval,
                     std::string range);

    void requestStop();

public slots:
    void process();

signals:
    void progress(int value);             // % progress
    void status(const QString &msg);      // status text

    // 🔹 Updated signal with structured result
    void finished(bool cancelled, QStringList failed, int timeSec, int total, int completed);

private:
    std::vector<std::string> m_tickers;
    std::string m_interval;
    std::string m_range;

    std::atomic<bool> stopRequested{false};
};