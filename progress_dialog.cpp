#include "progress_dialog.h"
#include "ui_progress_dialog.h"
#include "fetch_nse_tickers.h"
#include "config_manager.h"

#include "downloader_worker.h"

#include <QThread>
#include <QMessageBox>

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
        if (!worker)
            return;

        auto reply = QMessageBox::question(
            this,
            "Cancel Download",
            "Do you want to stop the download?",
            QMessageBox::Yes | QMessageBox::No
        );

        if (reply == QMessageBox::Yes)
        {
            worker->requestStop();
            ui->statusLabel->setText("Cancelling...");
            ui->cancelButton->setEnabled(false);
            m_cancelRequested = true;
        }
    });
}

void ProgressDialog::startDownload()
{
    m_timer.start();
    m_cancelRequested = false;

    std::vector<std::string> tickers;

    if (m_params.ticker == "All")
    {
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

    // Finished → show summary instead of direct close
    connect(worker, &DownloaderWorker::finished, this,
        [this]()
    {
        showFinalSummary();
    });

    // Cleanup
    connect(worker, &DownloaderWorker::finished, thread, &QThread::quit);
    connect(thread, &QThread::finished, worker, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    thread->start();
}

void ProgressDialog::showFinalSummary()
{
    int sec = m_timer.elapsed() / 1000;

    QString msg;

    if (m_cancelRequested)
        msg = QString("Download cancelled after %1 sec").arg(sec);
    else
        msg = QString("Download completed in %1 sec").arg(sec);

    QMessageBox::information(this, "Download Status", msg);

    accept();  // close dialog AFTER showing summary
}

void ProgressDialog::closeEvent(QCloseEvent *event)
{
    if (worker && !m_cancelRequested)
    {
        auto reply = QMessageBox::question(
            this,
            "Exit",
            "Download in progress. Cancel and exit?",
            QMessageBox::Yes | QMessageBox::No
        );

        if (reply == QMessageBox::No)
        {
            event->ignore();
            return;
        }

        worker->requestStop();
        ui->statusLabel->setText("Cancelling...");
        ui->cancelButton->setEnabled(false);
        m_cancelRequested = true;

        event->ignore();  // wait for worker to finish
        return;
    }

    event->accept();
}