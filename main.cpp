#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QPushButton>
#include <QString>
#include <QLabel>
#include <thread>

#include "fetch_nse_tickers.h"
#include "downloader_worker.h"
#include "config_manager.h"

// Build ticker list: NSE + extras + "All"
std::vector<std::string> buildTickerList(const std::string& index, ConfigManager& config)
{
    auto tickers = fetch_nse_tickers(index);

    // Append extra tickers from config
    const auto extras = config.extraTickers();
    for (const auto &e : extras)
        tickers.push_back(e);

    // Ensure "All" is always first
    tickers.insert(tickers.begin(), "All");

    return tickers;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Load configuration from settings.ini
    ConfigManager config("settings.ini");

    QWidget window;
    window.setWindowTitle("Yahoo Downloader");
    window.resize(400, 250);

    // -------- Layouts --------
    auto *mainLayout = new QVBoxLayout();
    auto *formLayout = new QFormLayout();

    // -------- Index Dropdown --------
    auto *indexBox = new QComboBox();
    indexBox->addItems({"nifty50", "nifty100", "nifty200", "nifty500"});
    indexBox->setCurrentText(QString::fromStdString(config.defaultIndex()));

    // -------- Ticker Dropdown --------
    auto *tickerBox = new QComboBox();

    // Initial population of tickers
    const auto initialTickers = buildTickerList(config.defaultIndex(), config);
    for (const auto &t : initialTickers)
        tickerBox->addItem(QString::fromStdString(t));

    tickerBox->setCurrentIndex(0);

    // Update tickers when index changes
    QObject::connect(indexBox, &QComboBox::currentTextChanged,
                     [&](const QString &text)
    {
        tickerBox->clear();

        const auto tickers = buildTickerList(text.toStdString(), config);
        for (const auto &t : tickers)
            tickerBox->addItem(QString::fromStdString(t));

        tickerBox->setCurrentIndex(0);
    });

    // -------- Interval Dropdown --------
    auto *intervalBox = new QComboBox();
    intervalBox->addItems({"1m", "5m", "15m", "30m", "1h", "1d", "5d", "1wk", "1mo", "3mo"});
    intervalBox->setCurrentText(QString::fromStdString(config.defaultInterval()));

    // -------- Range Dropdown --------
    auto *rangeBox = new QComboBox();
    rangeBox->addItems({"1d", "5d", "7d", "1mo", "60d", "3mo", "6mo", "1y", "2y", "5y", "10y", "ytd", "max"});
    rangeBox->setCurrentText(QString::fromStdString(config.defaultRange()));

    // -------- Form Layout --------
    formLayout->addRow("Index:", indexBox);
    formLayout->addRow("Ticker:", tickerBox);
    formLayout->addRow("Interval:", intervalBox);
    formLayout->addRow("Range:", rangeBox);

    // -------- Controls --------
    auto *downloadBtn = new QPushButton("Download");
    auto *statusLabel = new QLabel("Ready");

    // -------- Download Action --------
    QObject::connect(downloadBtn, &QPushButton::clicked, [&]()
    {
        statusLabel->setText("Preparing...");

        const std::string ticker   = tickerBox->currentText().toStdString();
        const std::string interval = intervalBox->currentText().toStdString();
        const std::string range    = rangeBox->currentText().toStdString();

        std::vector<std::string> tickers_to_download;

        // Build list for download
        if (ticker == "All")
        {
            const std::string index = indexBox->currentText().toStdString();
            const auto all_tickers = buildTickerList(index, config);

            for (const auto& t : all_tickers)
                if (t != "All")
                    tickers_to_download.push_back(t);
        }
        else
        {
            tickers_to_download.push_back(ticker);
        }

        downloadBtn->setEnabled(false);

        // Run worker in background thread
        std::thread([=]()
        {
            DownloaderWorker worker(
                tickers_to_download,
                interval,
                range,

                // Progress callback
                [&](const std::string& msg)
                {
                    QMetaObject::invokeMethod(statusLabel, [=]()
                    {
                        statusLabel->setText(QString::fromStdString(msg));
                    });
                },

                // Completion callback
                [&](const std::string& msg)
                {
                    QMetaObject::invokeMethod(statusLabel, [=]()
                    {
                        statusLabel->setText(QString::fromStdString(msg));
                        downloadBtn->setEnabled(true);
                    });
                }
            );

            worker.process();

        }).detach();
    });

    // -------- Assemble UI --------
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(downloadBtn);
    mainLayout->addWidget(statusLabel);

    window.setLayout(mainLayout);
    window.show();

    return app.exec();
}