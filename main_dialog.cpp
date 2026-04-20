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

    ui.rangeBox->addItems({"1d","5d","7d","1mo","60d","3mo","6mo","1y","2y","5y","10y","ytd","max"});
    ui.rangeBox->setCurrentText(QString::fromStdString(config.defaultRange()));

    ui.startDateEdit->setDate(QDate::currentDate().addMonths(-1));
    ui.endDateEdit->setDate(QDate::currentDate());

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
        p.startDate = QDate();  // clear
        p.endDate   = QDate();  // clear
    }
    else
    {
        p.range.clear();  // 🔥 VERY IMPORTANT
        p.startDate = ui.startDateEdit->date();
        p.endDate   = ui.endDateEdit->date();
    }

    return p;
}