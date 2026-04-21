#pragma once

#include <QDialog>
#include <QElapsedTimer>
#include <QCloseEvent>

#include "main_dialog.h"

namespace Ui {
    class progressDialog;
}

class DownloaderWorker;   // forward declaration

class ProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProgressDialog(const DownloadParams &params, QWidget *parent = nullptr);
    ~ProgressDialog();

    void startDownload();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::progressDialog *ui;

    DownloaderWorker *worker;
    DownloadParams m_params;

    void setupConnections();

    // Display final summary (success/failure/cancelled)
    void showFinalSummary(bool cancelled,
                          const QStringList &failed,
                          int sec,
                          int total,
                          int completed);
};