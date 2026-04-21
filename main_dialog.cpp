#include "main_dialog.h"

#include <QComboBox>
#include <QDate>
#include <QString>
#include <QRadioButton>
#include <QPushButton>
#include <QMessageBox>

#include "config_manager.h"
#include "fetch_nse_tickers.h"
#include "ui_ydl_input.h"

MainDialog::MainDialog(QWidget *parent)
    : QDialog(parent),
      config("settings.ini")
{
    ui.setupUi(this);

    populateUI();
    setupConnections();
}

void MainDialog::populateUI()
{
    ui.indexBox->addItems({"nifty50", "nifty100", "nifty200", "nifty500"});
    ui.indexBox->setCurrentText(QString::fromStdString(config.defaultIndex()));

    ui.intervalBox->addItems({"1m","5m","15m","30m","1h","1d","5d","1wk","1mo","3mo"});
    ui.intervalBox->setCurrentText(QString::fromStdString(config.defaultInterval()));

    // 🔹 Set dynamic range based on interval
    updateRangeOptions(ui.intervalBox->currentText());

    // 🔹 FIX: default dates (end should NOT be today)
    ui.startDateEdit->setDate(QDate::currentDate().addMonths(-1));
    ui.endDateEdit->setDate(QDate::currentDate().addDays(-1));

    ui.tickerBox->addItem("All");

    auto tickers = buildTickerList(config.defaultIndex(), config);
    for (const auto &t : tickers)
        ui.tickerBox->addItem(QString::fromStdString(t));

    ui.tickerBox->setCurrentIndex(0);
}

void MainDialog::setupConnections()
{
    connect(ui.indexBox, &QComboBox::currentTextChanged, this,
        [this](const QString &text)
    {
        ui.tickerBox->clear();
        ui.tickerBox->addItem("All");

        auto tickers = buildTickerList(text.toStdString(), config);
        for (const auto &t : tickers)
            ui.tickerBox->addItem(QString::fromStdString(t));

        ui.tickerBox->setCurrentIndex(0);
    });

    // 🔹 Interval → Range dependency
    connect(ui.intervalBox, &QComboBox::currentTextChanged, this,
        [this](const QString &interval)
    {
        updateRangeOptions(interval);
    });

    connect(ui.rangeRadio, &QRadioButton::toggled, this,
        [this](bool checked)
    {
        if (checked)
            ui.stackedWidget->setCurrentIndex(0);
    });

    connect(ui.dateRadio, &QRadioButton::toggled, this,
        [this](bool checked)
    {
        if (checked)
            ui.stackedWidget->setCurrentIndex(1);
    });

    // 🔹 VALIDATION HOOK
    connect(ui.downloadButton, &QPushButton::clicked, this,
        [this]()
    {
        DownloadParams p = getParams();

        if (validateParams(p))
            accept();
    });

    connect(ui.cancelButton, &QPushButton::clicked,
            this, &QDialog::reject);
}

DownloadParams MainDialog::getParams() const
{
    DownloadParams p;

    p.index    = ui.indexBox->currentText().toStdString();
    p.ticker   = ui.tickerBox->currentText().toStdString();
    p.interval = ui.intervalBox->currentText().toStdString();

    p.useRange = ui.rangeRadio->isChecked();

    if (p.useRange)
    {
        p.range = ui.rangeBox->currentText().toStdString();
        p.startDate = QDate();
        p.endDate   = QDate();
    }
    else
    {
        p.range.clear();  // 🔥 critical
        p.startDate = ui.startDateEdit->date();
        p.endDate   = ui.endDateEdit->date();
    }

    return p;
}

//
// 🔹 NEW FUNCTIONS
//

void MainDialog::updateRangeOptions(const QString& interval)
{
    ui.rangeBox->clear();

    if (interval == "1m")
    {
        ui.rangeBox->addItems({"1d", "5d", "7d"});
    }
    else if (interval == "5m" || interval == "15m" || interval == "30m")
    {
        ui.rangeBox->addItems({"1d", "5d", "7d", "1wk", "1mo", "2mo"});
    }
    else if (interval == "1h" || interval == "60m")
    {
        ui.rangeBox->addItems({"1d", "5d", "7d", "1wk", "1mo", "2mo",
                               "3mo", "6mo", "1y", "2y"});
    }
    else
    {
        ui.rangeBox->addItems({"1d","5d","7d","1wk","1mo","2mo",
                               "3mo","6mo","1y","2y","5y","10y","ytd","max"});
    }

    ui.rangeBox->setCurrentIndex(0);
}

bool MainDialog::validateParams(const DownloadParams& p)
{
    if (!p.useRange)
    {
        QDate today = QDate::currentDate();

        if (p.startDate > p.endDate)
        {
            QMessageBox::warning(this, "Invalid Dates",
                "Start date cannot be later than end date.");
            return false;
        }

        if (p.endDate >= today)
        {
            QMessageBox::warning(this, "Invalid Dates",
                "End date cannot be today or in the future.");
            return false;
        }

        // 🔹 Intraday 60-day restriction
        int days = p.startDate.daysTo(p.endDate);

        if (p.interval == "1m" || p.interval == "2m" ||
            p.interval == "5m" || p.interval == "15m" ||
            p.interval == "30m" || p.interval == "60m" ||
            p.interval == "90m" || p.interval == "1h")
        {
            if (days > 60)
            {
                QMessageBox::warning(this, "Invalid Selection",
                    "Intraday data cannot exceed 60 days.");
                return false;
            }
        }
    }

    return true;
}