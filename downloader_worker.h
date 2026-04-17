#pragma once

#include <QObject>
#include <vector>
#include <string>

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
    void finished();                      // done

private:
    std::vector<std::string> m_tickers;
    std::string m_interval;
    std::string m_range;

    bool stopRequested = false;
};