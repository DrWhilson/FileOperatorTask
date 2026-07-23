#include "mainwindow.h"
#include "startupdialog.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "FileOperatorTask_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    StartupDialog startupDialog;
    if (startupDialog.exec() != QDialog::Accepted)
        return 0;

    MainWindow w(startupDialog.selectedPatterns());
    w.show();
    return QApplication::exec();
}
