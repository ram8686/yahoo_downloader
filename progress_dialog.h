#pragma once

#include <QDialog>
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

private:
    Ui::progressDialog *ui;

    DownloaderWorker *worker;
    DownloadParams m_params;   // store input
    void setupConnections();
};