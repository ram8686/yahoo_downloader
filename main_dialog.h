#pragma once

#include <QDialog>
#include "config_manager.h"
#include "ui_ydl_input.h"   // <-- this will be generated

struct DownloadParams
{
    std::string index;
    std::string ticker;
    std::string interval;

    bool useRange;
    std::string range;

    QDate startDate;
    QDate endDate;
};

class MainDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MainDialog(QWidget *parent = nullptr);
    DownloadParams getParams() const;

private:
    Ui::inputDialog ui;
    ConfigManager config;

    void setupConnections();
    void populateUI();
};


