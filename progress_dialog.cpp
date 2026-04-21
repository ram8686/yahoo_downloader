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
        }
    });
}

void ProgressDialog::startDownload()
{
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
        m_params.range,
        m_params.startDate,
        m_params.endDate
    );

    QThread *thread = new QThread();

    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker, &DownloaderWorker::process);

    connect(worker, &DownloaderWorker::progress, this,
        [this](int value)
    {
        ui->progressBar->setValue(value);
    });

    connect(worker, &DownloaderWorker::status, this,
        [this](const QString &msg)
    {
        ui->statusLabel->setText(msg);
    });

    connect(worker, &DownloaderWorker::finished, this,
        [this](bool cancelled, QStringList failed, int sec, int total, int completed)
    {
        showFinalSummary(cancelled, failed, sec, total, completed);
    });

    // Cleanup
    connect(worker, &DownloaderWorker::finished, thread, &QThread::quit);
    connect(thread, &QThread::finished, worker, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    thread->start();
}

void ProgressDialog::showFinalSummary(bool cancelled,
                                      const QStringList &failed,
                                      int sec,
                                      int total,
                                      int completed)
{
    QString msg;

    if (cancelled)
        msg = QString("Download cancelled after %1 sec").arg(sec);
    else
        msg = QString("Download completed in %1 sec").arg(sec);

    msg += QString("\n\nCompleted: %1 / %2").arg(completed).arg(total);

    if (cancelled)
    {
        int remaining = total - completed - failed.size();
        msg += QString("\nRemaining: %1").arg(remaining);
    }

    if (!failed.isEmpty())
    {
        msg += "\n\nFailed tickers:\n";
        msg += failed.join(", ");
    }

    QMessageBox::information(this, "Download Status", msg);

    accept();
}

void ProgressDialog::closeEvent(QCloseEvent *event)
{
    if (worker)
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

        event->ignore();  // wait for worker to finish
        return;
    }

    event->accept();
}