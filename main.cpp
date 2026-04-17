#include <QApplication>
#include "main_dialog.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainDialog dialog;
    dialog.setWindowTitle("Yahoo Downloader");

    if (dialog.exec() == QDialog::Accepted)
    {
        auto params = dialog.getParams();

        // TEMP: just print / inspect
        qDebug() << "Index:" << QString::fromStdString(params.index);
        qDebug() << "Ticker:" << QString::fromStdString(params.ticker);
    }

    return 0;
}