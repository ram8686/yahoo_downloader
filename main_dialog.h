#pragma once

#include <QDialog>
#include <QDate>

#include "config_manager.h"
#include "ui_ydl_input.h"

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

    // 🔹 Range depends on interval
    void updateRangeOptions(const QString& interval);

    // 🔹 Date constraints (core of this change)
    void updateDateLimits();
    int maxDaysForInterval(const QString& interval);
};