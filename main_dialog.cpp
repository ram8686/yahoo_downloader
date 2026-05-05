#include "main_dialog.h"

#include <QComboBox>
#include <QDate>
#include <QString>
#include <QRadioButton>
#include <QPushButton>

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

    updateRangeOptions(ui.intervalBox->currentText());

    // Default dates
    QDate today = QDate::currentDate();
    ui.endDateEdit->setDate(today);
    ui.startDateEdit->setDate(today.addMonths(-1));

    // Prevent future dates
    ui.endDateEdit->setMaximumDate(today);
    ui.startDateEdit->setMaximumDate(today);

    ui.tickerBox->addItem("All");

    auto tickers = buildTickerList(config.defaultIndex(), config);
    for (const auto &t : tickers)
        ui.tickerBox->addItem(QString::fromStdString(t));

    ui.tickerBox->setCurrentIndex(0);

    updateDateLimits();
}

void MainDialog::setupConnections()
{
    // Index change → reload ticker list
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

    // Interval change → update range options + date constraints
    connect(ui.intervalBox, &QComboBox::currentTextChanged, this,
        [this](const QString &interval)
    {
        updateRangeOptions(interval);
        updateDateLimits();
    });

    // Ensure start ≤ end
    connect(ui.startDateEdit, &QDateEdit::dateChanged, this,
        [this](const QDate &start)
    {
        ui.endDateEdit->setMinimumDate(start);
    });

    connect(ui.endDateEdit, &QDateEdit::dateChanged, this,
        [this](const QDate &end)
    {
        ui.startDateEdit->setMaximumDate(end);
        updateDateLimits();
    });

    // Toggle between range mode and date mode
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

    // Accept dialog (UI already enforces valid input)
    connect(ui.downloadButton, &QPushButton::clicked, this,
        [this]()
    {
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
        p.range.clear();
        p.startDate = ui.startDateEdit->date();
        p.endDate   = ui.endDateEdit->date();
    }

    return p;
}

//
// RANGE CONTROL
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

    ui.rangeBox->setCurrentIndex(2);
}

//
// DATE LIMIT LOGIC
//

int MainDialog::maxDaysForInterval(const QString& interval)
{
    if (interval == "1m") return 7;
    if (interval == "5m" || interval == "15m" || interval == "30m") return 60;
    if (interval == "1h" || interval == "60m") return 730;
    return -1;
}

void MainDialog::updateDateLimits()
{
    QString interval = ui.intervalBox->currentText();
    QDate end = ui.endDateEdit->date();

    int maxDays = maxDaysForInterval(interval);

    if (maxDays > 0)
    {
        QDate minStart = end.addDays(-maxDays);
        ui.startDateEdit->setMinimumDate(minStart);

        // Auto-correct if out of allowed range
        if (ui.startDateEdit->date() < minStart)
            ui.startDateEdit->setDate(minStart);
    }
    else
    {
        ui.startDateEdit->setMinimumDate(QDate());
    }
}