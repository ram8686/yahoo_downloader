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

    // 🔹 New state tracking
    bool m_cancelRequested = false;
    QElapsedTimer m_timer;

    // 🔹 Helpers
    void setupConnections();
    void showFinalSummary();
};