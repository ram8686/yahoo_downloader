#include "progress_dialog.h"
#include "ui_progress_dialog.h"
#include "fetch_nse_tickers.h"
#include "config_manager.h"

#include "downloader_worker.h"

#include <QThread>

ProgressDialog::ProgressDialog(const DownloadParams &params, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::progressDialog),
      worker(nullptr),
      m_params(params)
{
    ui->setupUi(this);
    setupConnections();
}

ProgressDialog::~ProgressDialog()
{
    delete ui;
}

void ProgressDialog::setupConnections()
{
    connect(ui->cancelButton, &QPushButton::clicked, this, [this]()
    {
        if (worker)
            worker->requestStop();   // we’ll add this
            ui->statusLabel->setText("Cancelling...");
            ui->cancelButton->setEnabled(false);
    });
}

void ProgressDialog::startDownload()
{
    std::vector<std::string> tickers;

    if (m_params.ticker == "All")
    {
        // expand to full list
        ConfigManager config("settings.ini");

        tickers = buildTickerList(m_params.index, config);
    }
    else
    {
        tickers.push_back(m_params.ticker);
    }

    worker = new DownloaderWorker(
        tickers,
        m_params.interval,
        m_params.range
    );

    QThread *thread = new QThread();

    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker, &DownloaderWorker::process);

    // Progress update
    connect(worker, &DownloaderWorker::progress, this,
        [this](int value)
    {
        ui->progressBar->setValue(value);
    });

    // Status update
    connect(worker, &DownloaderWorker::status, this,
        [this](const QString &msg)
    {
        ui->statusLabel->setText(msg);
    });

    // Finished
    connect(worker, &DownloaderWorker::finished, this,
        [this]()
    {
        accept();   // close dialog
    });

    // Cleanup
    connect(worker, &DownloaderWorker::finished, thread, &QThread::quit);
    connect(thread, &QThread::finished, worker, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    thread->start();
}

