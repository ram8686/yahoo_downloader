#pragma once

#include <QDialog>
#include <QDate>

#include "config_manager.h"
#include "ui_ydl_input.h"

// Parameters collected from UI and passed to downloader
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

    // Gather current UI state into a parameter struct
    DownloadParams getParams() const;

private:
    Ui::inputDialog ui;
    ConfigManager config;

    void setupConnections();
    void populateUI();

    // Update available ranges based on selected interval
    void updateRangeOptions(const QString& interval);

    // Enforce valid date selection based on interval constraints
    void updateDateLimits();
    int maxDaysForInterval(const QString& interval);
};