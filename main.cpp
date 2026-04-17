#include <QApplication>
#include "main_dialog.h"
#include "progress_dialog.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainDialog dialog;
    dialog.setWindowTitle("Yahoo Downloader");

    if (dialog.exec() == QDialog::Accepted)
    {
        auto params = dialog.getParams();

        ProgressDialog progress(params);
        progress.setWindowTitle("Downloading...");

        progress.startDownload();
        progress.exec();
    }

    return 0;
}